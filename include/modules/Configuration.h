#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <iostream>
#include <map>
#include <vector>

#include "../zk/zkTools.h"
#include "../communication/IPC.h"
#include "../utils/tinyxml.h"

using namespace std;

class Configuration{
    private:
        zkTools zktools;
        zhandle_t* zh;
        string host = "";
        IPC ipc;
        TiXmlDocument* readDoc;
        map<string, string> xmlConfig;

    public:
        Configuration();
        ~Configuration();

        map<string,string> getXmlConfig(){return this->xmlConfig;}
        
        bool xmlParse();
        bool submitJobConfig(map<string, map<string,string>> config);
        bool deleteJobConfig(string jobId);
        bool deleteAllJobConfig(vector<map<string, map<string,string>>> allJobConfig);


};

#endif