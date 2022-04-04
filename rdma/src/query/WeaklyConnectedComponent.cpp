#include "WeaklyConnectedComponent.hpp"

WeaklyConnectedComponent::WeaklyConnectedComponent(int vertex_id, int out_edge, int in_edge, RDMA* _rdma, mutex* _socket_mu,int _host_num, double edgeValue):Vertex<double, double, double, int>(vertex_id, out_edge, in_edge, _rdma, _socket_mu, _host_num){

}

WeaklyConnectedComponent::~WeaklyConnectedComponent(){
    
}

void WeaklyConnectedComponent::Compute(){
    double currentComponent = GetValue();

    if(GetSuperstep() == 0){
        for(int i = 0; i < GetOutEdgeIterator().size(); i++){
            double neighbor = double(GetOutEdgeIterator().at(i));
            if(neighbor < currentComponent){
                currentComponent = neighbor;
            }
        }
    

        if(currentComponent != GetValue()){
            SetValue(currentComponent);
            for(int i = 0; i < GetOutEdgeIterator().size(); i++){
                int neighbor = GetOutEdgeIterator().at(i);
                if(neighbor > int(currentComponent)){
                    int rdma_num = externalHashFunction(GetOutEdgeIterator().at(i));
                    SendMessageTo(neighbor, GetValue(), rdma_num);
                }
            }
        }

        VoteHalt();
        NextSuperstep();
        return;
    }

    bool changed = false;

    for(int i = 0; i < GetExternalBucket(); i++){    
        for(int j = this->begin_pos; j < this->end_pos; j++){
            if(GetMsgQue()[i][j] == 0.0)break;
            double candidateComponent = GetMsgQue()[i][j];
            if(candidateComponent < currentComponent){
               currentComponent = candidateComponent;
               changed = true;
            }
        }
    }

    if(changed){
        SetValue(currentComponent);
        const int n = GetOutEdgeIterator().size();
        for(vector<double>::size_type i = 0; i < n; i++){
            int rdma_num = externalHashFunction(GetOutEdgeIterator().at(i));
            SendMessageTo(GetOutEdgeIterator().at(i), GetValue(), rdma_num);
        }
    }
    
    VoteHalt();
    NextSuperstep();
}