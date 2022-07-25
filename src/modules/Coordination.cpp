#include "../../include/modules/Coordination.h"
bool cmp(const pair<string, double>& a, const pair<string, double>& b) {
	if (a.second == b.second) return a.first < b.first;
    return a.second < b.second;
}

Coordination::Coordination(){
}

Coordination::~Coordination(){
    string resourcePath = "/PiGraph/Resource/CPU";
    for(int i = 0; i < this->workerManagers.size(); i++){
        this->zktools.zkDelete(this->zh, (char*)(resourcePath + "/" + workerManagers[i]).c_str());
    }
    this->zktools.zkDelete(this->zh, (char*) resourcePath.c_str());

    resourcePath = "/PiGraph/Resource/GPU";
    for(int i = 0; i < this->workerManagers.size(); i++){
        this->zktools.zkDelete(this->zh, (char*)(resourcePath + "/" + workerManagers[i]).c_str());
    }
    this->zktools.zkDelete(this->zh, (char*) resourcePath.c_str());

    char* queryPath = "/PiGraph/Query/CPU";
    for(int i = 0; i < this->workerManagers.size(); i++){
        this->zktools.zkDelete(this->zh, (char*)(string(queryPath) + workerManagers[i]).c_str());
    }
    this->zktools.zkDelete(this->zh, queryPath);

    queryPath = "/PiGraph/Query/GPU";
    for(int i = 0; i < this->workerManagers.size(); i++){
        this->zktools.zkDelete(this->zh, (char*)(string(queryPath) + workerManagers[i]).c_str());
    }
    this->zktools.zkDelete(this->zh, queryPath);
}

void Coordination::setResourceMonitoring(string zooHost, vector<string> workerManagers){
    this->zh = zktools.zkInit((char*) zooHost.c_str());
    this->workerManagers = workerManagers;
    
    string resourcePath = "/PiGraph/Resource";
    this->zktools.zkCreatePersistent(this->zh, (char*) resourcePath.c_str(), "Resource monitoring");
    this->zktools.zkCreatePersistent(this->zh, (char*) (resourcePath + "/CPU").c_str(), "Resource monitoring");

    for(int i = 0; i < this->workerManagers.size(); i++){
        char* buffer = "0";
        this->zktools.zkCreatePersistent(this->zh, (char*)(resourcePath + "/CPU" + "/" + workerManagers[i]).c_str(), buffer);
        this->resourceBuffer_CPU.insert(pair<string, char*>(workerManagers[i], buffer));
    }

    this->zktools.zkCreatePersistent(this->zh, (char*) (resourcePath + "/GPU").c_str(), "Resource monitoring");

    for(int i = 0; i < this->workerManagers.size(); i++){
        char* buffer = "0";
        this->zktools.zkCreatePersistent(this->zh, (char*)(resourcePath + "/GPU" + "/" + workerManagers[i]).c_str(), buffer);
        this->resourceBuffer_CPU.insert(pair<string, char*>(workerManagers[i], buffer));
    }

    string queryPath = "/PiGraph/Query";
    this->zktools.zkCreatePersistent(this->zh, (char*) queryPath.c_str(), "none");
    this->zktools.zkCreatePersistent(this->zh, (char*) (queryPath + "/CPU").c_str(), "none");
    for(int i = 0; i < this->workerManagers.size(); i++){
        char* buffer = "none";
        this->zktools.zkCreatePersistent(this->zh, (char*)(queryPath + "/CPU" + "/" + workerManagers[i]).c_str(), buffer);
    }

    this->zktools.zkCreatePersistent(this->zh, (char*) (queryPath + "/GPU").c_str(), "none");
    for(int i = 0; i < this->workerManagers.size(); i++){
        char* buffer = "none";
        this->zktools.zkCreatePersistent(this->zh, (char*)(queryPath + "/GPU" + "/" + workerManagers[i]).c_str(), buffer);
    }
}

void Coordination::resourceMonitoring_CPU(){
    string resourcePath = "/PiGraph/Resource/CPU/";
    for(int i = 0; i < this->workerManagers.size(); i++){
        this->zktools.zkWget(this->zh, (char*)(string(resourcePath) + workerManagers[i]).c_str(), this->resourceBuffer_CPU.find(workerManagers[i])->second);
    }

    while (true){
        this->mu.lock();
        this->resourceSort_CPU.clear();
        map<string, char*>::iterator iter;
        for(iter = this->resourceBuffer_CPU.begin(); iter != this->resourceBuffer_CPU.end(); iter++){
            this->resourceSort_CPU.push_back(pair<string, double>(iter->first, atof(iter->second)));
        }
        sort(this->resourceSort_CPU.begin(), this->resourceSort_CPU.end(), cmp);
        this->mu.unlock();
        sleep(5);
    }
}

void Coordination::resourceMonitoring_GPU(){
    string resourcePath = "/PiGraph/Resource/GPU/";
    for(int i = 0; i < this->workerManagers.size(); i++){
        this->zktools.zkWget(this->zh, (char*)(string(resourcePath) + workerManagers[i]).c_str(), this->resourceBuffer_GPU.find(workerManagers[i])->second);
    }

    while (true){   
        this->mu.lock();
        this->resourceSort_GPU.clear();
        map<string, char*>::iterator iter;
        for(iter = this->resourceBuffer_GPU.begin(); iter != this->resourceBuffer_GPU.end(); iter++){
            this->resourceSort_GPU.push_back(pair<string, double>(iter->first, atof(iter->second)));
        }
        sort(this->resourceSort_GPU.begin(), this->resourceSort_GPU.end(), cmp);
        this->mu.unlock();
        sleep(5);
    }
}

void Coordination::executionQuery_CPU(int numWorker, string jobId, string query){
    string queryPath = "/PiGraph/Query/CPU";
    for(int i = 0; i < numWorker; i++){
        this->mu.lock();
        cerr << this->resourceSort_CPU[i].first << endl;
        this->zktools.zkSet(this->zh, (char*)(queryPath + "/" + this->resourceSort_CPU[i].first).c_str(), (char*)(query+"/"+jobId).c_str());
        this->mu.unlock();
    }
}

void Coordination::executionQuery_GPU(int numWorker, string jobId, string query){
    string queryPath = "/PiGraph/Query/GPU";
    for(int i = 0; i < numWorker; i++){
        this->mu.lock();
        this->zktools.zkSet(this->zh, (char*)(queryPath + "/" + this->resourceSort_GPU[i].first).c_str(), (char*)(query+"/"+jobId).c_str());
        this->mu.unlock();
    }
}