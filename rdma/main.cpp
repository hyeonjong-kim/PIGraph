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
	parser.add_argument()
      .names({"-b", "--buffersize"})
      .description("buffer size")
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
	int buffer_size= stoi(parser.get<string>("b"));
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
			PageRank p(stoi(v[0]),stoi(v[1]), rdma, num_host, socketmu);
			pagerank_set.insert(pair<int, PageRank>(stoi(v[0]), p));
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


	double** recv_msg = new double*[num_host];
	
	for(int i = 0; i < num_host; i++){
		recv_msg[i] = new double[pagerank_set.size()*buffer_size]{0.0,};
	}

	map<int, vector<int>> recv_pos;
	int begin_pos = 0;
	int end_pos = buffer_size-1;
	map<int, PageRank>::iterator iter;
	
	for(iter=pagerank_set.begin(); iter!=pagerank_set.end();iter++){
		iter->second.SetMsgQue(recv_msg);
		iter->second.SetPos(begin_pos, end_pos);
		
		vector<int> pos;
		pos.push_back(begin_pos);
		pos.push_back(end_pos);
		recv_pos.insert(make_pair(iter->first, pos));
		begin_pos += buffer_size;
		end_pos += buffer_size;
	}

	for(int i = 0; i < num_host; i++){
		rdma[i].setInfo(&t[i], recv_msg[i], pagerank_set.size()*buffer_size, recv_pos);
		rdma[i].ConnectRDMA();
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
	
	cout<< "start graph query" <<endl;
	gettimeofday(&start, NULL);
	for (int i = 0; i < superstep; i++) {
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
		for(int o = 0; o < num_host; o++)rdma[o].SendMsg(2147483647, 0.0);
		for(int o = 0; o < num_host; o++)rdma[o].CheckCommunication();
	}

	gettimeofday(&end, NULL);

	for(int i; i<num_host;i++)t[i].CloseSocket();
	/*
	for(iter=pagerank_set.begin(); iter!=pagerank_set.end();iter++){
		cout << iter->second.GetValue() << endl;
	}
	*/
	for(int o = 0; o < num_host; o++)rdma[o].CloseRDMA();

	time = end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0;
	cout << "toal time: " << time << endl;
   	
	return 0;
}
