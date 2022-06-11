#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <mutex>

#include "zkTools.h"

class Propagation{

    private:
        zkTools zktools;
        zhandle_t* zh;
        vector<string> workerManagers;
        map<string, char*> resourceBuffer;
        vector<pair<string, double>> resourceSort;
        mutex mu;

    public:
        Propagation();

        void run();
        void resourceMonitoring();
        void propagateQuery();
        

};
