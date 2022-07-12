#include <signal.h>

#include "../include/modules/Communication.h"
#include "../include/modules/Configuration.h"

vector<map<string,string>> jobConfigLog;
Communication *communication = new Communication();
Configuration *configuration = new Configuration();

void interruptHandler(int sig){
    configuration->deleteAllJobConfig(jobConfigLog);
    delete communication;
    delete configuration;

    cerr << "[INFO]MASTER STOP" << endl;
    exit(0);
}

int main(int argc, const char *argv[]){
    signal(SIGINT, interruptHandler);
    while(true){
        map<string,string> config = communication->master();
        configuration->submitJobConfig(config);
        jobConfigLog.push_back(config);
    }
    
    return 0;
}