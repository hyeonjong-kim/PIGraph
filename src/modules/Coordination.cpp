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

    string queryPath = "/PiGraph/Query/CPU";
    for(int i = 0; i < this->workerManagers.size(); i++){
        this->zktools.zkDelete(this->zh, (char*)(queryPath + "/" + workerManagers[i]).c_str());
    }
    this->zktools.zkDelete(this->zh, (char*) queryPath.c_str());

    queryPath = "/PiGraph/Query/GPU";
    for(int i = 0; i < this->workerManagers.size(); i++){
        this->zktools.zkDelete(this->zh, (char*)(queryPath + "/" + workerManagers[i]).c_str());
    }
    this->zktools.zkDelete(this->zh, (char*) queryPath.c_str());
    
    this->zktools.zkDelete(this->zh,  "/PiGraph/Resource");
    this->zktools.zkDelete(this->zh, "/PiGraph/Query");
    
    this->zktools.zkClose(this->zh);
}

void Coordination::setResourceMonitoring(string zooHost, vector<string> workerManagers){
    this->zh = zktools.zkInit((char*) zooHost.c_str());
    this->zktools.zkCreatePersistent(this->zh, "/PiGraph", "PiGraph");
    this->workerManagers = workerManagers;

    string resourcePath = "/PiGraph/Resource";
    this->zktools.zkCreatePersistent(this->zh, (char*) resourcePath.c_str(), "Resource monitoring");
    this->zktools.zkCreatePersistent(this->zh, (char*) (resourcePath + "/CPU").c_str(), "Resource monitoring");

    for(int i = 0; i < this->workerManagers.size(); i++){
        char* buffer = "0.0";
        this->zktools.zkCreatePersistent(this->zh, (char*)(resourcePath + "/CPU" + "/" + workerManagers[i]).c_str(), buffer);
    }

    this->zktools.zkCreatePersistent(this->zh, (char*) (resourcePath + "/GPU").c_str(), "Resource monitoring");
    for(int i = 0; i < this->workerManagers.size(); i++){
        char* buffer = "0.0";
        this->zktools.zkCreatePersistent(this->zh, (char*)(resourcePath + "/GPU" + "/" + workerManagers[i]).c_str(), buffer);
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

void Coordination::resourceMonitoring(){
    string resourcePath = "/PiGraph/Resource/GPU/";
    for(int i = 0; i < this->workerManagers.size(); i++){
        char* buffer = new char[512];
        this->zktools.zkWget(this->zh, (char*)(string(resourcePath) + this->workerManagers[i]).c_str(), buffer);
        this->resourceBuffer_GPU.insert(pair<string, char*>(this->workerManagers[i], buffer));
    }

    resourcePath = "/PiGraph/Resource/CPU/";
    for(int i = 0; i < this->workerManagers.size(); i++){
        char* buffer = new char[512];
        this->zktools.zkWget(this->zh, (char*)(string(resourcePath) + this->workerManagers[i]).c_str(), buffer);
        this->resourceBuffer_CPU.insert(pair<string, char*>(this->workerManagers[i], buffer));
    }

    while (true){   
        this->mu.lock();
        this->resourceSort_GPU.clear();
        map<string, char*>::iterator iter;
        for(iter = this->resourceBuffer_GPU.begin(); iter != this->resourceBuffer_GPU.end(); iter++){
            this->resourceSort_GPU.push_back(pair<string, double>(iter->first, atof(iter->second)));
        }
        sort(this->resourceSort_GPU.begin(), this->resourceSort_GPU.end(), cmp);

        this->resourceSort_CPU.clear();
        for(iter = this->resourceBuffer_CPU.begin(); iter != this->resourceBuffer_CPU.end(); iter++){
            this->resourceSort_CPU.push_back(pair<string, double>(iter->first, atof(iter->second)));
        }
        sort(this->resourceSort_CPU.begin(), this->resourceSort_CPU.end(), cmp);
        this->mu.unlock();

        sleep(5);
    }
}

void Coordination::executionQuery_CPU(int numWorker, string jobId){
    string queryPath = "/PiGraph/Query/CPU";
    this->mu.lock();
    vector<string> workers;
    string workers_str = "";
    for(int i = 0; i < numWorker; i++){
        workers.push_back(this->resourceSort_CPU[i].first);
        if(i!=numWorker)workers_str += this->resourceSort_CPU[i].first + ",";
    }
    this->zktools.zkSet(this->zh, (char*)("PiGraph/job/" + jobId + "workers").c_str(), (char*)(workers_str.c_str()));
    for (size_t i = 0; i < workers.size(); i++){
        this->zktools.zkSet(this->zh, (char*)(queryPath + "/" + workers[i]).c_str(), (char*)(jobId).c_str());
    }
    this->mu.unlock();
    
    /*
    for(int i = 0; i < numWorker; i++){
        this->mu.lock();
        cerr << this->resourceSort_CPU[i].first << " " << this->resourceSort_CPU[i].second << endl;
        this->zktools.zkSet(this->zh, (char*)(queryPath + "/" + this->resourceSort_CPU[i].first).c_str(), (char*)(jobId).c_str());
        this->mu.unlock();
    }
    */
}

void Coordination::executionQuery_GPU(int numWorker, string jobId){
    string queryPath = "/PiGraph/Query/GPU";
    for(int i = 0; i < numWorker; i++){
        this->mu.lock();
        this->zktools.zkSet(this->zh, (char*)(queryPath + "/" + this->resourceSort_GPU[i].first).c_str(), (char*)(jobId).c_str());
        this->mu.unlock();
    }
}