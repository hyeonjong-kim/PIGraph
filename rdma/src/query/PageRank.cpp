#include "PageRank.hpp"


PageRank::PageRank(int vertex_id, int out_edge, int in_edge, RDMA* _rdma, mutex* _socket_mu,int _host_num):Vertex<double, double, double, int>(vertex_id, out_edge, in_edge, _rdma, _socket_mu, _host_num){

}

PageRank::~PageRank(){

}

void PageRank::Compute(){

    int Vertexidx = GetVertexId();
    
    if(GetSuperstep() >=1){
        double sum = 0;
        for(int i = 0; i < GetExternalBucket(); i++){
            for(int j = this->begin_pos; j < this->end_pos; j++){
                if(GetMsgQue()[i][j] == 0.0)break;
                sum += GetMsgQue()[i][j];
            }
        }
        
        double value = (0.15/GetNumVertices()) + 0.85 * sum;
        SetValue(value);
    }
    
    if(GetSuperstep() < 30){
        const int n = GetOutEdgeIterator().size();
        for(vector<double>::size_type i = 0; i < n; i++){
            int rdma_num = externalHashFunction(GetOutEdgeIterator().at(i));
            SendMessageTo(GetOutEdgeIterator().at(i), GetValue()/double(n), rdma_num);
        }
    }
    else{
        VoteHalt();
    }

    NextSuperstep();
}