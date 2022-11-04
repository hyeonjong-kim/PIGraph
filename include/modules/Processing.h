#ifndef PROCESSING_H
#define PROCESSING_H

#include <sys/time.h>
#include <algorithm>
#include <cmath>

#include "../modules/Graph.h"
#include "../modules/Network.h"
#include "../utils/ThreadPool.h"
#include "../utils/Tools.h"
#define pp pair<int, double>

bool cmp(const pp& a, const pp& b) {
	if (a.second == b.second) return a.first < b.first;
	return a.second > b.second;
}

class Processing{
    private:
        ThreadPool::ThreadPool* threadPool;
        Network* network;
        Tools tools;
        Vertex* vertices;
        map<int, vector<int>>* edges;
        int totalNumVertex;
        int thisNumVertex;
        int thisNumEdge;
        int superstep = 0;
        double* msgBuffer;
        int iteration;
        int numThread;
        int sourceVertex;
        bool checkAliveThisWorker = false;
        bool checkAlive = true;
        bool blockProcessing = true;
        

    public:
        Processing(){}
        ~Processing(){}
        void setInfo(Graph* graph, Network* network, int iteration, int numThread, int sourceVertex, bool blockProcessing);
        void PageRank(int start, int end);
        void SingleSourceShortestPath(int start, int end);
        void WeaklyConnectedComponent(int start, int end);
        string execute(string query);
};

void Processing::setInfo(Graph* graph, Network* network, int iteration, int numThread, int sourceVertex, bool blockProcessing){
    this->vertices = graph->getVertices();
    this->edges = graph->getEdges();
    this->network = network;
    this->msgBuffer = graph->getMsgBuffer();
    this->iteration = iteration;
    this->totalNumVertex = graph->getNumVertex() + network->getExternalNumVertex();
    this->thisNumVertex = graph->getNumVertex();
    this->thisNumEdge = graph->getNumEdge();
    this->numThread = numThread;
    this->threadPool = new ThreadPool::ThreadPool(numThread);
    this->sourceVertex = sourceVertex;
    this->blockProcessing = blockProcessing;

    cerr << this->blockProcessing << endl;
}

void Processing::PageRank(int start, int end){
    for (size_t i = start; i < end; i++){
        if(this->superstep == 0){
            this->vertices[i].vertexValue = 1/this->totalNumVertex;
        }
        else{
            this->vertices[i].vertexValue = (0.15f/this->totalNumVertex) + 0.85f * this->msgBuffer[this->vertices[i].pos];
        }
        if(this->superstep < this->iteration){
            vector<int> outgoingEdges = this->edges->find(this->vertices[i].vertexID)->second;
            long n = outgoingEdges.size();
            for(size_t j = 0; j < n; j++){
                this->network->sendMsg_sum(outgoingEdges[j], this->vertices[i].vertexValue/n);  
            }
        }
    }
}

void Processing::SingleSourceShortestPath(int start, int end){
    for (size_t i = start; i < end; i++){
        if(this->vertices[i].state == true){
            if(this->superstep == 0){
                this->vertices[i].vertexValue = numeric_limits<double>::max();
            }
            
            double minDist = (this->vertices[i].vertexID == this->sourceVertex) ? 0.0 : this->msgBuffer[this->vertices[i].pos];
            if(minDist < this->vertices[i].vertexValue){
                this->vertices[i].vertexValue = minDist;
                vector<int> outgoingEdges = this->edges->find(this->vertices[i].vertexID)->second;
                long n = outgoingEdges.size();
                for(size_t j = 0; j < n; j++){
                    this->network->sendMsg_min(outgoingEdges[j], this->vertices[i].vertexValue + 1.0); 
                }
            }
        }
        vertices[i].state = false;
    }
}

void Processing::WeaklyConnectedComponent(int start, int end){
    for (size_t i = start; i < end; i++){
        if(this->vertices[i].state == true){
            int currentComponent = this->vertices[i].vertexValue;
            vector<int> outgoingEdges = this->edges->find(this->vertices[i].vertexID)->second;
            if(this->superstep == 0){
                for (size_t j = 0; j < outgoingEdges.size(); j++){
                    if(outgoingEdges[j] < currentComponent)currentComponent = outgoingEdges[j];
                }
                
                if(currentComponent != this->vertices[i].vertexValue){
                    this->vertices[i].vertexValue = currentComponent;
                    for (size_t j = 0; j < outgoingEdges.size(); j++){
                        if(outgoingEdges[j] > currentComponent)this->network->sendMsg_min(outgoingEdges[j], this->vertices[i].vertexValue);
                    }
                }
                vertices[i].state = false;
                return;
            }
            
            if(this->msgBuffer[this->vertices[i].pos] < currentComponent){
                currentComponent = this->msgBuffer[this->vertices[i].pos];
                this->vertices[i].vertexValue = currentComponent;
                for (size_t j = 0; j < outgoingEdges.size(); j++){
                    this->network->sendMsg_min(outgoingEdges[j], this->vertices[i].vertexValue);
                }
            }
            vertices[i].state = false;
        }
    }
}

