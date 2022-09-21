#ifndef SETTING_H
#define SETTING_H

#include <iostream>
#include <string>
#include <vector>
#include <mutex>

#include "../utils/ArgParsing.h"
#include "../utils/Tools.h"

using namespace std;
using namespace argparse;

class Setting{
    private:
        ArgumentParser* parser;
        Tools tools;
    public:
        int superstep;
        int port;
        int partitionOpt;
        int numMutex;
        string filePath;
        string query;
        string HDFS_host;
        int HDFS_port;
        string processingUnit;
        vector<string> workers;
        string networkType;
        mutex* mu;

        Setting(){this->parser = new ArgumentParser("Pigraph_Worker", "Pigraph Worker");}
        bool argParse(int argc, const char *argv[]);
};

bool Setting::argParse(int argc, const char *argv[]){
    this->parser->add_argument()
        .names({"-s", "--superstep"})
        .description("sperstep")
        .required(true);
    this->parser->add_argument()
        .names({"-P", "--Port"})
        .description("port information")
        .required(true);
    this->parser->add_argument()
        .names({"-f", "--file_path"})
        .description("file path")
        .required(true);
    this->parser->add_argument()
        .names({"-p", "--partitioning_opt"})
        .description("partitioning option")
        .required(true);
    this->parser->add_argument()
        .names({"-q", "--query"})
        .description("query")
        .required(true);
    this->parser->add_argument()
        .names({"-H", "--HDFS"})
        .description("HDFS information")
        .required(true);
    this->parser->add_argument()
        .names({"-u", "--processing_unit"})
        .description("processing unit")
        .required(true);
    this->parser->add_argument()
        .names({"-m", "--mutex"})
        .description("number of mutexs")
        .required(true);
    this->parser->add_argument()
        .names({"-w", "--workers"})
        .description("worker information")
        .required(true);
    this->parser->add_argument()
        .names({"-t", "--thread"})
        .description("number of thread")
        .required(true);
    this->parser->add_argument()
        .names({"-n", "--networkType"})
        .description("Network Type")
        .required(true);
    this->parser->enable_help();
    
    auto err = parser->parse(argc, argv);
    if(err){
        std::cout << err << std::endl;
        exit(0);
    }
    
    this->superstep = stoi(this->parser->get<string>("s"));
    this->port = stoi(this->parser->get<string>("P"));
    this->partitionOpt = stoi(this->parser->get<string>("p"));
    this->numMutex = stoi(this->parser->get<string>("m"));
    this->mu = new mutex[this->numMutex];
    this->filePath = this->parser->get<string>("f");
    this->query = this->parser->get<string>("q");
    this->processingUnit = this->parser->get<string>("u");
    string worker = this->parser->get<string>("w");
    this->workers = tools.split_simple(worker, ',');

    
    this->networkType = this->parser->get<string>("n");
    
    string HDFS = this->parser->get<string>("H");
    vector<string> split_HDFS = tools.split_simple(HDFS, ':');
    this->HDFS_host = split_HDFS[0];
    this->HDFS_port = stoi(split_HDFS[1]);

    return true;
}

#endif