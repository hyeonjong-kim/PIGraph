#ifndef RESOURSE_H
#define RESOURSE_H

#include <stdio.h>
#include <string.h>

#include "../zk/zkTools.h"

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
        ~Resource();
        double getCurrentValueCPU();
        void resourceChecker();
};

Resource::Resource(){
    this->zh = this->zktools.zkInit("localhost.ib:2181");
    gethostname(this->hostName, sizeof(this->hostName));
    cerr << this->hostName << endl;

}

Resource::~Resource(){
    this->zktools.zkClose(this->zh);
}

double Resource::getCurrentValueCPU(){
    int idx;
    statFile = fopen("/proc/stat", "r");
    fscanf(this->statFile, "%s %d %d %d %d", this->cpuId, &this->jiffies[PRESENT][USER], &this->jiffies[PRESENT][USER_NICE], &this->jiffies[PRESENT][SYSTEM], &this->jiffies[PRESENT][IDLE]);
    for(idx = 0, this->totalJiffies = 0; idx < JIFFIES_NUM; ++idx){
        this->diffJiffies[idx] = this->jiffies[PRESENT][idx] - this->jiffies[PAST][idx];
        this->totalJiffies = this->totalJiffies + this->diffJiffies[idx];
    }
    memcpy(this->jiffies[PAST], this->jiffies[PRESENT], sizeof(int)*JIFFIES_NUM);
    fclose(statFile);
    return 100.0*(1.0-(this->diffJiffies[IDLE] / (double)this->totalJiffies));
}

void Resource::resourceChecker(){
    char* persistentPath = "/PiGraph/Resource/CPU";

    while(true){
        double cpu = this->getCurrentValueCPU();
        this->zktools.zkSet(this->zh, (char*)(string(persistentPath)+ "/" + string(this->hostName)).c_str(), (char*)to_string(cpu).c_str());
        sleep(5);
    }
}

#endif