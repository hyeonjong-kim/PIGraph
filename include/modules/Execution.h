#include <iostream>
#include <string>
#include <thread>
#include <map>

#include "../../include/zk/zkTools.h"

using namespace std;

class Execution{

    private:
        zkTools zktools;
        zhandle_t* zh;
        char hostName[256];
        
    public:
        Execution();
        ~Execution();
        void executeWorker();
};