#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include <queue>
#include <vector>
#include <map>
#include <mutex>

#include "../network/tcp.hpp"
#include "../network/RDMA.hpp"

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
        int state = 1;
        vector<Vertexidx> out_edge;
        double NumVertices;
        mutex* socket_mu;
        int internalBucket;
        int externalBucket;
        RDMA* msgThread;
        vector<Vertexidx> in_edge;

        mutex* vertex_mu;

        double** msg_queue;
        

    public:
        int begin_pos;
        int end_pos;

        Vertex(Vertexidx vertex_id, Vertexidx out_edge, Vertexidx in_edge, RDMA* _rdma, mutex* _socket_mu, int _host_num);
        ~Vertex();
        
        virtual void Compute() = 0;

        const Vertexidx& GetVertexId() const {return this->vertex_id;}
        const int& GetSuperstep() const {return this->superstep;} 
        const VertexValue& GetValue() const {return this->vertex_value;}
        const vector<Vertexidx>& GetOutEdgeIterator()const {return this->out_edge;}
        const vector<Vertexidx>& GetInEdgeIterator()const {return this->in_edge;}
        const double& GetNumVertices() const {return this->NumVertices;}
        mutex* GetSocketMutex(){return this->socket_mu;}
        mutex* GetVertexMutex(){return this->vertex_mu;}
        int GetState(){return this->state;}
        RDMA* GetMsgThread(){return this->msgThread;}
        int GetExternalBucket(){return this->externalBucket;}
        double** GetMsgQue(){return this->msg_queue;}
    
        void SetValue(VertexValue vertex_value){this->vertex_value = vertex_value;}
        void SetVertexid(Vertexidx vertex_id){this->vertex_id = vertex_id;}
        void SetNumVertices(double NumVertices){this->NumVertices = NumVertices;}
        void SetMutex(mutex* mu){this->mu = mu;}
        void SetSocketMutex(mutex* socket_mu){this->socket_mu = socket_mu;}
        void SetVertexMutex(mutex* _vertex_mu){this->vertex_mu = _vertex_mu;}
        void SetExternalBucket(int _host_num){this->externalBucket = _host_num;}
        void SetMsgThread(RDMA* _rdma){this->msgThread = _rdma;}
        void SetMsgQue(double** _msg_queue){this->msg_queue=_msg_queue;}
        void SetPos(int begin, int end){
            this->begin_pos = begin;
            this->end_pos = end;
        }

        void AddOutEdge(Vertexidx vertexidx){this->out_edge.push_back(vertexidx);}
        void AddInEdge(Vertexidx vertexidx){this->in_edge.push_back(vertexidx);}
        void NextSuperstep(){this->superstep++;}
        void SendMessageTo(const Vertexidx& dest_vertex, const MessageValue& message, int socket_num);
        void VoteHalt(){this->state = 0;}
        int externalHashFunction(int x) {return (x % this->externalBucket);}
};

template<typename VertexValue, typename EdgeValue, typename MessageValue,typename Vertexidx>
Vertex<VertexValue, EdgeValue, MessageValue, Vertexidx>::Vertex(Vertexidx vertex_id, Vertexidx out_edge, Vertexidx in_edge, RDMA* _rdma, mutex* _socket_mu, int _host_num){
    SetVertexid(vertex_id);
    SetExternalBucket(_host_num);
    SetSocketMutex(_socket_mu);
    SetMsgThread(_rdma);
    
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
    //this->socket_mu[socket_num].lock();
    this->msgThread[socket_num].SendMsg(dest_vertex, message);
    //this->socket_mu[socket_num].unlock();
}

#endif