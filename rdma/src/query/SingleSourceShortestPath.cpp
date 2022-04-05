#include "SingleSourceShortestPath.hpp"

SingleSourceShortestPath::SingleSourceShortestPath(int vertex_id, int out_edge, int in_edge, RDMA* _rdma, mutex* _socket_mu,int _host_num, int source_id_, double edgeValue):Vertex<double, double, double, int>(vertex_id, out_edge, in_edge, _rdma, _socket_mu, _host_num){
    this->source_id = source_id_;
    if(out_edge!=NULL){
        AddOutEdge(out_edge);
        AddOutEdgeValue(edgeValue);
    }
    else{
        AddInEdge(in_edge);
        AddInEdgeValue(edgeValue);
    }
}

SingleSourceShortestPath::~SingleSourceShortestPath(){

}

void SingleSourceShortestPath::Compute(){
   
    if(GetSuperstep() == 0){
        SetValue(numeric_limits<double>::max());
    }
    
    double minDist = (this->source_id == GetVertexId()) ? 0.0 : numeric_limits<double>::max();

    for(int i = 0; i < GetExternalBucket(); i++){
        for(int j = this->begin_pos; j < this->end_pos; j++){
            if(GetMsgQue()[i][j] == numeric_limits<double>::max())break;
            if(GetMsgQue()[i][j] < minDist){
                minDist = GetMsgQue()[i][j];
            }
        }
    }

    if(minDist < GetValue()){
        SetValue(minDist);
        const int n = GetOutEdgeIterator().size();
        for(vector<double>::size_type i = 0; i < n; i++){
            double distance = minDist + GetOutEdgeValueIterator().at(i);
            int rdma_num = externalHashFunction(GetOutEdgeIterator().at(i));
            SendMessageTo(GetOutEdgeIterator().at(i), distance, rdma_num);
        }
    }

    VoteHalt();
    
    NextSuperstep();
}

