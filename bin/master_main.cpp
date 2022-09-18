#include <signal.h>

#include "../include/modules/Communication.h"
#include "../include/modules/Configuration.h"
#include "../include/modules/Coordination.h"
#include "../include/utils/ThreadPool.h"
#include "../include/utils/Tools.h"

Communication *communication = new Communication("master");
Configuration *configuration = new Configuration();
Coordination *coordination = new Coordination();
Tools tools;

vector<map<string, map<string,string>>> jobConfigLog;
ThreadPool::ThreadPool* threadPool =  new ThreadPool::ThreadPool(1);

void interruptHandler(int sig){
    delete configuration;
    delete coordination;
    delete communication;
    cerr << "[INFO]MASTER STOP" << endl;
    exit(0);
}

int main(int argc, const char *argv[]){
    signal(SIGINT, interruptHandler);
    signal(SIGKILL, interruptHandler);
    signal(SIGSTOP, interruptHandler);
    signal(SIGTERM, interruptHandler);

    configuration->xmlParse();
    map<string,string> xmlConfig = configuration->getXmlConfig();
    coordination->setResourceMonitoring(xmlConfig.find("zk")->second, tools.split_simple(xmlConfig.find("workers")->second, ','));
    
    std::vector<std::future<void>> futures;
    auto f1 = [](){coordination->resourceMonitoring();};
    futures.emplace_back(threadPool->EnqueueJob(f1));

    while(true){
        const map<string,string> argConfig = communication->master();
        map<string, map<string,string>> config;
        config.insert({"xml", xmlConfig});
        config.insert({"arg", argConfig});
        configuration->submitJobConfig(config);

        if(argConfig.find("processingUnit")->second == "cpu"){
            coordination->executionQuery_CPU(stoi(argConfig.find("numWorker")->second), argConfig.find("jobId")->second);
        }
        else if(argConfig.find("processingUnit")->second == "gpu"){
            coordination->executionQuery_GPU(stoi(argConfig.find("numWorker")->second), argConfig.find("jobId")->second);
        }
    }
    return 0;
}