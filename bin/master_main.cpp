#include <signal.h>

#include "../include/modules/Communication.h"
#include "../include/modules/Configuration.h"
#include "../include/modules/Coordination.h"
#include "../include/utils/ThreadPool.h"
#include "../include/utils/Tools.h"

Communication *communication = new Communication();
Configuration *configuration = new Configuration();
Coordination *coordination = new Coordination();


vector<map<string, map<string,string>>> jobConfigLog;
ThreadPool::ThreadPool threadPool(2);

void interruptHandler(int sig){
    configuration->deleteAllJobConfig(jobConfigLog);
    cerr << "[INFO]MASTER STOP" << endl;
    exit(0);
}

int main(int argc, const char *argv[]){
    signal(SIGINT, interruptHandler);
    
    configuration->xmlParse();
    map<string,string> xmlConfig = configuration->getXmlConfig();
    Tools tools;
    coordination->setResourceMonitoring(xmlConfig.find("zk")->second, tools.split_simple(xmlConfig.find("workers")->second, ','));
    
    std::vector<std::future<void>> futures;
    auto f1 = [](){coordination->resourceMonitoring_CPU();};
    auto f2 = [](){coordination->resourceMonitoring_GPU();};
    futures.emplace_back(threadPool.EnqueueJob(f1));
    futures.emplace_back(threadPool.EnqueueJob(f2));

    while(true){
        map<string,string> argConfig = communication->master();
        map<string, map<string,string>> config;
        config.insert({"xml", xmlConfig});
        config.insert({"arg", argConfig});
        configuration->submitJobConfig(config);
        jobConfigLog.push_back(config);
        
        if(argConfig.find("processingUnit")->second == "cpu"){
            coordination->executionQuery_CPU(stoi(argConfig.find("numWorker")->second), argConfig.find("jobId")->second, argConfig.find("query")->second);
        }
        else if(argConfig.find("processingUnit")->second == "gpu"){
            coordination->executionQuery_GPU(stoi(argConfig.find("numWorker")->second), argConfig.find("jobId")->second, argConfig.find("query")->second);
        }
    }
    return 0;
}