#ifndef WEAKLYCONNECTEDCOMPONENT_H
#define WEAKLYCONNECTEDCOMPONENT_H

#include <cmath>

#include "Vertex.hpp"

class WeaklyConnectedComponent : public Vertex<double, double, double, int> {
    public:
        WeaklyConnectedComponent(int vertex_id, int out_edge, int in_edge, map<int, queue<double>>* _messsage_addr, tcp* _t, int host_num, mutex* socketmu, double edgeValue);
        ~WeaklyConnectedComponent();
        
        void Compute();
        
};

#endif