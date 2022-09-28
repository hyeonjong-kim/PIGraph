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
    
    struct timeval start_total = {};
    struct timeval end_total = {};
    struct timeval start_read_file = {};
    struct timeval end_read_file = {};
    struct timeval start_create_graph = {};
    struct timeval end_create_graph = {};
    struct timeval start_network_set = {};
    struct timeval end_network_set = {};
    struct timeval start_query = {};
    struct timeval end_query = {};

    gettimeofday(&start_total, NULL);
    setting->argParse(argc, argv);
    IO* io = new IO();
    
    gettimeofday(&start_read_file, NULL);
    io->setHDFSInfo((char*)(setting->HDFS_host.c_str()), setting->HDFS_port);
    string readFile = io->readHDFSFile((char*)(setting->filePath.c_str()), O_RDONLY);
    gettimeofday(&end_read_file, NULL);

    gettimeofday(&start_create_graph, NULL);
    graph->setInfo(setting->partitionOpt, setting->workers.size(), setting->thisHostNumber);
    graph->createGraph(readFile);
    gettimeofday(&end_create_graph, NULL);
    
    gettimeofday(&start_network_set, NULL);
    network->setNetwork(setting->networkType, setting->workers.size(), setting->workers, setting->port, graph->getRecvPos(), setting->mu, setting->numMutex, setting->thisHostNumber, graph->getMsgBuffer());
    gettimeofday(&end_network_set, NULL);

    gettimeofday(&start_query, NULL);
    processing->setInfo(graph, network, setting->superstep, setting->numThread);
    processing->execute();
    gettimeofday(&end_query, NULL);
    gettimeofday(&end_total, NULL);

    double time_total = end_total.tv_sec + end_total.tv_usec / 1000000.0 - start_total.tv_sec - start_total.tv_usec / 1000000.0;
    double time_read_file = end_read_file.tv_sec + end_read_file.tv_usec / 1000000.0 - start_read_file.tv_sec - start_read_file.tv_usec / 1000000.0;
    double time_create_graph = end_create_graph.tv_sec + end_create_graph.tv_usec / 1000000.0 - start_create_graph.tv_sec - start_create_graph.tv_usec / 1000000.0;
    double time_network_setting = end_network_set.tv_sec + end_network_set.tv_usec / 1000000.0 - start_network_set.tv_sec - start_network_set.tv_usec / 1000000.0;
    double time_query = end_query.tv_sec + end_query.tv_usec / 1000000.0 - start_query.tv_sec - start_query.tv_usec / 1000000.0;
    
    cerr << "Total execution time: " << time_total << endl;
    cerr << "Time of Reading file: " << time_read_file << endl;
    cerr << "Time of creating graph: " << time_create_graph << endl;
    cerr << "Time of setting network: " << time_network_setting << endl;
    cerr << "Time of processing graph: " << time_query << endl;
    
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
