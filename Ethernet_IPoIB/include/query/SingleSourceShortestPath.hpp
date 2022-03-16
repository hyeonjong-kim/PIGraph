#ifndef SINGLESOURCESHORTESTPATH_H
#define SINGLESOURCESHORTESTPATH_H

#include <cmath>
#include <limits>

#include "Vertex.hpp"

class SingleSourceShortestPath : public Vertex<double, double, double, int> {
    public:
        SingleSourceShortestPath(int vertex_id, int out_edge, int in_edge, map<int, queue<double>>* _messsage_addr, tcp* _t, int host_num, mutex* socketmu, int source_id_, double edgeValue);
        ~SingleSourceShortestPath();
        
        void Compute();

        int source_id;
        
};

#endif