#ifndef PROCESSING_H
#define PROCESSING_H

#include "../modules/Graph.h"
#include "../modules/Network.h"
#include "../utils/ThreadPool.h"

class Processing{
    private:
        ThreadPool::ThreadPool* ThreadPool;
        Network* network;

        Vertex* vertices;
        map<int, vector<int>>* edges;
        int numVertex;
        int superstep = 0;
        double* msgBuffer;
        int iteration;

    public:
        Processing(){}
        ~Processing(){}
        void setInfo(Graph* graph, Network* network, int iteration);
        void PageRank(int start, int end);
        void SingleShortestPath(int start, int end);
        void execute();
};

void Processing::setInfo(Graph* graph, Network* network, int iteration){
    this->vertices = graph->getVertices();
    this->edges = graph->getEdges();
    this->network = network;
    this->msgBuffer = network->getMessageBuffer();
    this->iteration = iteration;
    this->numVertex = graph->getNumVertex();
}

void Processing::PageRank(int start, int end){
    for (size_t i = start; i < end; i++){
        if(this->superstep == 0){
            this->vertices[i].vertexValue = 1.0/(double)this->numVertex;
        }
        else{
            //cerr << this->msgBuffer[this->vertices[i].pos] << endl;
            double value = (0.15/this->numVertex) + (0.85 * (this->msgBuffer[this->vertices[i].pos]));
            this->vertices[i].vertexValue = value;
        }
        vector<int> outgoingEdges = this->edges->find(this->vertices[i].vertexID)->second;
        size_t n = outgoingEdges.size();
        for(size_t i = 0; i < n; i++){
            this->network->sendMsg_sum(outgoingEdges[i], this->vertices[i].vertexValue/(double)n);
        }
    }
    this->superstep++;
}

void Processing::execute(){
    for (size_t i = 0; i < this->iteration; i++){
        this->PageRank(0, this->numVertex);
        this->network->sendMsg_sum(numeric_limits<int>::max(), 0.0);
    }
}

#endif