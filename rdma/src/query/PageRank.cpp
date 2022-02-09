#include "PageRank.hpp"


PageRank::PageRank(int vertex_id, int out_edge, int in_edge, RDMA* _rdma, int host_num, mutex* socketmu):Vertex<double, void, double, int>(vertex_id, out_edge, in_edge, _rdma, host_num, socketmu){
    SetValue(1.0/GetNumVertices());
}

PageRank::~PageRank(){

}

void PageRank::Compute(){

    int Vertexidx = GetVertexId();
    
    if(GetSuperstep() >=1){
        double sum = 0;
        for(int i = 0; i < GetExternalBucket(); i++){
            for(int j = this->begin_pos; j < this->end_pos; j++){
                sum += GetMsgQue()[i][j];
                //cout <<  GetMsgQue()[i][j] << endl;
            }
        }
        double value = 0.15/GetNumVertices() + 0.85 * sum;
        
        SetValue(value);
    }


    if(GetSuperstep() < 30){
        const int n = GetOutEdgeIterator().size();
        for(vector<double>::size_type i = 0; i < n; i++){
            int rdma_num = externalHashFunction(GetOutEdgeIterator().at(i));
            SendMessageTo(GetOutEdgeIterator().at(i), GetValue()/n, rdma_num);
        }
    }else{
        VoteHalt();
    }

    NextSuperstep();

}