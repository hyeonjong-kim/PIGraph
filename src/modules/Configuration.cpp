#include "../../include/modules/Configuration.h"

Configuration::Configuration(){
    
}

Configuration::~Configuration(){
    map<string,string>::iterator iter;
    if(!this->allJobConfig.empty()){
        for(size_t i = 0; i < this->allJobConfig.size(); i++){
        string jobIdPath = "/PiGraph/job/" + this->allJobConfig[i].find("arg")->second.find("jobId")->second;
        cerr << jobIdPath << endl;
        for(iter = this->allJobConfig[i].find("arg")->second.begin(); iter != this->allJobConfig[i].find("arg")->second.end(); iter++){
            if(iter->first != "jobId"){
                this-zktools.zkDelete(this->zh, (char*)(string(jobIdPath+"/"+iter->first).c_str()));
            }
        }

        for(iter = this->allJobConfig[i].find("xml")->second.begin(); iter != this->allJobConfig[i].find("xml")->second.end(); iter++){
            if(iter->first != "zk"){
                this-zktools.zkDelete(this->zh, (char*)(string(jobIdPath+"/"+iter->first).c_str()));
            }
        }
        this->zktools.zkDelete(this->zh, (char*) jobIdPath.c_str());
        }
        this->zktools.zkDelete(this->zh, "/PiGraph/job");
    }
    
    this->zktools.zkClose(this->zh);
}

bool Configuration::submitJobConfig(map<string, map<string,string>> config){
    int resultShmId = this->ipc.createShm((key_t)190507);
    if (resultShmId == -1){
		cerr << "[ERROR]FAIL TO CREATE OR GET SHM" << endl;
        return false;
	}
    
    if(this->host == ""){
        this->host =  config.find("xml")->second.find("zk")->second;
        this->zh = this->zktools.zkInit(host.c_str());
        this->zktools.zkCreatePersistent(this->zh, "/PiGraph/job", "job");
    }
    
    else if(this->host != config.find("xml")->second.find("zk")->second){
        this->ipc.setData(resultShmId, "[ERROR]ZOOKEEPER HOST IS DIFFERENT");
        return false;
    }
    
    string jobIdPath = "/PiGraph/job/" + config.find("arg")->second.find("jobId")->second;
    this->zktools.zkCreatePersistent(this->zh, (char*)jobIdPath.c_str(), "jobId");
    
    map<string,string>::iterator iter;
    for(iter=config.find("arg")->second.begin(); iter!=config.find("arg")->second.end(); iter++){
        if(iter->first != "jobId"){
            this->zktools.zkCreatePersistent(this->zh, (char*)(string(jobIdPath+"/"+iter->first).c_str()), (char*)(iter->second.c_str()));
        }
    }

    for(iter=config.find("xml")->second.begin(); iter!=config.find("xml")->second.end(); iter++){
        if(iter->first != "zk"){
            this->zktools.zkCreatePersistent(this->zh, (char*)(string(jobIdPath+"/"+iter->first).c_str()), (char*)(iter->second.c_str()));
        }
    }
    
    this->ipc.setData(resultShmId, "[INFO]SUCCESS TO SUBMIT JOB CONFIGURATION");
    this->allJobConfig.push_back(config);
    return true;
}

bool Configuration::deleteJobConfig(string joibId){

}

bool Configuration::xmlParse(){
    this->readDoc->LoadFile("/home/hjkim/PiGraph/conf/pigraph-conf.xml");
    TiXmlElement* rootElement = this->readDoc->FirstChildElement( "configuration" );
    TiXmlElement* element = rootElement->FirstChildElement("property");
    
    while(element){
        string name = element->FirstChildElement("name")->GetText();
        string value = element->FirstChildElement("value")->GetText();
        this->xmlConfig.insert({name, value});
        element = element->NextSiblingElement("property");
    }
    
    return true;
}
