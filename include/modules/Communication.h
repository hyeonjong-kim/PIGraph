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
        condition_variable cv;
        mutex mu;
    public:
        bool client(map<string, string>& config);
        map<string, string> master();
        
};

