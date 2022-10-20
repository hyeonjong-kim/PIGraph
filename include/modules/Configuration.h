#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

#include "../zk/zkTools.h"
#include "../communication/IPC.h"
#include "../utils/tinyxml.h"
#include "../utils/Tools.h"

using namespace std;

class Configuration{
    private:
        zkTools zktools;
        Tools tools;
        zhandle_t* zh;
        string host = "";
        IPC ipc;
        TiXmlDocument* readDoc = new TiXmlDocument();
        map<string, string> xmlConfig;
        vector<map<string, map<string,string>>> allJobConfig;
        vector<string> jobIds;
        

    public:
        Configuration();
        ~Configuration();

        map<string,string> getXmlConfig(){return this->xmlConfig;}
        bool xmlParse();
        bool submitJobConfig(map<string, map<string,string>> config);
        bool killJob(string jobId);
        void checkJobState(string jobId);
};

#endif