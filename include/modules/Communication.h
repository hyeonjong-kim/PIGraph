#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <iostream>
#include <map>
#include <string>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <thread>
#include <chrono>

#include "../communication/IPC.h"

using namespace std;

class Communication{

    private:
        IPC ipc;
    public:
        bool client(map<string, string>& config);
        map<string, string> master();
        
};

#endif