#include <signal.h>

#include "../include/modules/Communication.h"
#include "../include/modules/Configuration.h"
#include "../include/modules/Coordination.h"

vector<map<string, map<string,string>>> jobConfigLog;

vector<string> split_simple(string& input, char delimiter) {
	vector<string> answer;
	stringstream ss(input);
    string temp;

    while (getline(ss, temp, delimiter)) {
        answer.push_back(temp);
    }

    return answer;
}

void interruptHandler(int sig){
    Configuration *configuration = new Configuration();
    configuration->deleteAllJobConfig(jobConfigLog);

    cerr << "[INFO]MASTER STOP" << endl;
    exit(0);
}

int main(int argc, const char *argv[]){
    signal(SIGINT, interruptHandler);
    Communication *communication = new Communication();
    Configuration *configuration = new Configuration();
    configuration->xmlParse();
    map<string,string> xmlConfig = configuration->getXmlConfig();
    Coordination* coordination = new Coordination(xmlConfig.find("zk")->second, split_simple(xmlConfig.find("workers")->second, ','));
    
    //thread로 바꿀 것
    coordination->resourceMonitoring();
    
    while(true){
        map<string,string> argConfig = communication->master();
        map<string, map<string,string>> config;
        config.insert({"xml", xmlConfig});
        config.insert({"arg", argConfig});
        configuration->submitJobConfig(config);
        jobConfigLog.push_back(config);
        coordination->executionQuery(stoi(argConfig.find("numWorkers")->second), argConfig.find("jobId")->second, argConfig.find("query")->second);
    }
    
    return 0;
}