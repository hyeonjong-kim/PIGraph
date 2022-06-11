#include <iostream>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include "zkTools.h"

using namespace std;

class Resource{

    private:
        static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;

    public:
        Resource();
        double getCurrentValueCPU();
        double getCurrentValueRAM();
        void run();
};