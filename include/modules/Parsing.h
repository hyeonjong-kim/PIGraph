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
    .names({"-Q", "--query_type"})
    .description("query type")
    .required(true);
    this->parser->add_argument()
    .names({"-j", "--job"})
    .description("information of job id")
    .required(true);

    this->parser->add_argument()
    .names({"-f", "--file_path"})
    .description("file path")
    .required(false);
    this->parser->add_argument()
    .names({"-w", "--worker_number"})
    .description("numbers of worker")
    .required(false);
    this->parser->add_argument()
    .names({"-s", "--superstep"})
    .description("sperstep")
    .required(false);
    this->parser->add_argument()
    .names({"-p", "--partitioning_opt"})
    .description("partitioning option")
    .required(false);
    this->parser->add_argument()
    .names({"-n", "--network_type"})
    .description("network type")
    .required(false);
    this->parser->add_argument()
    .names({"-u", "--processing_unit"})
    .description("processing unit")
    .required(false);
    this->parser->add_argument()
    .names({"-P", "--Port"})
    .description("information of port")
    .required(false);
    this->parser->add_argument()
    .names({"-q", "--query"})
    .description("information of query")
    .required(false);
    this->parser->add_argument()
    .names({"-o", "--outputPath"})
    .description("information of output path")
    .required(false);
    this->parser->add_argument()
    .names({"-t", "--thread"})
    .description("information of thead")
    .required(false);
    this->parser->enable_help();

    auto err = parser->parse(argc, argv);
    if(err){
        std::cout << err << std::endl;
        exit(0);
    }
    
    string queryType = this->parser->get<string>("Q");
    for (size_t i = 0; i < queryType.size(); i++){
        queryType[i] = tolower(queryType[i]);
    }
    
    if(queryType == "submit"){
        this->argConfig.insert({"queryType", queryType});
        this->argConfig.insert({"filePath", this->parser->get<string>("f")});
        this->argConfig.insert({"numWorker", this->parser->get<string>("w")});
        this->argConfig.insert({"superstep", this->parser->get<string>("s")});
        this->argConfig.insert({"patitionOpt", this->parser->get<string>("p")});
        this->argConfig.insert({"outputPath", this->parser->get<string>("o")});
        this->argConfig.insert({"port", this->parser->get<string>("P")});
        this->argConfig.insert({"jobId", this->parser->get<string>("j")});
        this->argConfig.insert({"thread", this->parser->get<string>("t")});

        string processingUnit = this->parser->get<string>("u");
        string query = this->parser->get<string>("q");
        string networkType = this->parser->get<string>("n");

        for (size_t i = 0; i < processingUnit.size(); i++){
            processingUnit[i] = tolower(processingUnit[i]);
        }
        for (size_t i = 0; i < query.size(); i++){
            query[i] = tolower(query[i]);     
        }
        for (size_t i = 0; i < networkType.size(); i++){
            networkType[i] = tolower(networkType[i]);     
        }

        this->argConfig.insert({"processingUnit", processingUnit});
        this->argConfig.insert({"query", query});
        this->argConfig.insert({"networkType", networkType});
        return true;
    }
    if(queryType == "check"){
        this->argConfig.insert({"queryType", queryType});
        this->argConfig.insert({"jobId", this->parser->get<string>("j")});
    }
    if(queryType == "kill"){
        this->argConfig.insert({"queryType", queryType});
        this->argConfig.insert({"jobId", this->parser->get<string>("j")});
    }
}

#endif
