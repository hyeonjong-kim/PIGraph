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
    network->setNetwork(setting->networkType, setting->workers.size(), setting->workers, setting->port, graph->getRecvPos(), setting->mu, setting->numMutex, setting->thisHostNumber);
    processing->setInfo(graph, network, setting->superstep);
    processing->execute();
    cerr << "working!!!!!" << endl;
    return 0;
}
