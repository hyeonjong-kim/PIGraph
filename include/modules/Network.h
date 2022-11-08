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
        int internalBucket;
        double* messageBuffer;
        IPoIB* ipoib;
        RDMA* rdma;
        Tools tools;
        ThreadPool::ThreadPool* connectionThreadPool;
        int externalNumVertex = 0;
        double initValue;
        bool recvCombiner = true;

    public:
        Network(){};
        ~Network();

        void setNetwork(string _networkType, int _numHost, vector<string> _hostInfo, int _port, map<int, int>* _recvPos, mutex* _mu, int _numMu, int thisHostNumber, double* msgBuffer, double initValue, bool recvCombiner);
        bool setIPoIB();
        bool setRDMA();
        void sendMsg_sum(int vertexID, double value);
        void sendMsg_min(int vertexID, double value);
        
        double* getMessageBuffer(){return this->messageBuffer;}
        int getExternalNumVertex(){return this->externalNumVertex;}
        double** getRecvMsg(){return this->recvMsg;}
        int getNumHost(){return this->numHost;}
        void sendAliveMsg(string msg);
        bool getAliveMsg();

        int externalHashFunction(int x){return (x % externalBucket);}
        int internalHashFunction(int x){return (x % internalBucket);}

        void closeNetwork();
};



void Network::setNetwork(string _networkType, int _numHost, vector<string> _hostInfo, int _port, map<int, int>* _recvPos, mutex* _mu, int _numMu, int thisHostNumber, double* msgBuffer, double initValue, bool recvCombiner){
    this->initValue = initValue;
    this->networkType =  _networkType;
    this->numHost = _numHost;
    this->externalBucket = _numHost;
    this->internalBucket = _numMu;
    this->hostInfo = _hostInfo;
    this->mu = _mu;
    this->numMu = _numMu; 
    this->recvPos = _recvPos;
    this->port = _port;
    this->thisHostNumber = thisHostNumber;
    this->ipoib = new IPoIB[numHost];
    this->rdma = new RDMA[numHost];
    
    this->recvMsg = new double*[this->numHost];
    for(size_t i = 0; i < this->numHost; i++){
        this->recvMsg[i] = new double[this->recvPos->size()];
        fill_n(this->recvMsg[i], this->recvPos->size(), this->initValue);
	}
    
    this->messageBuffer = msgBuffer;
    fill_n(this->messageBuffer, this->recvPos->size(), this->initValue);
    this->connectionThreadPool =  new ThreadPool::ThreadPool(this->numHost);
    this->recvCombiner = recvCombiner;

    if(this->networkType == "rdma"){
        this->setIPoIB();
        this->setRDMA();
    }
    else if(this->networkType == "ipoib"){
        this->setIPoIB();
    }
}

bool Network::setIPoIB(){
    std::vector<std::future<void>> futures;
    for(size_t i = 0; i < hostInfo.size(); i++){
        this->ipoib[i].setInfo(i, this->port, hostInfo[i]+".ib", this->numHost, this->port+this->thisHostNumber, this->recvPos->size(), this->recvPos, this->numMu);
        this->ipoib[i].setSocket();
        auto f = [this, i](){
            this->ipoib[i].connectSocket(this->initValue);
            return;
        };
        futures.emplace_back(this->connectionThreadPool->EnqueueJob(f));
    }
    
    for(auto& f_ : futures){
        f_.wait();
    }
    futures.clear();
    
    if(this->networkType == "ipoib"){
        for(size_t i = 0; i < hostInfo.size(); i++){
            if(i != this->thisHostNumber){
                auto f = [this, i](){
                    this->ipoib[i].getRemoteBufferSize();
                    this->ipoib[i].exchangeInfo();
                    return;
                };
                futures.emplace_back(this->connectionThreadPool->EnqueueJob(f));
            }
        }

        for(auto& f_ : futures){
    	    f_.wait();
  	    }
	    futures.clear();
        for(size_t i = 0; i < hostInfo.size(); i++){
            if(i != this->thisHostNumber)this->externalNumVertex += this->ipoib[i].getSendMsgBufSize();
        }
    }
   
    for (size_t i = 0; i < hostInfo.size(); i++){
        auto f = [this, i](){
            this->ipoib[i].sendCheckMsg();
            string s = "";
            while(s.compare("1\n")!= 0){
                s = this->ipoib[i].readCheckMsg();
            }
            cerr <<  "[INFO]" << this->ipoib[i].getServerAddr() << " - SUCCESS TO IPoIB CONNECTION" << endl;
            return;
        };
        futures.emplace_back(this->connectionThreadPool->EnqueueJob(f));
    }

    for(auto& f_ : futures){
    	f_.wait();
  	}
    
	futures.clear();
    return true;
}

