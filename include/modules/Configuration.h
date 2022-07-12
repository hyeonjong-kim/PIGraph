#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <iostream>
#include <map>
#include <vector>

#include "../zk/zkTools.h"
#include "../communication/IPC.h"

using namespace std;

class Configuration{
    private:
        zkTools zktools;
        zhandle_t* zh;
        string host = "";
        IPC ipc;

    public:
        Configuration();
        ~Configuration();
        bool submitJobConfig(map<string,string>& config);
        bool deleteJobConfig(string jobId);
        bool deleteAllJobConfig(vector<map<string, string>> allJobConfig);
};

#endif