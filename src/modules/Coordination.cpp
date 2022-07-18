#include "../../include/modules/Coordination.h"

bool cmp(const pair<string, double>& a, const pair<string, double>& b) {
	if (a.second == b.second) return a.first < b.first;
    return a.second < b.second;
}

Coordination::Coordination(string zooHost, vector<string> workerManagers){
    this->zh = zktools.zkInit((char*) zooHost.c_str());
    this->workerManagers = workerManagers;

    string resourcePath = "/PiGraph/Resource";
    this->zktools.zkCreatePersistent(this->zh, (char*) resourcePath.c_str(), "Resource monitoring");

    for(int i = 0; i < this->workerManagers.size(); i++){
        char* buffer = "0";
        this->zktools.zkCreatePersistent(this->zh, (char*)(resourcePath + "/" + workerManagers[i]).c_str(), buffer);
        this->resourceBuffer.insert(pair<string, char*>(workerManagers[i], buffer));
    }

    char* queryPath = "/PiGraph/Query";
    this->zktools.zkCreatePersistent(this->zh, queryPath, "active");
    for(int i = 0; i < this->workerManagers.size(); i++){
        char* buffer = "none";
        this->zktools.zkCreatePersistent(this->zh, (char*)(string(queryPath) + workerManagers[i]).c_str(), buffer);
    }
}

void Coordination::resourceMonitoring(){
    string resourcePath = "/PiGraph/Resource";
    for(int i = 0; i < this->workerManagers.size(); i++){
        this->zktools.zkWget(this->zh, (char*)(string(resourcePath) + workerManagers[i]).c_str(), this->resourceBuffer.find(workerManagers[i])->second);
    }

    while (true){   
        sleep(5);
        this->mu.lock();
        this->resourceSort.clear();
        map<string, char*>::iterator iter;
        for(iter = this->resourceBuffer.begin(); iter != this->resourceBuffer.end(); iter++){
            this->resourceSort.push_back(pair<string, double>(iter->first, atof(iter->second)));
        }
        sort(this->resourceSort.begin(), this->resourceSort.end(), cmp);
        this->mu.unlock();
    }
}

void Coordination::executionQuery(int numWorker, string jobId, string query){
    
    string queryPath = "/PiGraph/Query";
    for(int i = 0; i < numWorker; i++){
        this->mu.lock();
        this->zktools.zkSet(this->zh, (char*)(queryPath + "/" + this->resourceSort[i].first).c_str(), (char*)(query+"/"+jobId).c_str());
        this->mu.unlock();
    }
}