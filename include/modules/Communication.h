#include <iostream>
#include <map>
#include <string>

#include "../communication/IPC.h"

using namespace std;

class Communication{

    private:
        IPC ipc;
    public:
        bool client(map<string, string>& config);
        bool master();
};