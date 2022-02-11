#include "ResourceChecker.hpp"

void ResourceChecker::init(){
    FILE* file;
    struct tms timeSample;
    char line[128];

    lastCPU = times(&timeSample);
    lastSysCPU = timeSample.tms_stime;
    lastUserCPU = timeSample.tms_utime;

    file = fopen("/proc/cpuinfo", "r");
    numProcessors = 0;
    while(fgets(line, 128, file) != NULL){
        if (strncmp(line, "processor", 9) == 0) numProcessors++;
    }
    fclose(file);
}

int ResourceChecker::parseLine(char* line){
    int i = strlen(line);
    while (*line < '0' || *line > '9') line++;
    line[i-3] = '\0';
    i = atoi(line);
    return i;
}

void ResourceChecker::getCurrentValue(){
    struct tms timeSample;
    clock_t now;
    double percent;

    now = times(&timeSample);
    if (now <= lastCPU || timeSample.tms_stime < lastSysCPU ||
    timeSample.tms_utime < lastUserCPU){
        //오버플로우 감지
        percent = -1.0;
    }
    else{
        percent = (timeSample.tms_stime - lastSysCPU) +
            (timeSample.tms_utime - lastUserCPU);
        percent /= (now - lastCPU);
        percent /= numProcessors;
        percent *= 100;
    }
    lastCPU = now;
    lastSysCPU = timeSample.tms_stime;
    lastUserCPU = timeSample.tms_utime;

    this->cpu_percent += percent;

    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line1[128];

    while (fgets(line1, 128, file) != NULL){
        if (strncmp(line1, "VmSize:", 7) == 0){
            result = parseLine(line1);
            break;
        }
    }

    fclose(file);
    this->vm_usage += result;

    file = fopen("/proc/self/status", "r");
    result = -1;
    char line2[128];

    while (fgets(line2, 128, file) != NULL){
    if (strncmp(line2, "VmRSS:", 6) == 0){
            result = parseLine(line2);
        break;
        }
    }

    fclose(file);
    this->pm_usage += result;

    this->count++;
}

void ResourceChecker::printValue(){
    cout << "CPU usage percent: " << this->cpu_percent/this->count << "%" << endl;
    cout << "VM usage : " << this->vm_usage/this->count << "KB" << endl;
    cout << "PM usage : " << this->pm_usage/this->count << "KB" << endl;
}