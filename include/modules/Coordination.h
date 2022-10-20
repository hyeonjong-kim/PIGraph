#ifndef COORDINATION_H
#define COORDINATION_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <mutex>
#include <arpa/inet.h>
#include <netdb.h>

#include <sys/wait.h>
#include <fcntl.h>

#include "../zk/zkTools.h"
#include "../utils/Tools.h"

class Coordination{

    private:
        zkTools zktools;
        zhandle_t* zh;
        vector<string> workerManagers;
        map<string, char*> resourceBuffer_CPU;
        map<string, char*> resourceBuffer_GPU;
        vector<pair<string, double>> resourceSort_CPU;
        vector<pair<string, double>> resourceSort_GPU;
        vector<map<string, map<string,string>>>* allJobConfig;
        mutex mu;
        Tools tools;

    public:
        Coordination();
        ~Coordination();
        void setResourceMonitoring(string zooHost, vector<string> workerManagers);
        void resourceMonitoring();
        void wakeMonitoring();
        bool addNode(string nodeName);
        void executionQuery_CPU(int numWorker, string jobId);
        void executionQuery_GPU(int numWorker, string jobId);
        
        string HostToIp(string host){
            hostent* _hostname = gethostbyname(host.c_str());
            if(_hostname)
                return string(inet_ntoa(**(in_addr**)_hostname->h_addr_list));
            return {};
        }
        
        bool ping(string ip){
            int null, bak, statval;
            null = open("/dev/null", O_WRONLY);
            bak = dup(STDOUT_FILENO);
            dup2(null, STDOUT_FILENO);
            close(null);
            if(fork() == 0) {
                execl("/bin/ping", "ping", "-c1 -w1", (char*)ip.c_str(),  NULL);
                exit(1);
            }
            else{
                // restore stdout
                dup2(bak, STDOUT_FILENO);
                close(bak);
                wait(&statval);
                if(WIFEXITED(statval) && WEXITSTATUS(statval) == 0)
                    return true;
                else
                    return false;
            }
        }
};

#endif