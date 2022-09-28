#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "../include/modules/Setting.h"
#include "../include/modules/IO.h"
#include "../include/modules/Graph.h"
#include "../include/modules/Network.h"
#include "../include/modules/Processing.h"

#define pp pair<int, double>
bool cmp(const pp& a, const pp& b) {
	if (a.second == b.second) return a.first < b.first;
	return a.second > b.second;
}

using namespace std;

struct mesg_buffer {
    long mesg_type;
    char mesg_text[1024];
} message;

int main(int argc, const char *argv[]){
    Setting* setting = new Setting();
    Graph* graph = new Graph();
    Network* network = new Network();
    Processing* processing = new Processing();
    

    setting->argParse(argc, argv);
    IO* io = new IO();
    io->setHDFSInfo((char*)(setting->HDFS_host.c_str()), setting->HDFS_port);
    string readFile = io->readHDFSFile((char*)(setting->filePath.c_str()), O_RDONLY);
    graph->setInfo(setting->partitionOpt, setting->workers.size(), setting->thisHostNumber);
    graph->createGraph(readFile);
    network->setNetwork(setting->networkType, setting->workers.size(), setting->workers, setting->port, graph->getRecvPos(), setting->mu, setting->numMutex, setting->thisHostNumber, graph->getMsgBuffer());
    
    processing->setInfo(graph, network, setting->superstep, setting->numThread);
    processing->execute();
    
    /*
    map<int,double> finalResult;
    for (size_t i = 0; i < graph->getNumVertex(); i++){
        finalResult.insert({graph->getVertices()[i].vertexID, graph->getVertices()[i].vertexValue});
    }
    vector<pp> vec(finalResult.begin(), finalResult.end());
    sort(vec.begin(), vec.end(), cmp);
    for(auto num:vec){
        cerr << num.first << ": " << num.second << endl;
    }
    */

    cerr << "working!!!!!" << endl;
    return 0;
}
