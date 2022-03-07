#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <cstdlib>
#include <netdb.h>
#include <sys/types.h>
#include <sys/sysinfo.h>

#include "PageRank.hpp"
#include "tcp.hpp"
#include "RDMA.hpp"
#include "ThreadPool.hpp"
#include "argparse.hpp"
#include "ResourceChecker.hpp"


class Topology{
    
    private:
        int num_thread = thread::hardware_concurrency();
        int num_mutex;
        string data_file_name;
        string host_file_name = "../hostfile/hostinfo.txt";
        string query_type;
        int num_host;
        int superstep;
        int p_option;
        char delimiter;
        int this_host_num = 0;

        map<int, PageRank> pagerank_set;
        map<int, vector<int>> recv_pos;
        double** recv_msg;

        mutex* compute_mu;
        mutex* network_mu;

        tcp* t;
        RDMA* rdma;
        
        ThreadPool* compute_threadPool;
	    ThreadPool* network_threadPool;
        

        int internalBucket;
        int externalBucket;

        struct timeval start = {};
        struct timeval end = {};


    public:
        Topology(int argc, const char *argv[]);
        bool setTopology(int argc, const char *argv[]);
        bool readingDataFile();
        bool connectTCPNetwork();
        bool connectRDMANetwork();
        bool readyToRun();
        bool run();
        bool waitThreadPool(ThreadPool* thread_pool, int num);

        vector<string> split(string input, char delimiter);
        string HostToIp(string host);
        
        int internalHashFunction(int x){return (x % this->internalBucket);}
        int externalHashFunction(int x){return (x % this->externalBucket);}

};



#endif