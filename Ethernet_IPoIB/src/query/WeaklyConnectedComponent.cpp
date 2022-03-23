#include "WeaklyConnectedComponent.hpp"

WeaklyConnectedComponent::WeaklyConnectedComponent(int vertex_id, int out_edge, int in_edge, map<int, queue<double>>* _messsage_addr, tcp* _t, int host_num, mutex* socketmu, double edgeValue):Vertex<double, double, double, int>(vertex_id, out_edge, in_edge, _messsage_addr, _t, host_num, socketmu){

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
                    int tcp_num = externalHashFunction(GetOutEdgeIterator().at(i));
                    SendMessageTo(neighbor, GetValue(), tcp_num);
                }
            }
        }

        VoteHalt();
        NextSuperstep();
        return;
    }
    
    bool changed = false;

    queue<double>& q = GetMessageAddr()->find(GetVertexId())->second;
    
    while(!q.empty()){
        double candidateComponent = q.front();
        if(candidateComponent < currentComponent){
            currentComponent = candidateComponent;
            changed = true;
        }
        q.pop();
    }

    if(changed){
        SetValue(currentComponent);
        const int n = GetOutEdgeIterator().size();
        for(vector<double>::size_type i = 0; i < n; i++){
            int tcp_num = externalHashFunction(GetOutEdgeIterator().at(i));
            SendMessageTo(GetOutEdgeIterator().at(i), GetValue(), tcp_num);
        }
    }

    VoteHalt();
    NextSuperstep();
}