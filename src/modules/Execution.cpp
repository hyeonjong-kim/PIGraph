#include "../../include/modules/Execution.h"

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
    map<string, string> executionOpt;
    while(true){
        if(strcmp(buffer, previousBuffer) !=0){
            string jobPath = "/PiGraph/job/"+string(buffer);
            cerr << buffer << endl;
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
            cerr << "[INFO]Worker Excution(Job ID: " << buffer << ")" << endl;
            cerr << executionCl << endl;
            memset(buffer, 0, 512);
        }
        this_thread::sleep_for(chrono::milliseconds(50));
    }
}