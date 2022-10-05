#ifndef PROCESSING_H
#define PROCESSING_H

#include <sys/time.h>
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
        int count = 0;
        

    public:
        Processing(){}
        ~Processing(){}
        void setInfo(Graph* graph, Network* network, int iteration, int numThread);
        void PageRank(int start, int end);
        void SingleShortestPath(int start, int end);
        string execute(string query);
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

string Processing::execute(string query){
    string result = "";
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
            if(this->superstep < this->iteration)this->network->sendMsg_sum(numeric_limits<int>::max(), 0.0);
            this->superstep++;
        }
        this->network->closeNetwork();

        map<int,double> finalResult;
        for (size_t i = 0; i < this->thisNumVertex; i++){
            //double tmp = floor(vertices[i].vertexValue*100000)/100000;
            finalResult.insert({vertices[i].vertexID, vertices[i].vertexValue});
        }
        vector<pp> vec(finalResult.begin(), finalResult.end());
        sort(vec.begin(), vec.end(), cmp);
        
        for(auto num:vec){ 
            char tmp[128];
            sprintf(tmp, "%0.16f", num.second);
            result += to_string(num.first) + "\t" + string(tmp) + "\n";
        }

        map<int,double> finalResult2;
        ifstream data_file("../am_result_4");
        vector<string> split_line;
        string read_str;
        while(getline(data_file, read_str)){
            split_line = this->tools.split_simple(read_str, '\t');
            finalResult2.insert({stoi(split_line[0]), stod(split_line[1])});
        }
        map<int,double>::iterator iter;
        
        double err = 0.0;
        for (iter = finalResult2.begin(); iter != finalResult2.end(); iter++){
            cerr.precision(10);
            double a = fabs(iter->second);
            double b = fabs(finalResult.find(iter->first)->second);
            double c;
            if(a > b)c = a;
            else c = b; 
            err += fabs(b-a)/fabs(c);
        }
        cerr << "Err: " << err / finalResult2.size() << endl;
    }

    return result;
}

#endif
