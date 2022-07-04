#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

#include "ArgParsing.h"
#include "tinyxml.h"

using namespace std;
using namespace argparse;

class Parsing{

    private:
        ArgumentParser* parser;
        TiXmlDocument* readDoc;
        map<string, string> config;

    public:
        Parsing(){
            this->parser = new ArgumentParser("Pigraph", "Pigraph execution");
            this->readDoc =  new TiXmlDocument();
        }
        
        bool argParse(int argc, const char *argv[]);
        bool xmlParse();
        bool run(int argc, const char *argv[]);
};