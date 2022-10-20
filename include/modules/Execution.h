#ifndef EXECUTION_H
#define EXECUTION_H

#include <iostream>
#include <string>
#include <thread>
#include <map>
#include <sys/types.h>
#include <signal.h>
#include <mutex>

#include "../zk/zkTools.h"
#include "../utils/Tools.h"
#include "../utils/ThreadPool.h"

using namespace std;

class Execution{

    private:
        Tools tools;
        ThreadPool::ThreadPool* threadPool = new ThreadPool::ThreadPool(16);
        zkTools zktools;
        zhandle_t* zh;
        char hostName[256];
        map<string, string> jobPid;
        mutex mu;
        
    public:
        Execution();
        ~Execution();
        void executeWorker();
        void monitoringExecution(string jobID);
        string getPidList();
};

Execution::Execution(){
    this->zh = this->zh = this->zktools.zkInit("localhost.ib:2181");
    gethostname(this->hostName, sizeof(this->hostName));
}

Execution::~Execution(){
    this->zktools.zkClose(this->zh);
}

void Execution::executeWorker(){
    string persistentPath = "/PiGraph/Query/CPU/" + string(this->hostName);
    char* buffer = new char[512];
    char* previousBuffer = new char[512];
    char* znodeValue = new char[512];
    
    memset(previousBuffer, 0, 512);
    this->zktools.zkWget(this->zh,(char*)persistentPath.c_str(), buffer);
    memset(buffer, 0, 512);
    String_vector* strings = new String_vector;
    while(true){
        if(strcmp(buffer, previousBuffer) !=0){
            map<string, string> executionOpt;
            string jobPath = "/PiGraph/job/"+string(buffer);
            this->zktools.zkGetChildern(this->zh, (char*)jobPath.c_str(), strings);
            for (size_t i = 0; i < strings->count; i++){
                memset(znodeValue, 0, 512);
                if(this->zktools.zkGet(this->zh, (char*)(jobPath+"/"+string(strings->data[i])).c_str(), znodeValue)){
                    executionOpt.insert({string(strings->data[i]), string(znodeValue)});
                }
            }

            string executionCl = string("nohup ") + string("./worker") + string(" -s ") + executionOpt.find("superstep")->second + string(" -P ") + executionOpt.find("port")->second + string(" -f ") + executionOpt.find("filePath")->second 
                                + string(" -p ") + executionOpt.find("patitionOpt")->second + string(" -q ") + executionOpt.find("query")->second + string(" -H ") + executionOpt.find("HDFS")->second 
                                + string(" -u ") + executionOpt.find("processingUnit")->second + string(" -m ") + executionOpt.find("mutex")->second + string(" -w ") + executionOpt.find("workers")->second 
                                + string(" -t ") + executionOpt.find("thread")->second + string(" -n ") + executionOpt.find("networkType")->second + string(" -o ") + executionOpt.find("outputPath")->second + string(" 1 > logs/worker_") + string(buffer) + string(" 2>&1 &");
            system((char*)executionCl.c_str());
            string workerPids = this->getPidList();
            
            if(workerPids != ""){
                workerPids = workerPids.substr(0, workerPids.length()-1);
                vector<string> workerPidList = this->tools.split_simple(workerPids, ' ');
                this->mu.lock();
                for(size_t i = 0; i < workerPidList.size(); i++){
                    if(this->jobPid.count(workerPidList[i]) == 0){
                        this->jobPid.insert({workerPidList[i], string(buffer)});
                        auto f = [this, workerPidList, i](){
                            this->monitoringExecution(workerPidList[i]);
                        };
                        this->threadPool->EnqueueJob(f);
                    }
                }
                this->mu.unlock();
            }
            cerr << "[INFO]Worker Excution(Job ID: " << buffer << ")" << endl;
            memset(buffer, 0, 512);
        }
        
        this_thread::sleep_for(chrono::milliseconds(50));
    }
}

string Execution::getPidList() {
    const char *proc_name = "worker";
    char command[256]{0, };
    char result[256]{0, };

    sprintf(command, "pidof %s", proc_name);
    // get "pidof" result as string
    FILE *fp = popen(command, "r");
    if (fp) {
        size_t ret = fread(result, 1, 256, fp);
        pclose(fp);
        if (ret <= 0) {
            printf("failed to get pidof command result\n");
            return "";
        }
        return string(result);
    }

    return "";
}

void Execution::monitoringExecution(string pid){
    char* buffer = new char[512];
    this->zktools.zkWget(this->zh,(char*)("/PiGraph/job/"+this->jobPid.find(pid)->second+"/State").c_str(), buffer);

    while (true){
        if(string(buffer) == "Kill"){
            this->mu.lock();
            string executionCl = "kill -9 " + pid;
            system((char*)executionCl.c_str());
            this->jobPid.erase(pid);
            this->mu.unlock();
            return;
        }

        string workerPids = this->getPidList();
        if(workerPids == ""){
            this->mu.lock();
            this->zktools.zkSet(this->zh, (char*)("/PiGraph/job/"+this->jobPid.find(pid)->second+"/State").c_str(), "Done");
            this->jobPid.erase(pid);
            this->mu.unlock();
            return;
        }
        else{
            bool workerState = false;
            workerPids = workerPids.substr(0, workerPids.length()-1);
            vector<string> workerPidList = this->tools.split_simple(workerPids, ' ');
            for (size_t i = 0; i < workerPidList.size(); i++){
                if(workerPidList[i] == pid){
                    workerState = true;
                    break;
                }
            }
            if(!workerState){
                this->mu.lock();
                this->zktools.zkSet(this->zh, (char*)("/PiGraph/job/"+this->jobPid.find(pid)->second+"/State").c_str(), "Done");
                this->jobPid.erase(pid);
                this->mu.unlock();
                return;
            }
        }
        this_thread::sleep_for(chrono::milliseconds(50));
    }
    return;
}

#endif