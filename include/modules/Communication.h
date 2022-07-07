#include <iostream>
#include <map>
#include <string>
#include <mutex>
#include <condition_variable>

#include "../communication/IPC.h"

using namespace std;

class Communication{

    private:
        IPC ipc;
        mutex mu;
        condition_variable cv;

    public:
        bool client(map<string, string>& config);
        map<string, string>& master();
};