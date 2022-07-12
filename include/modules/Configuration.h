#include <iostream>
#include <map>
#include <vector>

#include "../zk/zkTools.h"

using namespace std;

class Configuration{
    private:
        zkTools zktools;
    public:
        bool submitJobConfig(map<string,string>& config);
        bool deleteJobConfig(string jobId);
        bool deleteAllJobConfig(vector<map<string, string>> allJobConfig);
};