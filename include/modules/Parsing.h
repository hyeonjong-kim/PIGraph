#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

#include "../utils/ArgParsing.h"
#include "../utils/tinyxml.h"

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
        
        map<string,string>& getConfig(){return this->config;}

        bool argParse(int argc, const char *argv[]);
        bool xmlParse();
        bool run(int argc, const char *argv[]);
};

bool Parsing::argParse(int argc, const char *argv[]){    
        this->parser->add_argument()
        .names({"-f", "--file_path"})
        .description("file path")
        .required(true);
        this->parser->add_argument()
        .names({"-w", "--worker_number"})
        .description("numbers of worker")
        .required(true);
        this->parser->add_argument()
        .names({"-s", "--superstep"})
        .description("sperstep")
        .required(true);
        this->parser->add_argument()
        .names({"-p", "--partitioning_opt"})
        .description("partitioning option")
        .required(true);
        this->parser->add_argument()
        .names({"-n", "--network_type"})
        .description("network type")
        .required(true);
        this->parser->add_argument()
        .names({"-u", "--processing_unit"})
        .description("processing unit")
        .required(true);
        this->parser->add_argument()
        .names({"-P", "--Port"})
        .description("information of port")
        .required(true);
        this->parser->add_argument()
        .names({"-q", "--query"})
        .description("information of port")
        .required(true);
        this->parser->enable_help();
	
        auto err = parser->parse(argc, argv);
	if (err) {
		std::cout << err << std::endl;
		return -1;
	}
        
        this->config.insert({"filePath", this->parser->get<string>("f")});
        this->config.insert({"numWorker", this->parser->get<string>("w")});
        this->config.insert({"superstep", this->parser->get<string>("s")});
        this->config.insert({"patitionOpt", this->parser->get<string>("p")});
        this->config.insert({"networkType", this->parser->get<string>("n")});
        this->config.insert({"processingUnit", this->parser->get<string>("u")});
        this->config.insert({"port", this->parser->get<string>("P")});
        this->config.insert({"query", this->parser->get<string>("q")});

        return true;
}

bool Parsing::xmlParse(){
        this->readDoc->LoadFile("/home/hjkim/PiGraph/conf/pigraph-conf.xml");
        TiXmlElement* rootElement = this->readDoc->FirstChildElement( "configuration" );
        TiXmlElement* element = rootElement->FirstChildElement("property");

        while(element){
                string name = element->FirstChildElement("name")->GetText();
                string value = element->FirstChildElement("value")->GetText();
                this->config.insert({name, value});
                element = element->NextSiblingElement("property");
        }

        return true;
}

bool Parsing::run(int argc, const char *argv[]){
        if(!this->argParse(argc, argv))return false;
        if(!this->xmlParse())return false;
        
        return true;
}
