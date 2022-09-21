#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <map>
#include <mutex>
#include <vector>

#include "../communication/IPoIB.h"
#include "../communication/RDMA.h"
#include "../utils/ThreadPool.h"
#include "../utils/Tools.h"


using namespace std;

class Network{
    private:
        string networkType;
        vector<string> hostInfo;
        int thisHostNumber;
        int numHost;
        int port;
        double** recvMsg;
        mutex* mu;
        int numMu;
        map<int, int>* recvPos;
        int externalBucket;
        double* messageBuffer;
        IPoIB** ipoib;
        RDMA** rdma;
        
        Tools tools;
        ThreadPool::ThreadPool* connectionThreadPool;

    public:
        Network(){};
        ~Network();

        void setNetwork(string _networkType, int _numHost, vector<string> _hostInfo, int _port, map<int, int>* _recvPos, mutex* _mu, int _numMu);
        bool setIPoIB();
        bool setRDMA();
        void sendMsg_sum(int vertexID, double value);
        
        double* getMessageBuffer(){return this->messageBuffer;}

        int externalHashFunction(int x){return (x % externalBucket);}
};

 char* HostToIp(string host) {
    hostent* hostname = gethostbyname(host.c_str());
    if(hostname)
        return inet_ntoa(**(in_addr**)hostname->h_addr_list);
    return {};
}

void Network::setNetwork(string _networkType, int _numHost, vector<string> _hostInfo, int _port, map<int, int>* _recvPos, mutex* _mu, int _numMu){
    this->networkType =  _networkType;
    this->numHost = _numHost;
    this->externalBucket = _numHost;
    this->hostInfo = _hostInfo;
    this->mu = _mu;
    this->numMu = _numMu; 
    this->recvPos = _recvPos;
    this->port = _port;
    
    this->ipoib = new IPoIB*[numHost];
    this->rdma = new RDMA*[numHost];
    this->recvMsg = new double*[this->numHost];
    for(size_t i = 0; i < this->numHost; i++){
        this->recvMsg[i] = new double[this->recvPos->size()]{0.0};
	}
    this->messageBuffer = new double[this->recvPos->size()]{0.0};

    this->connectionThreadPool =  new ThreadPool::ThreadPool(this->numHost);
    for (size_t i = 0; i < this->numHost; i++){
        if(i != this->thisHostNumber){
            this->ipoib[i] = new IPoIB();
            this->rdma[i] = new RDMA();
        }
    }

    if(this->networkType == "rdma"){
        this->setIPoIB();
        this->setRDMA();
    }
}

bool Network::setIPoIB(){
    
    char thisHostName[256];
    gethostname(thisHostName, 256);
    for (size_t i = 0; i < hostInfo.size(); i++){
        if(string(thisHostName).compare(hostInfo[i]) == 0){
            break;
        }
        this->thisHostNumber++;
    }
    vector<char*> serverAddr(this->numHost);
    std::vector<std::future<void>> futures;

    for(size_t i = 0; i < hostInfo.size(); i++){
        
        if(i != this->thisHostNumber){
            serverAddr[i] = HostToIp(hostInfo[i]+".ib");
            this->ipoib[i]->setInfo(i, this->port, serverAddr[i], this->numHost, this->port+this->thisHostNumber);
            this->ipoib[i]->setSocket();
            auto f = [this, i](){
                this->ipoib[i]->connectSocket();
                return;
            };
            futures.emplace_back(this->connectionThreadPool->EnqueueJob(f));
        }
    }
     
    for(auto& f_ : futures){
        f_.wait();
    }
    futures.clear();

    for (size_t i = 0; i < hostInfo.size(); i++){
        if(i != this->thisHostNumber){
            auto f = [this, i](){
                this->ipoib[i]->sendCheckMsg();
                string s = "";
                while(s.compare("1\n")!= 0){
                    s = this->ipoib[i]->readCheckMsg();
                }
                cerr <<  "[INFO]" << this->ipoib[i]->getServerAddr() << " - SUCCESS TO IPoIB CONNECTION" << endl;
                return;
		    };
		    futures.emplace_back(this->connectionThreadPool->EnqueueJob(f));
        }
    }

    for(auto& f_ : futures){
    	f_.wait();
  	}
    
	futures.clear();
    return true;
}

