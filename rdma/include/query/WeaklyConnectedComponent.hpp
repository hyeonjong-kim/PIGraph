#ifndef WEAKLYCONNECTEDCOMPONENT_H
#define WEAKLYCONNECTEDCOMPONENT_H

#include <cmath>

#include "Vertex.hpp"

class WeaklyConnectedComponent : public Vertex<double, double, double, int> {
    public:
        WeaklyConnectedComponent(int vertex_id, int out_edge, int in_edge, RDMA* _rdma, mutex* _socket_mu,int _host_num, double edgeValue);
        ~WeaklyConnectedComponent();
        
        void Compute();
        
};

#endif