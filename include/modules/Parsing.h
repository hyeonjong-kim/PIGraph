#ifndef PARSING_H
#define PARSING_H

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
        map<string, string> argConfig;

    public:
        Parsing(){
            this->parser = new ArgumentParser("Pigraph_Master", "Pigraph Master");
        }
        
        map<string,string> getArgConfig(){return this->argConfig;}
        bool argParse(int argc, const char *argv[]);
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
        this->parser->add_argument()
        .names({"-j", "--job"})
        .description("information of port")
        .required(true);
        this->parser->enable_help();
	
        auto err = parser->parse(argc, argv);
	    if(err){
		    std::cout << err << std::endl;
		    exit(0);
	    }
        
        this->argConfig.insert({"filePath", this->parser->get<string>("f")});
        this->argConfig.insert({"numWorker", this->parser->get<string>("w")});
        this->argConfig.insert({"superstep", this->parser->get<string>("s")});
        this->argConfig.insert({"patitionOpt", this->parser->get<string>("p")});
        this->argConfig.insert({"networkType", this->parser->get<string>("n")});
        this->argConfig.insert({"processingUnit", this->parser->get<string>("u")});
        this->argConfig.insert({"port", this->parser->get<string>("P")});
        this->argConfig.insert({"query", this->parser->get<string>("q")});
        this->argConfig.insert({"jobId", this->parser->get<string>("j")});
        
        return true;
}

#endif
