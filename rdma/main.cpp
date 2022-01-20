#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <cstdlib>
#include <netdb.h>

#include "PageRank.hpp"
#include "ThreadPool.hpp"
#include "Parser.hpp"
#include "tcp.hpp"
#include "RDMA.hpp"

using namespace argparse;

int internalBucket;
int internalHashFunction(int x){
	return (x % internalBucket);
}

vector<string> split(string input, char delimiter) {
	vector<string> answer;
    stringstream ss(input);
    string temp;

    while (getline(ss, temp, delimiter)) {
        answer.push_back(temp);
    }

    return answer;
}

string HostToIp(string host) {
    hostent* hostname = gethostbyname(host.c_str());
    if(hostname)
        return string(inet_ntoa(**(in_addr**)hostname->h_addr_list));
    return {};
}

int main(int argc, const char *argv[]){	
	
	//Parser info
	ArgumentParser parser("Pigraph", "Pigraph execution");
	parser.add_argument()
      .names({"-m", "--mutex"})
      .description("mutex number")
      .required(true);
	parser.add_argument()
      .names({"-f", "--filename"})
      .description("file name")
      .required(true);
	parser.add_argument()
      .names({"-n", "--hostnumber"})
      .description("number of host")
      .required(true);
	parser.add_argument()
      .names({"-s", "--superstep"})
      .description("sperstep")
      .required(true);
	parser.add_argument()
      .names({"-p", "--partitioning"})
      .description("partitioning option")
      .required(true);
	parser.enable_help();
	
	auto err = parser.parse(argc, argv);
	if (err) {
		std::cout << err << std::endl;
		return -1;
	}
	
	//Parameter
	int num_thread = thread::hardware_concurrency();
	int num_mutex = stoi(parser.get<string>("m"));
	string file_name = parser.get<string>("f");
	string host_file = "../hostfile/hostinfo.txt";
	int num_host = stoi(parser.get<string>("n"));
	int superstep = stoi(parser.get<string>("s"));
	int p_option= stoi(parser.get<string>("p"));
    char delimiter;
	

    if(p_option == 0){
        delimiter = ' ';
    }
    else if(p_option == 1){
        delimiter = '\t';
    }
    else{
        cout << "partitioning option error" << endl;
        return 0;
    }

	char hostname[256];
	size_t hostnamelen = 256;
	gethostname(hostname, hostnamelen);
	string hostname_str(hostname);
	int hostnum = 0;

	ifstream hostfile(host_file);
	char buf[100];
	string s;

	for(int i=0; i<num_host; i++){
		hostfile.getline(buf, 100);
		s = buf;
		if(hostname_str.compare(s) == 0){
			break;
		}
		hostnum++;
	}

	hostfile.close();

	struct timeval start = {};
    struct timeval end = {};

	ThreadPool* threadPool = new ThreadPool(num_thread);
	ThreadPool* connectionThread = new ThreadPool(num_host);
	map<int, queue<double>>* messages = new map<int, queue<double>>;
	map<int, PageRank> pagerank_set;
	mutex mu[num_mutex];
	mutex socketmu[num_host];
	internalBucket = num_mutex;

	tcp *t = new tcp[num_host];
	vector<char[15]> server_ip(num_host);
	vector<string> v;
	hostfile.open(host_file);

	for(int i=0; i< num_host; i++){
		hostfile.getline(buf, 100);
		s = buf;
		s = HostToIp(s);
		strcpy(server_ip[i], s.c_str());
		t[i].SetInfo(i, 3141592, server_ip[i], num_host, 3141592+hostnum);
		t[i].SetSocket();
		connectionThread->EnqueueJob([&t, i](){t[i].ConnectSocket();});
	}

	while(true){
			if(connectionThread->getJobs().empty()){
				while(true){
					if(connectionThread->checkAllThread())break;
				}
				break;
			}
	}
    
	RDMA *rdma = new RDMA[num_host];

	
	for(int i = 0; i < num_host; i++){
		rdma[i].setInfo(&t[i]);
		rdma[i].ConnectRDMA();
	}

	cout<< "Read file" <<endl;

	ifstream data(file_name);
	
	gettimeofday(&start, NULL);
	while (true) {
        data.getline(buf, 100);
        if(data.eof())break;

		s = buf;
        v = split(s, delimiter);

		if(pagerank_set.count(stoi(v[0])) == 1){
			pagerank_set.find(stoi(v[0]))->second.AddOutEdge(stoi(v[1]));
		}
		else{
			PageRank p(stoi(v[0]),stoi(v[1]), messages, rdma, num_host, socketmu);
			pagerank_set.insert(pair<int, PageRank>(stoi(v[0]), p));
		}
	}

	gettimeofday(&end, NULL);
	data.close();

	double time = end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0;
	cout << "Time of reading file: " << time << endl;

    for(int i = 0; i < num_host; i++)t[i].SendCheckmsg();
	
	for(int j = 0; j < num_host; j++){
		connectionThread->EnqueueJob([&t, j](){
			string s = "";
			while(s.compare("1\n")!= 0){
				s = t[j].CheckReadfile();
			}
		});
	}

	while(true){
		if(connectionThread->getJobs().empty()){
			while(true){
				if(connectionThread->checkAllThread())break;
			}
			break;
		}
	}
	cout << "Complete reading file all node" << endl;

	map<int, PageRank>::iterator iter;
	for(iter=pagerank_set.begin(); iter!=pagerank_set.end();iter++){
		queue<double> q;
		messages->insert(make_pair(iter->first, q));
	}
	

	struct timeval start_query = {};
	struct timeval end_query = {};
	struct timeval start_network = {};
	struct timeval end_network = {};

	double query_time = 0.0;
	double network_time = 0.0;
	

	cout<< "start graph query" <<endl;
	gettimeofday(&start, NULL);
	for (int i = 0; i < superstep; i++) {

		gettimeofday(&start_query, NULL);
		//Conduct Query
		for(iter=pagerank_set.begin(); iter!=pagerank_set.end();iter++){
			threadPool->EnqueueJob([iter](){iter->second.Compute();});
		}
		//Wait to complete Query
		while(true){
			if(threadPool->getJobs().empty()){
				while(true){
					if(threadPool->checkAllThread())break;
				}
				break;
			}
		}

		gettimeofday(&end_query, NULL);
		cout <<  "query time is " << end_query.tv_sec + end_query.tv_usec / 1000000.0 - start_query.tv_sec - start_query.tv_usec / 1000000.0 << endl;
		query_time += end_query.tv_sec + end_query.tv_usec / 1000000.0 - start_query.tv_sec - start_query.tv_usec / 1000000.0;
		for(int o = 0; o < num_host; o++)rdma[o].SendMsg("Q");
		
		
		gettimeofday(&start_network, NULL);
		for(int j = 0; j < num_host; j++){
			threadPool->EnqueueJob([&rdma, j, &mu, num_host, &pagerank_set,&messages](){
				int count = 0;
				string s(rdma[j].ReadMsg());
				vector<string> result;
				vector<string> msg;
				vector<string> v;
				v = split(s, '\n');

				for(int k = 0; k < v.size(); k++){
					msg = split(v[k], ' ');
					
					if(msg.size()==2){
						count++;
					}
					
					if(msg.size() ==2 && pagerank_set.count(stoi(msg[0])) == 1){
						
						int mu_num = internalHashFunction(stoi(msg[0]));
						mu[mu_num].lock();
						messages->find(stoi(msg[0]))->second.push(stod(msg[1]));
						mu[mu_num].unlock();
					}
					
				}
				cout <<  "msg amount is " << count << endl;
		});
	}

		while(true){
			if(threadPool->getJobs().empty()){
				while(true){
					if(threadPool->checkAllThread())break;
				}
				break;
			}
		}
		
		gettimeofday(&end_network, NULL);
		cout <<  "network time is " << end_network.tv_sec + end_network.tv_usec / 1000000.0 - start_network.tv_sec - start_network.tv_usec / 1000000.0 << endl;

		network_time += end_network.tv_sec + end_network.tv_usec / 1000000.0 - start_network.tv_sec - start_network.tv_usec / 1000000.0;
	}

	gettimeofday(&end, NULL);

	for(int i; i<num_host;i++)t[i].CloseSocket();
	
	for(iter=pagerank_set.begin(); iter!=pagerank_set.end();iter++){
		cout << iter->second.GetValue() << endl;
	}
	
	time = end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0;
	cout << "toal time: " << time << endl;
	cout << "toal network time: " << time << endl;
	cout << "toal query time: " << time << endl;


	for(int o = 0; o < num_host; o++)rdma[o].CloseRDMA();
	
	
	
	return 0;
}
