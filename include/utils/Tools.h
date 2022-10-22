#ifndef TOOLS_H
#define TOOLS_H

#include <iostream>
#include <sstream>
#include <vector>
#include <string>


using namespace std;

class Tools{

    private:
        
    public:
        vector<string> split_simple(string& input, char delimiter);
};

#endif