#include "Topology.hpp"

static const char *const usages[] = {
    "basic [options] [[--] args]",
    "basic [options]",
    NULL,
};

Topology::Topology(int argc, const char *argv[]){
    
    const char *data_file_name_C = NULL;
    const char *query_type_C= NULL;

    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("Basic options"),
        OPT_INTEGER('m', "mutex", &(this->num_mutex), "mutex number", NULL, 0, 0),
        OPT_STRING('f', "data_file_name", &(data_file_name_C), "path to read", NULL, 0, 0),
        OPT_INTEGER('n', "num_host", &(this->num_host), "host number", NULL, 0, 0),
        OPT_INTEGER('s', "superstep", &(this->superstep), "superstep number", NULL, 0, 0),
        OPT_INTEGER('p', "partition", &(this->p_option), "partitioning option", NULL, 0, 0),
        OPT_STRING('q', "query", &(query_type_C), "query type", NULL, 0, 0),
        OPT_END(),
    };
    

    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\nA brief description of what the program does and how it works.", "\nAdditional description of the program after the description of the arguments.");
    argc = argparse_parse(&argparse, argc, argv);
    this->data_file_name = data_file_name_C;
    this->query_type = query_type_C;
    
    if(this->p_option == 0){
        this->delimiter = ' ';
    }
    else if(this->p_option == 1){
        this->delimiter = '\t';
    }
    else{
        cout << "partitioning option error" << endl;
        exit(0);
    }

    if(query_type == "PR"){

    }
    else if(query_type == "SSSP"){

    }
    else if(query_type == "WCC"){

    }
    else if(query_type == "userdefined"){

    }
    else{
        exit(0);
    }

    this->compute_threadPool = new ThreadPool(this->num_thread);
	this->network_threadPool = new ThreadPool(this->num_host);

    this->compute_mu = new mutex[this->num_mutex];
    this->network_mu = new mutex[this->num_host];

    this->t = new tcp[this->num_host];
    this->rdma = new RDMA[this->num_host];

    this->externalBucket = this->num_host;
}


bool Topology::setTopology(int argc, const char *argv[]){
    /*
    this->parser = new argparse::ArgumentParser("Pigraph", "Pigraph execution");
	this->parser->add_argument()
      .names({"-m", "--mutex"})
      .description("mutex number")
      .required(true);
	this->parser->add_argument()
      .names({"-f", "--datafilename"})
      .description("data file name")
      .required(true);
	this->parser->add_argument()
      .names({"-n", "--hostnumber"})
      .description("numbers of host")
      .required(true);
	this->parser->add_argument()
      .names({"-s", "--superstep"})
      .description("sperstep")
      .required(true);
	this->parser->add_argument()
      .names({"-p", "--partitioning"})
      .description("partitioning option")
      .required(true);
    this->parser->add_argument()
      .names({"-q", "--queryname"})
      .description("query name")
      .required(true);
	this->parser->enable_help();

	auto err = this->parser->parse(argc, argv);
	if (err) {
		std::cout << err << std::endl;
		return false;
	}
	
	this->num_mutex = stoi(this->parser->get<string>("m"));
	this->data_file_name = this->parser->get<string>("f");
	this->num_host = stoi(this->parser->get<string>("n"));
	this->superstep = stoi(this->parser->get<string>("s"));
	this->p_option = stoi(this->parser->get<string>("p"));
    this->query_type = this->parser->get<string>("q");

    if(p_option == 0){
        this->delimiter = ' ';
    }
    else if(p_option == 1){
        this->delimiter = '\t';
    }
    else{
        cout << "partitioning option error" << endl;
        return false;
    }

    if(query_type == "PR"){

    }
    else if(query_type == "SSSP"){

    }
    else if(query_type == "WCC"){

    }
    else if(query_type == "userdefined"){

    }
    else{
        return false;
    }

    this->compute_threadPool = new ThreadPool(this->num_thread);
	this->network_threadPool = new ThreadPool(this->num_host);

    this->compute_mu = new mutex[this->num_mutex];
    this->network_mu = new mutex[this->num_host];

    this->t = new tcp[this->num_host];
    this->rdma = new RDMA[this->num_host];

    return true;
    */
}


bool Topology::readingDataFile(){

    ifstream data(this->data_file_name);
	string line;
    vector<string> split_line;
    gettimeofday(&(this->start), NULL);

    if(this->query_type == "PR"){
        while(getline(data, line)){
            split_line = this->split(line, delimiter);
            if(externalHashFunction(stoi(split_line[0])) == this->this_host_num){
                if(this->pagerank_set.count(stoi(split_line[0])) == 1){
                    this->pagerank_set.find(stoi(split_line[0]))->second.AddOutEdge(stoi(split_line[1]));
                }
                else{
                    PageRank p(stoi(split_line[0]),stoi(split_line[1]), NULL, this->rdma, this->network_mu, this->num_host);
                    this->pagerank_set.insert(pair<int, PageRank>(stoi(split_line[0]), p));
                }
            }

            if(externalHashFunction(stoi(split_line[1])) == this->this_host_num){
                if(this->pagerank_set.count(stoi(split_line[1])) == 1){
                    this->pagerank_set.find(stoi(split_line[1]))->second.AddInEdge(stoi(split_line[0]));
                }
                else{
                    PageRank p(stoi(split_line[1]), NULL, stoi(split_line[0]), this->rdma, this->network_mu, this->num_host);
                    this->pagerank_set.insert(pair<int, PageRank>(stoi(split_line[1]), p));
                }
            }
	    }
    }
    else if(query_type == "SSSP"){

    }
    else if(query_type == "WCC"){

    }
    else if(query_type == "userdefined"){

    }
    else{
        return false;
    }
    gettimeofday(&(this->end), NULL);
	data.close();
    double time = this->end.tv_sec + this->end.tv_usec / 1000000.0 - this->start.tv_sec - this->start.tv_usec / 1000000.0;
    cout << "time of reading file: " << time << endl;
    for(int i = 0; i < this->num_host; i++)this->t[i].SendCheckmsg();

    for(int j = 0; j < num_host; j++){
		this->network_threadPool->EnqueueJob([this, j](){
			string s = "";
			while(s.compare("1\n")!= 0){
				s = this->t[j].ReadCheckMsg();
			}
		});
	}
    cout << "Complete reading file all node" << endl;
    this->waitThreadPool(this->network_threadPool, 2);
   
}   

