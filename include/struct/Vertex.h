#ifndef VERTEX_H
#define VERTEX_H

class Vertex{
    public:
        int vertexID;
        int pos;
        bool status = true;
        double vertexValue;
        double edgeValue;
        Vertex(){}
        Vertex(int vertexID){this->vertexID = vertexID;}
};

#endif