string Processing::execute(string query){
    string result = "";
    if(this->blockProcessing == true){
        if(query == "pagerank"){
            int slice = this->thisNumEdge/this->numThread;
            int start;
            int end;
            cerr << this->totalNumVertex << endl;
            cerr << this->thisNumEdge << endl;
            std::vector<std::future<void>> futures;

            for (size_t i = 0; i <= this->iteration; i++){
                cerr << "[INFO]SUPERSTEP " << this->superstep << endl;
                start = 0;
                end = 0;
                for (size_t j = 1; j <= this->numThread; j++){
                    int sliceEdge = 0;
                    for (size_t z = start; z < this->thisNumVertex; z++){
                        sliceEdge += this->edges->find(this->vertices[z].vertexID)->second.size();
                        if(sliceEdge < slice)end++;
                        else break;
                    }
                    if(j != this->numThread){
                        auto f = [this, start, end](){
                            this->PageRank(start, end);
                        };
                        futures.emplace_back(this->threadPool->EnqueueJob(f));
                    }
                    else{
                        auto f = [this, start](){
                            this->PageRank(start, this->thisNumVertex);
                        };
                        futures.emplace_back(this->threadPool->EnqueueJob(f));
                    }
                    start = end;
                }
                for(auto& f_ : futures){
                    f_.wait();
                }
                futures.clear();
                cerr <<  "[INFO]SUCCESS PROCESSING GRAPH" << endl;
                if(this->superstep < this->iteration)this->network->sendMsg_sum(numeric_limits<int>::max(), 0.0);
                this->superstep++;
            }
            this->network->closeNetwork();

            map<int,double> finalResult;
            for (size_t i = 0; i < this->thisNumVertex; i++){
                finalResult.insert({vertices[i].vertexID, vertices[i].vertexValue});
            }
            vector<pp> vec(finalResult.begin(), finalResult.end());
            sort(vec.begin(), vec.end(), cmp);
            
            for(auto num:vec){ 
                char tmp[128];
                sprintf(tmp, "%0.16f", num.second);
                result += to_string(num.first) + "\t" + string(tmp) + "\n";
            }
        }

        else if(query == "sssp"){
            int slice = this->thisNumEdge/this->numThread;
            int start;
            int end;
            cerr << "[INFO]Total vertices: " << this->totalNumVertex << endl;
            cerr << "[INFO]This worker's edges: " << this->thisNumEdge << endl;
            std::vector<std::future<void>> futures;
            while(this->checkAlive){
                cerr << "[INFO]SUPERSTEP " << this->superstep << endl;
                start = 0;
                end = 0;
                for (size_t i = 1; i <= this->numThread; i++){
                    int sliceEdge = 0;
                    for (size_t j = start; j < this->thisNumVertex; j++){
                        sliceEdge += this->edges->find(this->vertices[j].vertexID)->second.size();
                        if(sliceEdge < slice)end++;
                        else break;
                    }
                    if(i != this->numThread){
                        auto f = [this, start, end](){
                            this->SingleSourceShortestPath(start, end);
                        };
                        futures.emplace_back(this->threadPool->EnqueueJob(f));
                    }
                    else{
                        auto f = [this, start](){
                            this->SingleSourceShortestPath(start, this->thisNumVertex);
                        };
                        futures.emplace_back(this->threadPool->EnqueueJob(f));
                    }
                    start = end;
                }
                for(auto& f_ : futures){
                    f_.wait();
                }
                futures.clear();
                cerr <<  "[INFO]SUCCESS PROCESSING GRAPH" << endl;
                
                this->network->sendMsg_min(numeric_limits<int>::max(), 0.0);
                
                for (size_t i = 0; i < this->thisNumVertex; i++){
                    if(this->msgBuffer[this->vertices[i].pos]!= numeric_limits<double>::max()){
                        this->vertices[i].state = true;
                        this->checkAliveThisWorker = true;
                    }
                }

                if(this->checkAliveThisWorker == true){
                    this->network->sendAliveMsg("alive");
                }
                else{
                    this->network->sendAliveMsg("dead");
                }
                this->checkAliveThisWorker = false;
                this->checkAlive = this->network->getAliveMsg();
                this->superstep++;
            }
            
            this->network->closeNetwork();

            for (size_t i = 0; i < this->thisNumVertex; i++){
                if(this->vertices[i].vertexValue == numeric_limits<double>::max())this->vertices[i].vertexValue = 0.0;
                result += to_string(this->vertices[i].vertexID) + "\t" + to_string(this->vertices[i].vertexValue) + "\n";
            }
        }

        else if(query == "wcc"){
            int slice = this->thisNumEdge/this->numThread;
            int start;
            int end;
            cerr << "[INFO]Total vertices: " << this->totalNumVertex << endl;
            cerr << "[INFO]This worker's edges: " << this->thisNumEdge << endl;
            std::vector<std::future<void>> futures;
            while(this->checkAlive){
                cerr << "[INFO]SUPERSTEP " << this->superstep << endl;
                start = 0;
                end = 0;
                for (size_t i = 1; i <= this->numThread; i++){
                    int sliceEdge = 0;
                    for (size_t j = start; j < this->thisNumVertex; j++){
                        sliceEdge += this->edges->find(this->vertices[j].vertexID)->second.size();
                        if(sliceEdge < slice)end++;
                        else break;
                    }
                    if(i != this->numThread){
                        auto f = [this, start, end](){
                            this->WeaklyConnectedComponent(start, end);
                        };
                        futures.emplace_back(this->threadPool->EnqueueJob(f));
                    }
                    else{
                        auto f = [this, start](){
                            this->WeaklyConnectedComponent(start, this->thisNumVertex);
                        };
                        futures.emplace_back(this->threadPool->EnqueueJob(f));
                    }
                    start = end;
                }
                for(auto& f_ : futures){
                    f_.wait();
                }
                futures.clear();
                cerr <<  "[INFO]SUCCESS PROCESSING GRAPH" << endl;
                
                this->network->sendMsg_min(numeric_limits<int>::max(), 0.0);
                
                for (size_t i = 0; i < this->thisNumVertex; i++){
                    if(this->msgBuffer[this->vertices[i].pos]!= numeric_limits<double>::max()){
                        this->vertices[i].state = true;
                        this->checkAliveThisWorker = true;
                    }
                }

                if(this->checkAliveThisWorker == true){
                    this->network->sendAliveMsg("alive");
                }
                else{
                    this->network->sendAliveMsg("dead");
                }
                this->checkAliveThisWorker = false;
                this->checkAlive = this->network->getAliveMsg();
                this->superstep++;
            }
            
            this->network->closeNetwork();

            for (size_t i = 0; i < this->thisNumVertex; i++){
                if(this->vertices[i].vertexValue == numeric_limits<double>::max())this->vertices[i].vertexValue = 0.0;
                result += to_string(this->vertices[i].vertexID) + "\t" + to_string(this->vertices[i].vertexValue) + "\n";
            }
        }
    }

    else if(this->blockProcessing == false){
        if(query == "pagerank"){
            cerr << this->totalNumVertex << endl;
            cerr << this->thisNumEdge << endl;
            std::vector<std::future<void>> futures;
            for (size_t i = 0; i <= this->iteration; i++){
                cerr << "[INFO]SUPERSTEP " << this->superstep << endl;
                for (size_t j = 0; j < this->thisNumVertex; j++){
                    auto f = [this, j](){
                        this->PageRank(j, j);
                    };
                    futures.emplace_back(this->threadPool->EnqueueJob(f));
                }
                for(auto& f_ : futures){
                    f_.wait();
                }
                futures.clear();
                cerr <<  "[INFO]SUCCESS PROCESSING GRAPH" << endl;
                if(this->superstep < this->iteration)this->network->sendMsg_sum(numeric_limits<int>::max(), 0.0);
                this->superstep++;
            }

            this->network->closeNetwork();
            map<int,double> finalResult;
            for (size_t i = 0; i < this->thisNumVertex; i++){
                finalResult.insert({vertices[i].vertexID, vertices[i].vertexValue});
            }
            vector<pp> vec(finalResult.begin(), finalResult.end());
            sort(vec.begin(), vec.end(), cmp);
            
            for(auto num:vec){ 
                char tmp[128];
                sprintf(tmp, "%0.16f", num.second);
                result += to_string(num.first) + "\t" + string(tmp) + "\n";
            }
        }

        else if(query == "sssp"){
            int slice = this->thisNumEdge/this->numThread;
            int start;
            int end;
            cerr << "[INFO]Total vertices: " << this->totalNumVertex << endl;
            cerr << "[INFO]This worker's edges: " << this->thisNumEdge << endl;
            std::vector<std::future<void>> futures;
            while(this->checkAlive){
                cerr << "[INFO]SUPERSTEP " << this->superstep << endl;
                start = 0;
                end = 0;
                for (size_t i = 1; i <= this->numThread; i++){
                    int sliceEdge = 0;
                    for (size_t j = start; j < this->thisNumVertex; j++){
                        sliceEdge += this->edges->find(this->vertices[j].vertexID)->second.size();
                        if(sliceEdge < slice)end++;
                        else break;
                    }
                    if(i != this->numThread){
                        auto f = [this, start, end](){
                            this->SingleSourceShortestPath(start, end);
                        };
                        futures.emplace_back(this->threadPool->EnqueueJob(f));
                    }
                    else{
                        auto f = [this, start](){
                            this->SingleSourceShortestPath(start, this->thisNumVertex);
                        };
                        futures.emplace_back(this->threadPool->EnqueueJob(f));
                    }
                    start = end;
                }
                for(auto& f_ : futures){
                    f_.wait();
                }
                futures.clear();
                cerr <<  "[INFO]SUCCESS PROCESSING GRAPH" << endl;
                
                this->network->sendMsg_min(numeric_limits<int>::max(), 0.0);
                
                for (size_t i = 0; i < this->thisNumVertex; i++){
                    if(this->msgBuffer[this->vertices[i].pos]!= numeric_limits<double>::max()){
                        this->vertices[i].state = true;
                        this->checkAliveThisWorker = true;
                    }
                }

                if(this->checkAliveThisWorker == true){
                    this->network->sendAliveMsg("alive");
                }
                else{
                    this->network->sendAliveMsg("dead");
                }
                this->checkAliveThisWorker = false;
                this->checkAlive = this->network->getAliveMsg();
                this->superstep++;
            }
            
            this->network->closeNetwork();

            for (size_t i = 0; i < this->thisNumVertex; i++){
                if(this->vertices[i].vertexValue == numeric_limits<double>::max())this->vertices[i].vertexValue = 0.0;
                result += to_string(this->vertices[i].vertexID) + "\t" + to_string(this->vertices[i].vertexValue) + "\n";
            }
        }

        else if(query == "wcc"){
            int slice = this->thisNumEdge/this->numThread;
            int start;
            int end;
            cerr << "[INFO]Total vertices: " << this->totalNumVertex << endl;
            cerr << "[INFO]This worker's edges: " << this->thisNumEdge << endl;
            std::vector<std::future<void>> futures;
            while(this->checkAlive){
                cerr << "[INFO]SUPERSTEP " << this->superstep << endl;
                start = 0;
                end = 0;
                for (size_t i = 1; i <= this->numThread; i++){
                    int sliceEdge = 0;
                    for (size_t j = start; j < this->thisNumVertex; j++){
                        sliceEdge += this->edges->find(this->vertices[j].vertexID)->second.size();
                        if(sliceEdge < slice)end++;
                        else break;
                    }
                    if(i != this->numThread){
                        auto f = [this, start, end](){
                            this->WeaklyConnectedComponent(start, end);
                        };
                        futures.emplace_back(this->threadPool->EnqueueJob(f));
                    }
                    else{
                        auto f = [this, start](){
                            this->WeaklyConnectedComponent(start, this->thisNumVertex);
                        };
                        futures.emplace_back(this->threadPool->EnqueueJob(f));
                    }
                    start = end;
                }
                for(auto& f_ : futures){
                    f_.wait();
                }
                futures.clear();
                cerr <<  "[INFO]SUCCESS PROCESSING GRAPH" << endl;
                
                this->network->sendMsg_min(numeric_limits<int>::max(), 0.0);
                
                for (size_t i = 0; i < this->thisNumVertex; i++){
                    if(this->msgBuffer[this->vertices[i].pos]!= numeric_limits<double>::max()){
                        this->vertices[i].state = true;
                        this->checkAliveThisWorker = true;
                    }
                }

                if(this->checkAliveThisWorker == true){
                    this->network->sendAliveMsg("alive");
                }
                else{
                    this->network->sendAliveMsg("dead");
                }
                this->checkAliveThisWorker = false;
                this->checkAlive = this->network->getAliveMsg();
                this->superstep++;
            }
            
            this->network->closeNetwork();

            for (size_t i = 0; i < this->thisNumVertex; i++){
                if(this->vertices[i].vertexValue == numeric_limits<double>::max())this->vertices[i].vertexValue = 0.0;
                result += to_string(this->vertices[i].vertexID) + "\t" + to_string(this->vertices[i].vertexValue) + "\n";
            }
        }
    }

    return result;
}

#endif
