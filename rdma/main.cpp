#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <cstdlib>
#include <netdb.h>
#include <sys/types.h>
#include <sys/sysinfo.h>

#include "PageRank.hpp"
#include "ThreadPool.hpp"
#include "Parser.hpp"
#include "tcp.hpp"
#include "RDMA.hpp"
#include "ResourceChecker.hpp"

using namespace argparse;
struct sysinfo memInfo;

int internalBucket;
int internalHashFunction(int x){
	return (x % internalBucket);
}

int externalBucket;
int externalHashFunction(int x){
	return (x % externalBucket);
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
	ThreadPool* RDMAconnectionThread = new ThreadPool(num_host);

	map<int, PageRank> pagerank_set;
	mutex mu[num_mutex];
	mutex socketmu[num_host];
	internalBucket = num_mutex;
	externalBucket = num_host;

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

	cout<< "Read file" <<endl;

	ifstream data(file_name);
	
	gettimeofday(&start, NULL);
	while(getline(data, s)){
        v = split(s, delimiter);

		if(externalHashFunction(stoi(v[0])) == hostnum){
			if(pagerank_set.count(stoi(v[0])) == 1){
				pagerank_set.find(stoi(v[0]))->second.AddOutEdge(stoi(v[1]));
			}
			else{
				PageRank p(stoi(v[0]),stoi(v[1]), NULL, rdma, num_host, socketmu);
				pagerank_set.insert(pair<int, PageRank>(stoi(v[0]), p));
			}
		}

		if(externalHashFunction(stoi(v[1])) == hostnum){
			if(pagerank_set.count(stoi(v[1])) == 1){
				pagerank_set.find(stoi(v[1]))->second.AddInEdge(stoi(v[0]));
			}
			else{
				PageRank p(stoi(v[1]), NULL, stoi(v[0]), rdma, num_host, socketmu);
				pagerank_set.insert(pair<int, PageRank>(stoi(v[1]), p));
			}
		}
	}

	gettimeofday(&end, NULL);
	data.close();

	double time = end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0;
	cout << "Time of reading file: " << time << endl;

	for(int i = 0; i < num_host; i++)t[i].SendCheckmsg();
	
	for(int j = 0; j < num_host; j++){
		connectionThread->EnqueueJob([t, j](){
			string s = "";
			while(s.compare("1\n")!= 0){
				s = t[j].ReadCheckMsg();
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
	
	map<int, vector<int>> recv_pos;
	int begin_pos = 0;
	int end_pos = 0;
	int buffer_size = 0;
	double** recv_msg = new double*[num_host];


	for(iter=pagerank_set.begin(); iter!=pagerank_set.end();iter++){
		buffer_size += iter->second.GetInEdgeIterator().size();
		end_pos += iter->second.GetInEdgeIterator().size();
		
		iter->second.SetPos(begin_pos, end_pos);

		vector<int> pos;
		pos.push_back(begin_pos);
		pos.push_back(end_pos);
		recv_pos.insert(make_pair(iter->first, pos));
		begin_pos = end_pos;
	}

	for(int i = 0; i < num_host; i++){
		recv_msg[i] = new double[buffer_size]{0.0,};
	}

	for(iter=pagerank_set.begin(); iter!=pagerank_set.end();iter++){
		iter->second.SetMsgQue(recv_msg);
	}

	for(int i = 0; i < num_host; i++){
		rdma[i].setInfo(&t[i], recv_msg[i], buffer_size, recv_pos);
		RDMAconnectionThread->EnqueueJob([rdma, i, &t](){
			rdma[i].ConnectRDMA();
		});
		sleep(1);
	}

	while(true){
		if(RDMAconnectionThread->getJobs().empty()){
			while(true){
				if(RDMAconnectionThread->checkAllThread()){
					break;
				}
			}
			break;
		}
	}

	for(int i = 0; i < num_host; i++)t[i].SendCheckmsg();
	
	for(int j = 0; j < num_host; j++){
		connectionThread->EnqueueJob([&t, j](){
			string s = "";
			while(s.compare("1\n")!= 0){
				s = t[j].ReadCheckMsg();
			}
			cout <<  t[j].GetServerAddr() << " is RDMA connection" << endl;
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
	
	
	cout << "Complete all node RDMA setting" << endl;
	
	struct timeval start_query = {};
	struct timeval end_query = {};
	struct timeval start_network = {};
	struct timeval end_network = {};
	
	ResourceChecker* resourceChecker = new ResourceChecker();
	resourceChecker->init();
	thread resourceCheckerThread([](ResourceChecker* _resourceChecker){
		while (1)
		{
			_resourceChecker->getCurrentValue();
			sleep(1);
		}
		
	}, resourceChecker);

	cout<< "start graph query" <<endl;
	gettimeofday(&start, NULL);
	for (int i = 0; i < superstep; i++) {
		gettimeofday(&start_query, NULL);
		for(iter=pagerank_set.begin(); iter!=pagerank_set.end();iter++){
			threadPool->EnqueueJob([iter](){iter->second.Compute();});
		}
		
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
		gettimeofday(&start_network, NULL);
		for(int o = 0; o < num_host; o++)rdma[o].SendMsg(2147483647, 0.0);
		for(int o = 0; o < num_host; o++)rdma[o].CheckCommunication();
		gettimeofday(&end_network, NULL);
		cout <<  "network time is " << end_network.tv_sec + end_network.tv_usec / 1000000.0 - start_network.tv_sec - start_network.tv_usec / 1000000.0 << endl;
	}

	gettimeofday(&end, NULL);

	for(int i; i<num_host;i++)t[i].CloseSocket();
	
	for(iter=pagerank_set.begin(); iter!=pagerank_set.end();iter++){
		cout << iter->second.GetValue() << endl;
	}
	
	for(int o = 0; o < num_host; o++)rdma[o].CloseRDMA();
	resourceCheckerThread.detach();
	resourceChecker->printValue();
	
	time = end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0;
	cout << "toal time: " << time << endl;
   	
	return 0;
}
