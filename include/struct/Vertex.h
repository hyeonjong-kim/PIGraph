#ifndef VERTEX_H
#define VERTEX_H

#include <limits>
using namespace std;

class Vertex{
    public:
        int vertexID;
        int pos;
        bool status = true;
        double vertexValue = numeric_limits<double>::max();
        double edgeValue;
        bool state = true;
        Vertex(){}
        Vertex(int vertexID){this->vertexID = vertexID;}
};

#endif
