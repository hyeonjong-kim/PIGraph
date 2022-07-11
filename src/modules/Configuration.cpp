#include "../../include/modules/Configuration.h"


bool Configuration::submitJobConfig(map<string,string> config){
    string host =  config.find("zk")->second;
    config.erase("zk");

    zhandle_t* zh = this->zktools.zkInit(host.c_str());
    this->zktools.zkCreatePersistent(zh, "/PiGraph/job", "job");
    string jobIdPath = "/PiGraph/job/" + config.find("jobId")->second;
    config.erase("jobId");
    this->zktools.zkCreatePersistent(zh, (char*)jobIdPath.c_str(), "jobId");
    
    map<string,string>::iterator iter;
    for(iter=config.begin(); iter!=config.end(); iter++){
        this->zktools.zkCreatePersistent(zh, (char*)(string(jobIdPath+"/"+iter->first).c_str()), (char*)(iter->second.c_str()));
    }
    
    return true;
}
