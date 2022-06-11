#include "Propagation.h"

bool cmp(const pair<string, double>& a, const pair<string, double>& b) {
	if (a.second == b.second) return a.first < b.first;
    return a.second > b.second;
}

Propagation::Propagation(){
    this->zh = this->zktools.zkInit("localhost.ib:2181");
}


void Propagation::run(){
    this->resourceMonitoring();
}

void Propagation::resourceMonitoring(){
    char* persistentPath = "/resource_Monitor";

    if(this->zktools.zkExists(this->zh, persistentPath)){
       this->zktools.zkDelete(this->zh, persistentPath); 
    }
    
    this->zktools.zkCreatePersistent(this->zh, persistentPath, "active");
    
    for(int i = 0; i < this->workerManagers.size(); i++){
        char* buffer = "0";
        this->zktools.zkCreateEphemeral(this->zh, (char*)(string(persistentPath) + "/" + workerManagers[i]).c_str(), buffer);
        this->resourceBuffer.insert(pair<string, char*>(workerManagers[i], buffer));
        this->zktools.zkWget(this->zh, (char*)(string(persistentPath) + workerManagers[i]).c_str(), this->resourceBuffer.find(workerManagers[i])->second);
    }

    while (true)
    {   
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

void Propagation::propagateQuery(){
    /*
    char* persistentPath = "/Query";

     if(this->zktools.zkExists(this->zh, persistentPath)){
        this->zktools.zkDelete(this->zh, persistentPath); 
    }
    
    this->zktools.zkCreatePersistent(this->zh, persistentPath, "active");
    for(int i = 0; i < this->workerManagers.size(); i++){
        char* buffer = "none";
        this->zktools.zkCreateEphemeral(this->zh, (char*)(string(persistentPath) + workerManagers[i]).c_str(), buffer);
    }

    this->mu.lock();
    this->mu.unlock();
    */
}