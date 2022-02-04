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
        double NumVertices = 4031.0;
        map<int, queue<double>>* message_addr;
        double** msg_queue;
        mutex* socket_mu;
        int externalBucket;
        RDMA* msgThread;
        
    public:
        Vertex(Vertexidx vertex_id, Vertexidx out_edge, map<int, queue<double>>* _messsage_addr, RDMA* _rdma, int host_num, mutex* socketmu, double** _msg_queue);
        ~Vertex();
        
        virtual void Compute() = 0;

        const Vertexidx& GetVertexId() const {return this->vertex_id;}
        const int& GetSuperstep() const {return this->superstep;} 
        const VertexValue& GetValue() const {return this->vertex_value;}
        const vector<Vertexidx>& GetOutEdgeIterator()const {return this->out_edge;}
        const double& GetNumVertices() const {return this->NumVertices;}
        map<int, queue<double>>* GetMessageAddr() {return this->message_addr;}
        mutex* GetMutex(){return this->mu;}
        mutex* GetSocketMutex(){return this->socket_mu;}
        int GetState(){return this->state;}
        RDMA* GetMsgThread(){return this->msgThread;}
        int GetExternalBucket(){return this->externalBucket;}
        double** GetMsgQue(){return this->msg_queue;}

        void SetValue(VertexValue vertex_value){this->vertex_value = vertex_value;}
        void SetVertexid(Vertexidx vertex_id){this->vertex_id = vertex_id;}
        void SetNumVertices(double NumVertices){this->NumVertices = NumVertices;}
        void SetMessageAddr(map<int, queue<double>>* message_addr){this->message_addr = message_addr;}
        void SetMutex(mutex* mu){this->mu = mu;}
        void SetSocketMutex(mutex* socket_mu){this->socket_mu = socket_mu;}
        void SetExternalBucket(int host_num){this->externalBucket = host_num;}
        void SetMsgThread(RDMA* _rdma){this->msgThread = _rdma;}
        void SetMsgQue(double** _msg_queue){this->msg_queue=_msg_queue;}

        void AddOutEdge(Vertexidx vertexidx){this->out_edge.push_back(vertexidx);}
        void NextSuperstep(){this->superstep++;}
        void SendMessageTo(const Vertexidx& dest_vertex, const MessageValue& message, int socket_num);
        void VoteHalt(){this->state = 0;}
        int externalHashFunction(int x) {return (x % externalBucket);}
};

template<typename VertexValue, typename EdgeValue, typename MessageValue,typename Vertexidx>
Vertex<VertexValue, EdgeValue, MessageValue, Vertexidx>::Vertex(Vertexidx vertex_id, Vertexidx out_edge, map<int, queue<double>>* _messsage_addr, RDMA* _rdma, int host_num, mutex* socketmu, double** _msg_queue){
    SetVertexid(vertex_id);
    AddOutEdge(out_edge);
    SetMessageAddr(_messsage_addr);
    SetExternalBucket(host_num);
    SetSocketMutex(socketmu);
    SetMsgThread(_rdma);
    SetMsgQue(_msg_queue);
}

template<typename VertexValue, typename EdgeValue, typename MessageValue,typename Vertexidx>
Vertex<VertexValue, EdgeValue, MessageValue, Vertexidx>::~Vertex(){

}

template<typename VertexValue, typename EdgeValue, typename MessageValue,typename Vertexidx>
void Vertex<VertexValue, EdgeValue, MessageValue, Vertexidx>::SendMessageTo(const Vertexidx& dest_vertex, const MessageValue& message, int socket_num){
    this->socket_mu[socket_num].lock();
    this->msgThread[socket_num].SendMsg(dest_vertex, message);
    this->socket_mu[socket_num].unlock();
}

#endif