bool Network::setRDMA(){
	int buffer_size = recvPos->size();
    std::vector<std::future<void>> futures;
	for(size_t i = 0; i < this->numHost; i++){
        if(i != this->thisHostNumber){
            this->rdma[i]->setInfo(this->ipoib[i], this->recvMsg[i], buffer_size, recvPos, mu, numMu);
            auto f = [this, i](){
                this->rdma[i]->connectRDMA();
			    return;
		    };
		    futures.emplace_back(this->connectionThreadPool->EnqueueJob(f));
        }
	}
    
	for (auto& f_ : futures) {
    	f_.wait();
  	}

	futures.clear();
    
    for (size_t i = 0; i < this->numHost; i++){
        if(i != this->thisHostNumber){
            auto f = [this, i](){
                this->ipoib[i]->sendCheckMsg();
                string s = "";
                while(s.compare("1\n")!= 0){
                    s = this->ipoib[i]->readCheckMsg();
                }
                cerr <<  "[INFO]" << this->ipoib[i]->getServerAddr() << " - SUCCESS TO RDMA CONNECTION" << endl;
                return;
            };
            futures.emplace_back(this->connectionThreadPool->EnqueueJob(f));
        }
    }

    for(auto& f_ : futures){
    	f_.wait();
  	}
	futures.clear();

    return true;
}

void Network::sendMsg_sum(int vertexID, double value){
    if(this->networkType == "rdma"){
        if(vertexID == numeric_limits<int>::max()){
            for (size_t i = 0; i < this->numHost; i++){
                if(i != this->thisHostNumber){
                    this->rdma[i]->sendMsg();   
                }
            }
            fill_n(this->messageBuffer, this->recvPos->size(), 0.0);
            std::vector<std::future<void>> futures;
            for (size_t i = 0; i < this->numHost; i++){
                if(i != this->thisHostNumber){
                    auto f = [this, i](){
                        this->ipoib[i]->sendCheckMsg();
                        string s = "";
                        while(s.compare("1\n")!= 0){
                            s = this->ipoib[i]->readCheckMsg();
                        }
                        cerr <<  "[INFO]" << this->ipoib[i]->getServerAddr() << " - SUCCESS TO RDMA Networking " << endl;
                        return;
                    };
                    futures.emplace_back(this->connectionThreadPool->EnqueueJob(f));
                }
            }
            for(auto& f_ : futures){
                f_.wait();
            }
            futures.clear();
            
            map<int,int>::iterator iter;
            for(iter = this->recvPos->begin();iter != this->recvPos->end(); iter++){
                for (size_t i = 0; i < this->numHost; i++){
                    this->messageBuffer[iter->second] += this->recvMsg[i][iter->second];
                }
            }

            for (size_t i = 0; i < this->numHost; i++){
                fill_n(this->recvMsg[i], this->recvPos->size(), 0.0);
            }
        }
        else{
            int dstNum = this->externalHashFunction(vertexID);
            if(dstNum == this->thisHostNumber){
                int recvIdx = this->recvPos->find(vertexID)->second;
                this->recvMsg[dstNum][recvIdx] += value;
            }
            else{
                this->rdma[dstNum]->combinerSum(vertexID, value);
            }
        }
    }
    else if(this->networkType == "ipoib"){
        if(vertexID == numeric_limits<int>::max()){
        }
        int dstNum = this->externalHashFunction(vertexID);
        if(dstNum == this->thisHostNumber){
        }
        else{
        }
    }
}

#endif