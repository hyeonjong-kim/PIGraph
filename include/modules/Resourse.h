#include <stdio.h>
#include <string.h>

#include "zkTools.h"

#define ONE_LINE 80
#define PAST 0
#define PRESENT 1
#define JIFFIES_NUM 4

enum jiffy{USER, USER_NICE, SYSTEM, IDLE} jiffy_enum;

using namespace std;

class Resource{

    private:
        zhandle_t* zh;
        zkTools zktools;
        char hostName[256];
        
        int jiffies[2][JIFFIES_NUM] = {0}, totalJiffies;
        int diffJiffies[JIFFIES_NUM];
        FILE* statFile;
        char cpuId[4] = {0};

    public:
        Resource();
        double getCurrentValueCPU();
        double getCurrentValueRAM();
        void resourceChecker();
        void run();
};