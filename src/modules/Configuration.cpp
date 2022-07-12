#include "../../include/modules/Configuration.h"

Configuration::Configuration(){

}

Configuration::~Configuration(){
    this->zktools.zkClose(this->zh);
}


bool Configuration::submitJobConfig(map<string,string>& config){
    int resultShmId = this->ipc.createShm((key_t)190507);
    if (resultShmId == -1){
		cerr << "[ERROR]FAIL TO CREATE OR GET SHM" << endl;
        return false;
	}
    
    if(this->host == ""){
        string host =  config.find("zk")->second;
        this->zh = this->zktools.zkInit(host.c_str());
    }
    else if(this->host != config.find("zk")->second){
        this->ipc.setData(resultShmId, "[ERROR]ZOOKEEPER HOST IS DIFFERENT");
        return false;
    }

    string jobPath = "/PiGraph/job";
    this->zktools.zkCreatePersistent(this->zh, (char*) jobPath.c_str(), "job");
    
    string jobIdPath = jobPath + config.find("jobId")->second;
    this->zktools.zkCreatePersistent(this->zh, (char*)jobIdPath.c_str(), "jobId");
    
    map<string,string>::iterator iter;
    for(iter=config.begin(); iter!=config.end(); iter++){
        if(iter->first != "jobId" && iter->first != "zk"){
            this->zktools.zkCreatePersistent(this->zh, (char*)(string(jobIdPath+"/"+iter->first).c_str()), (char*)(iter->second.c_str()));
        }
    }

    this->ipc.setData(resultShmId, "[INFO]SUCCESS TO SUBMIT JOB CONFIGURATION");
    return true;
}

bool Configuration::deleteJobConfig(string joibId){

}

bool Configuration::deleteAllJobConfig(vector<map<string, string>> allJobConfig){
    map<string,string>::iterator iter;
    for(size_t i = 0; i < allJobConfig.size(); i++){
        string jobIdPath = "/PiGraph/job" + allJobConfig[i].find("jobId")->second;
        for(iter = allJobConfig[i].begin(); iter != allJobConfig[i].end(); iter++){
            if(iter->first != "jobId" && iter->first != "zk"){
                this-zktools.zkDelete(this->zh, (char*)(string(jobIdPath+"/"+iter->first).c_str()));
            }
        }
        this->zktools.zkDelete(this->zh, (char*) jobIdPath.c_str());
    }
    
    return true;
}