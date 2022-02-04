#include "PageRank.hpp"


PageRank::PageRank(int vertex_id, int out_edge, map<int, queue<double>>* _messsage_addr, RDMA* _rdma, int host_num, mutex* socketmu, double** _msg_queue):Vertex<double, void, double, int>(vertex_id, out_edge, _messsage_addr, _rdma, host_num, socketmu, _msg_queue){
    SetValue(1.0/GetNumVertices());
}

PageRank::~PageRank(){

}

void PageRank::Compute(){
    
    int Vertexidx = GetVertexId();
    
    if(GetSuperstep() >=1){
        double sum = 0;
        for(int i = 0; i < GetExternalBucket(); i++){
            for(int j = 0; j < 256; j++){
                if(GetMsgQue()[i][j] == 0.0)break;
                //cout << GetMsgQue()[i][j] << endl;
                sum += GetMsgQue()[i][j];
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