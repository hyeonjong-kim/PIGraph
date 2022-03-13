#ifndef PAGERANK_H
#define PAGERANK_H

#include <cmath>

#include "Vertex.hpp"

class PageRank : public Vertex<double, double, double, int> {
    public:
        PageRank(int vertex_id, int out_edge, int in_edge, RDMA* _rdma, mutex* _socket_mu,int _host_num);
        ~PageRank();
        
        void Compute();
        
};

#endif