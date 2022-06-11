#include <Resourse.h>

Resource::Resource(){
    this->zh = this->zktools.zkInit("localhost.ib:2181");
    this->statFile = fopen("/proc/stat", "r");
    gethostname(this->hostName, sizeof(this->hostName));
}

double Resource::getCurrentValueCPU(){
    int idx;
    fscanf(this->statFile, "%s %d %d %d %d", this->cpuId, this->jiffies[PRESENT][USER], this->jiffies[PRESENT][USER_NICE], this->jiffies[PRESENT][SYSTEM], this->jiffies[PRESENT][IDLE]);
    for(idx = 0, this->totalJiffies = 0; idx < JIFFIES_NUM; ++idx){
        this->diffJiffies[idx] = this->jiffies[PRESENT][idx] - this->jiffies[PAST][idx];
        this->totalJiffies = this->totalJiffies + this->diffJiffies[idx];
    }
    memcpy(this->jiffies[PAST], this->jiffies[PRESENT], sizeof(int)*JIFFIES_NUM);
    return 100.0*(1.0-(this->diffJiffies[IDLE] / (double)this->totalJiffies));
}

void Resource::resourceChecker(){
    char* persistentPath = "/resource_Monitor";

    while(true){
        sleep(5);
        double cpu = this->getCurrentValueCPU();
        this->zktools.zkSet(this->zh, (char*)(string(persistentPath)+ "/" + string(this->hostName)).c_str(), (char*)to_string(cpu).c_str());
    }
}

double Resource::getCurrentValueRAM(){
    
}

void Resource::run(){
    this->resourceChecker();
}