bool Network::setRDMA(){
	int buffer_size = this->recvPos->size();
    std::vector<std::future<void>> futures;
	for(size_t i = 0; i < this->numHost; i++){
        if(i != this->thisHostNumber){
            this->rdma[i].setInfo(&(this->ipoib[i]), this->recvMsg[i], buffer_size, this->recvPos, this->numMu);
            auto f = [this, i](){
                this->rdma[i].connectRDMA(this->initValue);
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
            this->externalNumVertex += this->rdma[i].getNumVertex();
        }
    }
    
    for (size_t i = 0; i < this->numHost; i++){
        if(i != this->thisHostNumber){
            auto f = [this, i](){
                this->ipoib[i].sendCheckMsg();
                string s = "";
                while(s.compare("1\n")!= 0){
                    s = this->ipoib[i].readCheckMsg();
                }
                cerr <<  "[INFO]" << this->ipoib[i].getServerAddr() << " - SUCCESS TO RDMA CONNECTION" << endl;
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
            std::vector<std::future<void>> futures;
            for (size_t i = 0; i < this->numHost; i++){
                if(i != this->thisHostNumber){
                    auto f = [this, i](){
                        this->rdma[i].sendMsg();   
                    };
                    futures.emplace_back(this->connectionThreadPool->EnqueueJob(f));
                }
            }
            for(auto& f_ : futures){
                f_.wait();
            }
            futures.clear();

            for (size_t i = 0; i < this->numHost; i++){
                if(i != this->thisHostNumber){
                    auto f = [this, i](){
                        this->ipoib[i].sendCheckMsg();
                        string s = "";
                        while(s.compare("1\n")!= 0){
                            s = this->ipoib[i].readCheckMsg();
                        }
                        cerr <<  "[INFO]" << this->ipoib[i].getServerAddr() << " - SUCCESS RDMA Networking " << endl;
                        return;
                    };
                    futures.emplace_back(this->connectionThreadPool->EnqueueJob(f));
                }
            }
            
            for(auto& f_ : futures){
                f_.wait();
            }
            futures.clear();
            
            fill_n(this->messageBuffer, this->recvPos->size(), this->initValue);
            if(this->recvCombiner == true){
                map<int,int>::iterator iter;
                for(iter = this->recvPos->begin();iter != this->recvPos->end(); iter++){
                    for (size_t i = 0; i < this->numHost; i++){
                        this->messageBuffer[iter->second] += this->recvMsg[i][iter->second];
                    }
                }
                for (size_t i = 0; i < this->numHost; i++){
                    fill_n(this->recvMsg[i], this->recvPos->size(), this->initValue);
                }
            }
        }
        else{
            int dstNum = this->externalHashFunction(vertexID);
            if(dstNum == this->thisHostNumber){
                this->mu[this->internalHashFunction(vertexID)].lock();
                int recvIdx = this->recvPos->find(vertexID)->second;
                this->recvMsg[dstNum][recvIdx] += value;
                this->mu[this->internalHashFunction(vertexID)].unlock();
            }
            else{
                this->rdma[dstNum].combinerSum(vertexID, value);
            }
        }
    }

    else if(this->networkType == "ipoib"){
        if(vertexID == numeric_limits<int>::max()){
            std::vector<std::future<void>> futures;
            for (size_t i = 0; i < this->numHost; i++){
                if(i != this->thisHostNumber){
                    auto f = [this, i](){
                        string msg = "";
                        double* send_msg = this->ipoib[i].getSendMsgBuf();
                        for (size_t j = 0; j < this->ipoib[i].getSendMsgBufSize(); j++){
                            char tmp[128];
                            sprintf(tmp, "%0.16f", send_msg[j]);
                            msg += string(tmp) + "\n";
                        }
                        fill_n(send_msg, this->ipoib[i].getSendMsgBufSize(), this->initValue);
                        this->ipoib[i].sendMsg(msg); 
                        this->ipoib[i].sendMsg("Q");
                        msg = ipoib[i].readMsg();
                        
                        vector<string> splitMsg = this->tools.split_simple(msg, '\n');
                        for (size_t j = 0; j < splitMsg.size(); j++){
                            this->recvMsg[i][j] = stod(splitMsg[j]);
                        }
                    };
                    futures.emplace_back(this->connectionThreadPool->EnqueueJob(f));
                }
            }
            for(auto& f_ : futures){
                f_.wait();
            }
            futures.clear();

            for (size_t i = 0; i < this->numHost; i++){
                if(i != this->thisHostNumber){
                    auto f = [this, i](){
                        this->ipoib[i].sendCheckMsg();
                        string s = "";
                        while(s.compare("1\n")!= 0){
                            s = this->ipoib[i].readCheckMsg();
                        }
                        cerr <<  "[INFO]" << this->ipoib[i].getServerAddr() << " - SUCCESS IPoIB Networking " << endl;
                        return;
                    };
                    futures.emplace_back(this->connectionThreadPool->EnqueueJob(f));
                }
            }
            
            for(auto& f_ : futures){
                f_.wait();
            }
            futures.clear();

            fill_n(this->messageBuffer, this->recvPos->size(), this->initValue);
            if(this->recvCombiner == true){
                map<int,int>::iterator iter;
                for(iter = this->recvPos->begin();iter != this->recvPos->end(); iter++){
                    for (size_t i = 0; i < this->numHost; i++){
                        this->messageBuffer[iter->second] += this->recvMsg[i][iter->second];
                    }
                }
                for (size_t i = 0; i < this->numHost; i++){
                    fill_n(this->recvMsg[i], this->recvPos->size(), this->initValue);
                }
            }
        }
        else{
            int dstNum = this->externalHashFunction(vertexID);
            if(dstNum == this->thisHostNumber){
                this->mu[this->internalHashFunction(vertexID)].lock();
                int recvIdx = this->recvPos->find(vertexID)->second;
                this->recvMsg[dstNum][recvIdx] += value;
                this->mu[this->internalHashFunction(vertexID)].unlock();
            }
            else{
                this->ipoib[dstNum].combinerSum(vertexID, value);
            }
        }
    }
}

void Network::sendMsg_min(int vertexID, double value){
    if(this->networkType == "rdma"){
        if(vertexID == numeric_limits<int>::max()){
            std::vector<std::future<void>> futures;
            for (size_t i = 0; i < this->numHost; i++){
                if(i != this->thisHostNumber){
                    auto f = [this, i](){
                        this->rdma[i].sendMsg();   
                    };
                    futures.emplace_back(this->connectionThreadPool->EnqueueJob(f));
                }
            }
            for(auto& f_ : futures){
                f_.wait();
            }
            futures.clear();

            for (size_t i = 0; i < this->numHost; i++){
                if(i != this->thisHostNumber){
                    auto f = [this, i](){
                        this->ipoib[i].sendCheckMsg();
                        string s = "";
                        while(s.compare("1\n")!= 0){
                            s = this->ipoib[i].readCheckMsg();
                        }
                        cerr <<  "[INFO]" << this->ipoib[i].getServerAddr() << " - SUCCESS RDMA Networking " << endl;
                        return;
                    };
                    futures.emplace_back(this->connectionThreadPool->EnqueueJob(f));
                }
            }
            
            for(auto& f_ : futures){
                f_.wait();
            }
            futures.clear();
            
            fill_n(this->messageBuffer, this->recvPos->size(), this->initValue);
            if(this->recvCombiner == true){
                map<int,int>::iterator iter;
                for(iter = this->recvPos->begin();iter != this->recvPos->end(); iter++){
                    for (size_t i = 0; i < this->numHost; i++){
                        if(this->messageBuffer[iter->second] > this->recvMsg[i][iter->second])this->messageBuffer[iter->second] = this->recvMsg[i][iter->second];
                    }
                }
                for (size_t i = 0; i < this->numHost; i++){
                    fill_n(this->recvMsg[i], this->recvPos->size(), this->initValue);
                }
            }
           
        }
        else{
            int dstNum = this->externalHashFunction(vertexID);
            if(dstNum == this->thisHostNumber){
                this->mu[this->internalHashFunction(vertexID)].lock();
                int recvIdx = this->recvPos->find(vertexID)->second;
                if(this->recvMsg[dstNum][recvIdx] > value)this->recvMsg[dstNum][recvIdx] = value;
                this->mu[this->internalHashFunction(vertexID)].unlock();
            }
            else{
                this->rdma[dstNum].combinerMin(vertexID, value);
            }
        }
    }

    else if(this->networkType == "ipoib"){
        if(vertexID == numeric_limits<int>::max()){
            std::vector<std::future<void>> futures;
            for (size_t i = 0; i < this->numHost; i++){
                if(i != this->thisHostNumber){
                    auto f = [this, i](){
                        string msg = "";
                        double* send_msg = this->ipoib[i].getSendMsgBuf();
                        for (size_t j = 0; j < this->ipoib[i].getSendMsgBufSize(); j++){
                            char tmp[128];
                            if(send_msg[j] == numeric_limits<double>::max())send_msg[j] = 0.0;
                            sprintf(tmp, "%0.16f", send_msg[j]);
                            msg += string(tmp) + "\n";
                        }
                        fill_n(send_msg, this->ipoib[i].getSendMsgBufSize(), this->initValue);
                        this->ipoib[i].sendMsg(msg); 
                        this->ipoib[i].sendMsg("Q");
                        msg = ipoib[i].readMsg();
                        
                        vector<string> splitMsg = this->tools.split_simple(msg, '\n');
                        for (size_t j = 0; j < splitMsg.size(); j++){
                            if(stod(splitMsg[j]) == 0.0)this->recvMsg[i][j] = numeric_limits<double>::max();
                            else this->recvMsg[i][j] = stod(splitMsg[j]);
                        }
                    };
                    futures.emplace_back(this->connectionThreadPool->EnqueueJob(f));
                }
            }
            for(auto& f_ : futures){
                f_.wait();
            }
            futures.clear();

            for (size_t i = 0; i < this->numHost; i++){
                if(i != this->thisHostNumber){
                    auto f = [this, i](){
                        this->ipoib[i].sendCheckMsg();
                        string s = "";
                        while(s.compare("1\n")!= 0){
                            s = this->ipoib[i].readCheckMsg();
                        }
                        cerr <<  "[INFO]" << this->ipoib[i].getServerAddr() << " - SUCCESS IPoIB Networking " << endl;
                        return;
                    };
                    futures.emplace_back(this->connectionThreadPool->EnqueueJob(f));
                }
            }
            
            for(auto& f_ : futures){
                f_.wait();
            }
            futures.clear();

            fill_n(this->messageBuffer, this->recvPos->size(), this->initValue);
            if(this->recvCombiner == true){
                map<int,int>::iterator iter;
                for(iter = this->recvPos->begin();iter != this->recvPos->end(); iter++){
                    for (size_t i = 0; i < this->numHost; i++){
                        if(this->messageBuffer[iter->second] > this->recvMsg[i][iter->second])this->messageBuffer[iter->second] = this->recvMsg[i][iter->second];
                    }
                }
                for (size_t i = 0; i < this->numHost; i++){
                    fill_n(this->recvMsg[i], this->recvPos->size(), this->initValue);
                }
            }

            
        }
        else{
            int dstNum = this->externalHashFunction(vertexID);
            if(dstNum == this->thisHostNumber){
                this->mu[this->internalHashFunction(vertexID)].lock();
                int recvIdx = this->recvPos->find(vertexID)->second;
                if(this->recvMsg[dstNum][recvIdx] > value)this->recvMsg[dstNum][recvIdx] = value;
                this->mu[this->internalHashFunction(vertexID)].unlock();
            }
            else{
                this->ipoib[dstNum].combinerMin(vertexID, value);
            }
        }
    }
}

void Network::sendAliveMsg(string msg){
    for (size_t i = 0; i < this->numHost; i++){
        this->ipoib[i].sendAliveMsg(msg);
        this->ipoib[i].sendAliveMsg("Q");
    }  
}

bool Network::getAliveMsg(){
    bool state = false;
    for (size_t i = 0; i < this->numHost; i++){
        string msg = this->ipoib[i].readAliveMsg();
        if(msg.compare("alive") == 0)state = true;
    }
    return state;  
}

void Network::closeNetwork(){
    if(this->networkType == "rdma"){
        for (size_t i = 0; i < this->numHost; i++){
            if(i != this->thisHostNumber){
                this->ipoib[i].closeSocket();
                this->rdma[i].closeRDMA();
            }
        }
    }
    else if(this->networkType == "ipoib"){
        for (size_t i = 0; i < this->numHost; i++){
            if(i != this->thisHostNumber)this->ipoib[i].closeSocket();
        }
    }
}


#endif