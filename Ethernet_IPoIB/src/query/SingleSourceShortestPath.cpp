#include "SingleSourceShortestPath.hpp"

SingleSourceShortestPath::SingleSourceShortestPath(int vertex_id, int out_edge, int in_edge, map<int, queue<double>>* _messsage_addr, tcp* _t, int host_num, mutex* socketmu, int source_id_, double edgeValue):Vertex<double, double, double, int>(vertex_id, out_edge, in_edge, _messsage_addr, _t, host_num, socketmu){
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
    
    queue<double>& q = GetMessageAddr()->find(GetVertexId())->second;

    while(!q.empty()){
        if(q.front() < minDist)minDist = q.front();
        q.pop();
    }

    if(minDist < GetValue()){
        SetValue(minDist);
        const int n = GetOutEdgeIterator().size();
        for(vector<double>::size_type i = 0; i < n; i++){
            double distance = minDist + GetOutEdgeValueIterator().at(i);
            int tcp_num = externalHashFunction(GetOutEdgeIterator().at(i));
            SendMessageTo(GetOutEdgeIterator().at(i), distance, tcp_num);
        }
    }

    VoteHalt();
    
    NextSuperstep();
}

