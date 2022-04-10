#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include <queue>
#include <vector>
#include <map>
#include <mutex>

#include "../network/tcp.hpp"

using namespace std;

template<
    typename VertexValue,
    typename EdgeValue,
    typename MessageValue, 
    typename Vertexidx
>
class Vertex {

    private:
        int superstep=0;
        VertexValue vertex_value;
        Vertexidx vertex_id;
        bool state = true;
        double NumVertices = 4031.0;
        map<int, queue<double>>* message_addr;
        mutex* socket_mu;
        int externalBucket;
        tcp* msgThread;

        vector<Vertexidx> out_edge;
        vector<Vertexidx> in_edge;

        vector<EdgeValue> in_edge_value;
        vector<EdgeValue> out_edge_value;
        
    public:
        Vertex(Vertexidx vertex_id, Vertexidx out_edge, Vertexidx in_edge, map<int, queue<double>>* _messsage_addr, tcp* _t, int host_num, mutex* socketmu);
        ~Vertex();
        
        virtual void Compute() = 0;

        const Vertexidx& GetVertexId() const {return this->vertex_id;}
        const int& GetSuperstep() const {return this->superstep;} 
        const VertexValue& GetValue() const {return this->vertex_value;}
        const double& GetNumVertices() const {return this->NumVertices;}
        map<int, queue<double>>* GetMessageAddr() {return this->message_addr;}
        mutex* GetMutex(){return this->mu;}
        mutex* GetSocketMutex(){return this->socket_mu;}
        bool GetState(){return this->state;}
        tcp* GetMsgThread(){return this->msgThread;}
        int GetExternalBucket(){return this->externalBucket;}

        const vector<Vertexidx>& GetOutEdgeIterator()const {return this->out_edge;}
        const vector<Vertexidx>& GetInEdgeIterator()const {return this->in_edge;}

        const vector<EdgeValue>& GetOutEdgeValueIterator()const {return this->out_edge_value;}
        const vector<EdgeValue>& GetInEdgeValueIterator()const {return this->in_edge_value;}


        void SetValue(VertexValue vertex_value){this->vertex_value = vertex_value;}
        void SetVertexid(Vertexidx vertex_id){this->vertex_id = vertex_id;}
        void SetNumVertices(double NumVertices){this->NumVertices = NumVertices;}
        void SetMessageAddr(map<int, queue<double>>* message_addr){this->message_addr = message_addr;}
        void SetMutex(mutex* mu){this->mu = mu;}
        void SetSocketMutex(mutex* socket_mu){this->socket_mu = socket_mu;}
        void SetExternalBucket(int host_num){this->externalBucket = host_num;}
        void SetMsgThread(tcp* _t){this->msgThread = _t;}

        void AddOutEdge(Vertexidx vertexidx){this->out_edge.push_back(vertexidx);}
        void AddInEdge(Vertexidx vertexidx){this->in_edge.push_back(vertexidx);}
        void AddOutEdgeValue(EdgeValue edgeValue){this->out_edge_value.push_back(edgeValue);}
        void AddInEdgeValue(EdgeValue edgeValue){this->in_edge_value.push_back(edgeValue);}
        void NextSuperstep(){this->superstep++;}
        void SendMessageTo(const Vertexidx& dest_vertex, const MessageValue& message, int socket_num);
        int externalHashFunction(int x) {return (x % externalBucket);}
        
        void VoteHalt(){this->state = false;}
        void IsWake(){this->state = true;}
};

template<typename VertexValue, typename EdgeValue, typename MessageValue,typename Vertexidx>
Vertex<VertexValue, EdgeValue, MessageValue, Vertexidx>::Vertex(Vertexidx vertex_id, Vertexidx out_edge, Vertexidx in_edge, map<int, queue<double>>* _messsage_addr, tcp* _t, int host_num, mutex* socketmu){
    SetVertexid(vertex_id);
    SetMessageAddr(_messsage_addr);
    SetExternalBucket(host_num);
    SetSocketMutex(socketmu);
    SetMsgThread(_t);
    if(out_edge!=NULL){
        AddOutEdge(out_edge);
    }
    else{
        AddInEdge(in_edge);
    }
}

template<typename VertexValue, typename EdgeValue, typename MessageValue,typename Vertexidx>
Vertex<VertexValue, EdgeValue, MessageValue, Vertexidx>::~Vertex(){

}

template<typename VertexValue, typename EdgeValue, typename MessageValue,typename Vertexidx>
void Vertex<VertexValue, EdgeValue, MessageValue, Vertexidx>::SendMessageTo(const Vertexidx& dest_vertex, const MessageValue& message, int socket_num){
    
    //cerr << dest_vertex << endl;
    //this->socket_mu[socket_num].lock();
    this->msgThread[socket_num].Sendmsg(to_string(dest_vertex) + " " + to_string(message) + "\n", dest_vertex);
    //this->socket_mu[socket_num].unlock();
}

#endif