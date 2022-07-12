#include "../../include/modules/Configuration.h"


bool Configuration::submitJobConfig(map<string,string>& config){
    string host =  config.find("zk")->second;
    zhandle_t* zh = this->zktools.zkInit(host.c_str());

    string jobPath = "/PiGraph/job";
    this->zktools.zkCreatePersistent(zh, (char*) jobPath.c_str(), "job");
    
    string jobIdPath = jobPath + config.find("jobId")->second;
    this->zktools.zkCreatePersistent(zh, (char*)jobIdPath.c_str(), "jobId");
    
    map<string,string>::iterator iter;
    for(iter=config.begin(); iter!=config.end(); iter++){
        if(iter->first != "jobId" && iter->first != "zk"){
            this->zktools.zkCreatePersistent(zh, (char*)(string(jobIdPath+"/"+iter->first).c_str()), (char*)(iter->second.c_str()));
        }
    }
    this->zktools.zkClose(zh);

    return true;
}

bool Configuration::deleteJobConfig(string joibId){

}

bool Configuration::deleteAllJobConfig(vector<map<string, string>> allJobConfig){
    map<string,string>::iterator iter;
    for(size_t i = 0; i < allJobConfig.size(); i++){
        string host =  allJobConfig[i].find("zk")->second;
        zhandle_t* zh = this->zktools.zkInit(host.c_str());
        string jobIdPath = "/PiGraph/job" + allJobConfig[i].find("jobId")->second;

        for(iter = allJobConfig[i].begin(); iter != allJobConfig[i].end(); iter++){
            if(iter->first != "jobId" && iter->first != "zk"){
                this-zktools.zkDelete(zh, (char*)(string(jobIdPath+"/"+iter->first).c_str()));
            }
        }
        this->zktools.zkDelete(zh, (char*) jobIdPath.c_str());
        this->zktools.zkClose(zh);
    }
    
    return true;
}