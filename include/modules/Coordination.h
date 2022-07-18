#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <mutex>

#include "../zk/zkTools.h"

class Coordination{

    private:
        zkTools zktools;
        zhandle_t* zh;
        vector<string> workerManagers;
        map<string, char*> resourceBuffer;
        vector<pair<string, double>> resourceSort;
        mutex mu;

    public:
        Coordination(string zooHost, vector<string> workerManagers);
        void resourceMonitoring();
        void executionQuery(int numWorker, string jobId, string query);
};
