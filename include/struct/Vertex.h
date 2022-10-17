#ifndef VERTEX_H
#define VERTEX_H

class Vertex{
    public:
        int vertexID;
        int pos;
        bool status = true;
        double vertexValue = 3141592.0;
        double edgeValue;
        bool state = true;
        Vertex(){}
        Vertex(int vertexID){this->vertexID = vertexID;}
};

#endif
