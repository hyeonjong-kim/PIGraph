#ifndef PROCESSING_H
#define PROCESSING_H

#include <sys/time.h>

#include "../modules/Graph.h"
#include "../modules/Network.h"
#include "../utils/ThreadPool.h"

class Processing{
    private:
        ThreadPool::ThreadPool* threadPool;
        Network* network;

        Vertex* vertices;
        map<int, vector<int>>* edges;
        int totalNumVertex;
        int thisNumVertex;
        int thisNumEdge;
        int superstep = 0;
        double* msgBuffer;
        int iteration;
        int numThread;
        struct timeval start_query = {};
        struct timeval end_query = {};

    public:
        Processing(){}
        ~Processing(){}
        void setInfo(Graph* graph, Network* network, int iteration, int numThread);
        void PageRank(int start, int end);
        void SingleShortestPath(int start, int end);
        void execute();
};

void Processing::setInfo(Graph* graph, Network* network, int iteration, int numThread){
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
}

void Processing::PageRank(int start, int end){
    for (size_t i = start; i < end; i++){
        if(this->superstep == 0){
            this->vertices[i].vertexValue = 1.0/(double)this->totalNumVertex;
        }
        else{
            this->vertices[i].vertexValue = (0.15/this->totalNumVertex) + 0.85 * this->msgBuffer[this->vertices[i].pos];
        }
        vector<int> outgoingEdges = this->edges->find(this->vertices[i].vertexID)->second;
        int n = outgoingEdges.size();
        for(size_t j = 0; j < n; j++){
            this->network->sendMsg_sum(outgoingEdges[j], this->vertices[i].vertexValue/(double)n);
        }
    }
}

void Processing::execute(){
    int slice = this->thisNumEdge/this->numThread;
    int start;
    int end;
    
    std::vector<std::future<void>> futures;
    gettimeofday(&this->start_query, NULL);
    for (size_t i = 0; i < this->iteration; i++){
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
        this->network->sendMsg_sum(numeric_limits<int>::max(), 0.0);
        this->superstep++;
    }
    gettimeofday(&this->end_query, NULL);
    double time_query = end_query.tv_sec + end_query.tv_usec / 1000000.0 - start_query.tv_sec - start_query.tv_usec / 1000000.0;
    cerr << "Toal graph processing time: " << time_query << endl;
}

#endif