bool Topology::connectTCPNetwork(){
    char this_host_name[256];
	size_t this_host_name_len = 256;
	gethostname(this_host_name, this_host_name_len);
	string this_host_name_str(this_host_name);
	ifstream hostfile(this->host_file_name);

    char read_buf[100];
	string read_str;

	for(int i=0; i<num_host; i++){
		hostfile.getline(read_buf, 100);
		read_str = read_buf;
		if(this_host_name_str.compare(read_str) == 0){
			break;
		}
		this->this_host_num++;
	}
    
    vector<char[15]> server_ip(num_host);
    hostfile.close();

	hostfile.open(this->host_file_name);
	for(int i=0; i< num_host; i++){
		hostfile.getline(read_buf, 100);
		read_str = read_buf;
		read_str = HostToIp(read_str);
		strcpy(server_ip[i], read_str.c_str());
		this->t[i].SetInfo(i, 3141592, server_ip[i], num_host, 3141592+this->this_host_num);
		this->t[i].SetSocket();
        this->network_threadPool->EnqueueJob([this, i](){this->t[i].ConnectSocket();});
    }

    this->waitThreadPool(this->network_threadPool, 1);
}

bool Topology::connectRDMANetwork(){
    
    map<int, PageRank>::iterator iter;

	int begin_pos = 0;
	int end_pos = 0;
	int buffer_size = 0;
	this->recv_msg = new double*[num_host];
    
	for(iter=pagerank_set.begin(); iter!=pagerank_set.end();iter++){
		buffer_size += iter->second.GetInEdgeIterator().size();
		end_pos += iter->second.GetInEdgeIterator().size();
		
		iter->second.SetPos(begin_pos, end_pos);

		vector<int> pos;
		pos.push_back(begin_pos);
		pos.push_back(end_pos);
		this->recv_pos.insert(make_pair(iter->first, pos));
		begin_pos = end_pos;
	}

	for(int i = 0; i < num_host; i++){
		this->recv_msg[i] = new double[buffer_size]{0.0};
	}

	for(iter=pagerank_set.begin(); iter!=pagerank_set.end();iter++){
		iter->second.SetMsgQue(this->recv_msg);
	}

	for(int i = 0; i < num_host; i++){
		this->rdma[i].setInfo(&(this->t[i]), this->recv_msg[i], buffer_size, this->recv_pos, this->compute_mu, this->num_mutex);
		this->network_threadPool->EnqueueJob([this, i](){this->rdma[i].ConnectRDMA();});
		sleep(1);
	}

	this->waitThreadPool(this->network_threadPool, 3);
   

	for(int i = 0; i < num_host; i++)this->t[i].SendCheckmsg();
	
	for(int j = 0; j < num_host; j++){
		this->network_threadPool->EnqueueJob([this, j](){
			string s = "";
			while(s.compare("1\n")!= 0){
				s = this->t[j].ReadCheckMsg();
			}
			cout << this->t[j].GetServerAddr() << " is RDMA connection" << endl;
		});
	}

	this->waitThreadPool(this->network_threadPool, 4);
   
	
    cout << "Complete all node RDMA setting" << endl;

   return true;
}

bool Topology::readyToRun(){
    this->connectTCPNetwork();
    this->readingDataFile();
    cout << this->t[0].GetServerAddr() << endl;
    this->connectRDMANetwork();
}

bool Topology::run(){
    if(query_type == "PR"){

    }
    else if(query_type == "SSSP"){

    }
    else if(query_type == "WCC"){

    }
    else if(query_type == "userdefined"){

    }
    else{
        return false;
    }
}

vector<string> Topology::split(string input, char delimiter) {
	vector<string> answer;
    stringstream ss(input);
    string temp;

    while (getline(ss, temp, delimiter)) {
        answer.push_back(temp);
    }

    return answer;
}

string Topology::HostToIp(string host) {
    hostent* hostname = gethostbyname(host.c_str());
    if(hostname)
        return string(inet_ntoa(**(in_addr**)hostname->h_addr_list));
    return {};
}

bool Topology::waitThreadPool(ThreadPool* thread_pool, int num){
    while(true){
		if(thread_pool->getJobs().empty()){
			while(true){
                //cout << "fda" << endl;
 				if(thread_pool->checkAllThread()){
                    cout << "thread pool end "  <<  num << endl;
                    break;
                }
			}
			return true;
            break;
		}
	}
    return false;
}