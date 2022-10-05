#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <string.h>
#include <map>
#include <vector>

#include "../utils/Tools.h"
#include "../struct/Vertex.h"

using namespace std;

class Graph{
    private:
        Tools tools;
        
        Vertex* vertices;
        double* msgBuffer;
        map<int, vector<int>>* edges = new map<int, vector<int>>;
        map<int, int>* recvPos = new map<int, int>;
        
        int numVertex;
        int numEdge = 0;
        int hostID;
        int externalBucket;
        char delimiter;
        
    public:
        Graph(){};
        ~Graph();

        void setInfo(int partitioningOpt, int numHost, int hostID);
        void createGraph(string& edgeInfo);

        Vertex* getVertices(){return vertices;}
        void setVertices(Vertex* vertices){this->vertices = vertices;}
        map<int, vector<int>>* getEdges(){return this->edges;}
        void setEdges(map<int, vector<int>>* edges){this->edges = edges;}
        map<int, int>* getRecvPos(){return this->recvPos;}
        void setRecvPos(map<int, int>* recvPos){this->recvPos = recvPos;}
        double* getMsgBuffer(){return this->msgBuffer;}
        void setMsgBuffer(double* msgBuffer){this->msgBuffer = msgBuffer;}
        int getNumVertex(){return this->numVertex;}
        void setNumVertex(int numVertex){this->numVertex = numVertex;}
        int getNumEdge(){return this->numEdge;}
        void setNumEdge(int numEdge){this->numEdge = numEdge;}
        
        int externalHashFunction(int x){return (x % externalBucket);}
};

void Graph::setInfo(int partitioningOpt, int numHost, int hostID){
    if(partitioningOpt == 0){
        this->delimiter = ' ';
    }
    else if(partitioningOpt == 1){
        this->delimiter = '\t';
    }

    this->hostID = hostID;
    this->externalBucket = numHost;
}

void Graph::createGraph(string& edgeInfo){
    vector<string> splitEdgeInfo1 = this->tools.split_simple(edgeInfo, '\n');
    map<int, Vertex> tmp;
    for (size_t i = 0; i < splitEdgeInfo1.size(); i++){
        vector<string> splitEdgeInfo2 = this->tools.split_simple(splitEdgeInfo1[i], this->delimiter);
        if(this->externalHashFunction(stoi(splitEdgeInfo2[0])) == this->hostID){
			if(tmp.count(stoi(splitEdgeInfo2[0])) == 1){
                this->edges->find(stoi(splitEdgeInfo2[0]))->second.push_back(stoi(splitEdgeInfo2[1]));
                this->numEdge++;
            }
			else{
                Vertex v(stoi(splitEdgeInfo2[0]));
                tmp.insert({stoi(splitEdgeInfo2[0]),v});
                vector<int> e;
                e.push_back(stoi(splitEdgeInfo2[1]));
                this->edges->insert({stoi(splitEdgeInfo2[0]), e});
                this->numEdge++;
			}
		}
		if(this->externalHashFunction(stoi(splitEdgeInfo2[1])) == this->hostID){
			if(tmp.count(stoi(splitEdgeInfo2[1])) != 1){
                Vertex v(stoi(splitEdgeInfo2[1]));
                tmp.insert({stoi(splitEdgeInfo2[1]),v});
                vector<int> e;
                this->edges->insert({stoi(splitEdgeInfo2[1]), e});
			}
		}
    }
    this->numVertex = tmp.size();
    this->vertices = new Vertex[tmp.size()];
    this->msgBuffer = new double[tmp.size()];

    size_t idx = 0;
    map<int, Vertex>::iterator iter;
    for(iter = tmp.begin(); iter != tmp.end(); iter++){
        this->recvPos->insert({iter->first, idx});
        iter->second.pos=idx;
        this->vertices[idx] = iter->second;
        idx++;
    }
    return;
}

#endif