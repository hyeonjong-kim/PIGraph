#ifndef RESOURCECHECKER_H
#define RESOURCECHECKER_H

#include <iostream>

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "sys/times.h"
#include "sys/vtimes.h"

using namespace std;

class ResourceChecker{
    
    private:
        clock_t lastCPU, lastSysCPU, lastUserCPU;
        int numProcessors;
        int count = 0;
        double cpu_percent = 0;
        int vm_usage = 0;
        int pm_usage = 0;

    public:
        ResourceChecker(){}
        
        void init();
        int parseLine(char* line);
        void getCurrentValue();
        void printValue();
};

#endif