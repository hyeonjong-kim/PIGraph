#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <cstdlib>
#include <netdb.h>
#include <sys/types.h>
#include <sys/sysinfo.h>

#include "SingleSourceShortestPath.hpp"
#include "tcp.hpp"
#include "RDMA.hpp"
#include "ThreadPool.hpp"
#include "Parser.hpp"


int internalBucket;
int externalBucket;
int internalHashFunction(int x){return (x % internalBucket);}
int externalHashFunction(int x){return (x % externalBucket);}

bool CheckHalt(map<int, SingleSourceShortestPath>& set){
	
	map<int, SingleSourceShortestPath>::iterator iter;
	for(iter = set.begin(); iter != set.end(); iter++){
		if(iter->second.GetState())return false;
	}

	return true;
}

string HostToIp(string host) {
    hostent* hostname = gethostbyname(host.c_str());
    if(hostname)
        return string(inet_ntoa(**(in_addr**)hostname->h_addr_list));
    return {};
}

vector<string> split(string input, char delimiter) {
	vector<string> answer;
    stringstream ss(input);
    string temp;

    while(getline(ss, temp, delimiter))answer.push_back(temp);

    return answer;
}

using namespace argparse;

int main(int argc, const char *argv[]){
	struct timeval start = {};
    struct timeval end = {};

	struct timeval start_reading = {};
    struct timeval end_reading = {};

	struct timeval start_query = {};
    struct timeval end_query = {};

	gettimeofday(&start, NULL);
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
      .names({"-d", "--sourceid"})
      .description("partitioning option")
      .required(true);
	parser.enable_help();
	
	auto err = parser.parse(argc, argv);
	if (err) {
		cerr << err << endl;
		return -1;
	}

	int num_thread = thread::hardware_concurrency();
	int num_mutex = stoi(parser.get<string>("m"));
	string data_file_name = parser.get<string>("f");
	string host_file_name = "hostfile/hostinfo.txt";
	int num_host = stoi(parser.get<string>("n"));
	int superstep = stoi(parser.get<string>("s"));
	int p_option= stoi(parser.get<string>("p"));
	int source_id = stoi(parser.get<string>("d"));
	int wake_thread_num = num_thread/num_host;
	bool check_alive_worker = true;

    char delimiter;
	
    if(p_option == 0){
        delimiter = ' ';
    }
    else if(p_option == 1){
        delimiter = '\t';
    }
    else{
        cerr << "partitioning option error" << endl;
        return 0;
    }

	char this_host_name[256];
	gethostname(this_host_name, 256);
	string this_host_name_str(this_host_name);
	int host_num = 0;

	ifstream host_file(host_file_name);
	char read_buf[100];
	string read_str;

	for(int i=0; i<num_host; i++){
		host_file.getline(read_buf, 100);
		read_str = read_buf;
		if(this_host_name_str.compare(read_str) == 0){
			break;
		}
		host_num++;
	}
	host_file.close();
	
	ThreadPool::ThreadPool threadPool(num_thread);
	ThreadPool::ThreadPool connectionThread(num_host);
	ThreadPool::ThreadPool RDMAconnectionThread(num_host);

	map<int, SingleSourceShortestPath> singleshortestpath_set;
	mutex mu[num_mutex];
	mutex socketmu[num_host];
	mutex wake_mu[num_mutex];
	internalBucket = num_mutex;
	externalBucket = num_host;

	tcp *t = new tcp[num_host];
	vector<char[15]> server_ip(num_host);
	
	host_file.open(host_file_name);
	
	std::vector<std::future<void>> futures;
	for(int i=0; i< num_host; i++){
		host_file.getline(read_buf, 100);
		read_str = read_buf;
		read_str = HostToIp(read_str);
		strcpy(server_ip[i], read_str.c_str());
		t[i].SetInfo(i, 3141592, server_ip[i], num_host, 3141592+host_num);
		t[i].SetSocket();
		auto f = [&t, i](){
			t[i].ConnectSocket();
		};

		futures.emplace_back(connectionThread.EnqueueJob(f));
	}
	
	for (auto& f_ : futures) {
    	f_.wait();
  	}

	host_file.close();
    
	RDMA *rdma = new RDMA[num_host];

	cerr<< "Read file" <<endl;

	ifstream data_file(data_file_name);
	vector<string> split_line;
	gettimeofday(&start_reading, NULL);
	while(getline(data_file, read_str)){
        split_line = split(read_str, delimiter);

		if(externalHashFunction(stoi(split_line[0])) == host_num){
			if(singleshortestpath_set.count(stoi(split_line[0])) == 1){
				singleshortestpath_set.find(stoi(split_line[0]))->second.AddOutEdge(stoi(split_line[1]));
				singleshortestpath_set.find(stoi(split_line[0]))->second.AddOutEdgeValue(1.0);
			}
			else{
				SingleSourceShortestPath singleshortestpath(stoi(split_line[0]),stoi(split_line[1]), NULL, rdma, socketmu, num_host, source_id, 1.0);
				singleshortestpath_set.insert(pair<int, SingleSourceShortestPath>(stoi(split_line[0]), singleshortestpath));
			}
		}
		if(externalHashFunction(stoi(split_line[1])) == host_num){
			if(singleshortestpath_set.count(stoi(split_line[1])) == 1){
				singleshortestpath_set.find(stoi(split_line[1]))->second.AddInEdge(stoi(split_line[0]));
				singleshortestpath_set.find(stoi(split_line[1]))->second.AddInEdgeValue(1.0);
			}
			else{
				SingleSourceShortestPath singleshortestpath(stoi(split_line[1]), NULL, stoi(split_line[0]), rdma, socketmu, num_host, source_id, 1.0);
				singleshortestpath_set.insert(pair<int, SingleSourceShortestPath>(stoi(split_line[1]), singleshortestpath));
			}
		}
	}
	gettimeofday(&end_reading, NULL);
	data_file.close();
	
	double time_reading = end_reading.tv_sec + end_reading.tv_usec / 1000000.0 - start_reading.tv_sec - start_reading.tv_usec / 1000000.0;
	cerr << "Time of reading file: " << time_reading << endl;

	for(int i = 0; i < num_host; i++)t[i].SendCheckmsg();
	
	for(int j = 0; j < num_host; j++){
		auto f = [t, j](){
			string s = "";
			while(s.compare("1\n")!= 0){
				s = t[j].ReadCheckMsg();
			}
		};
		futures.emplace_back(connectionThread.EnqueueJob(f));
	}

	for (auto& f_ : futures) {
    	f_.wait();
  	}

	cerr << "Complete reading file all node" << endl;

	map<int, SingleSourceShortestPath>::iterator iter;
	
	map<int, vector<int>> recv_pos;
	int begin_pos = 0;
	int end_pos = 0;
	int buffer_size = 0;
	double** recv_msg = new double*[num_host];

	for(iter=singleshortestpath_set.begin(); iter!=singleshortestpath_set.end();iter++){
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
		recv_msg[i] = new double[buffer_size]{0.0};
	}

	for(iter=singleshortestpath_set.begin(); iter!=singleshortestpath_set.end();iter++){
		iter->second.SetMsgQue(recv_msg);
	}

	for(int i = 0; i < num_host; i++){
		rdma[i].setInfo(&t[i], recv_msg[i], buffer_size, recv_pos, mu, num_mutex);
		auto f = [rdma, i, &t](){
			rdma[i].ConnectRDMA();
		};

		futures.emplace_back(RDMAconnectionThread.EnqueueJob(f));
	}
	
	for (auto& f_ : futures) {
    	f_.wait();
  	}
	
	
	for(int i = 0; i < num_host; i++)t[i].SendCheckmsg();
	
	for(int j = 0; j < num_host; j++){
		auto f = [&t, j](){
			string s = "";
			while(s.compare("1\n")!= 0){
				s = t[j].ReadCheckMsg();
			}
			cerr <<  t[j].GetServerAddr() << " is RDMA connection" << endl;
		};
		futures.emplace_back(connectionThread.EnqueueJob(f));
	}

	for (auto& f_ : futures) {
    	f_.wait();
  	}

	
	cerr << "Complete all node RDMA setting" << endl;

	cout<< "start graph query" <<endl;
	gettimeofday(&start_query, NULL);
	for (int i = 0; i < superstep; i++) {
		cerr << "superstep " << i << endl;
		
		if(check_alive_worker){
			for(iter=singleshortestpath_set.begin(); iter!=singleshortestpath_set.end();iter++){
				auto f = [iter](){
					if(iter->second.GetState())iter->second.Compute();
				};
				futures.emplace_back(threadPool.EnqueueJob(f));
			}

			for (auto& f_ : futures) {
				f_.wait();
			}
			
			for(int o = 0; o < num_host; o++){
				auto f = [&rdma, o, &t](){
					rdma[o].SendMsg(NULL, 0.0);
				};

				futures.emplace_back(connectionThread.EnqueueJob(f));
			}
			
			for (auto& f_ : futures) {
				f_.wait();
			}
		}
		
		check_alive_worker = false;

		for(int o = 0; o < num_host; o++){
				auto f = [rdma, o, &singleshortestpath_set, &wake_mu, wake_thread_num](){
					string _msg = rdma[o].GetWakeVertex();
					vector<string> split_msg = split(_msg, '\n');
					
					int start = 0;
					int end_interval = int(split_msg.size()) / int(wake_thread_num);
					int end = end_interval;
					thread t[wake_thread_num];

					for (size_t u = 0; u < wake_thread_num; u++){
						t[u] = thread([&wake_mu, start, end, &singleshortestpath_set, &split_msg](){
							for (size_t z = start; z < end; z++){
								wake_mu[internalHashFunction(stoi(split_msg[z]))].lock();
								singleshortestpath_set.find(stoi(split_msg[z]))->second.IsWake();
								wake_mu[internalHashFunction(stoi(split_msg[z]))].unlock();
							}
						});
						start = end;
						if(u+1 == wake_thread_num-1){
							end = split_msg.size();						
						}
						else{
							end = end + end_interval;
						}
					}

					for (size_t u = 0; u < wake_thread_num; u++){
						t[u].join();
					}
					
					rdma[o].ClearWakeVertex();
				};

				futures.emplace_back(connectionThread.EnqueueJob(f));
			}

			for (auto& f_ : futures) {
    			f_.wait();
  			}

			if(CheckHalt(singleshortestpath_set)){
				for (size_t u = 0; u < num_host; u++){
					t[u].Sendmsg("dead");
					t[u].Sendmsg("Q");
				}
			}
			else{
				for (size_t u = 0; u < num_host; u++){
					t[u].Sendmsg("alive");
					t[u].Sendmsg("Q");
				}
			}
			
			for (size_t u = 0; u < num_host; u++){
				if(t[u].Readmsg().compare("alive") == 0)check_alive_worker = true;
			}

			if(check_alive_worker == false)break;
	}

	gettimeofday(&end_query, NULL);

	for(int i; i<num_host;i++)t[i].CloseSocket();
	for(int o = 0; o < num_host; o++)rdma[o].CloseRDMA();
	
	gettimeofday(&end, NULL);

	double time = end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0;
	double time_query = end_query.tv_sec + end_query.tv_usec / 1000000.0 - start_query.tv_sec - start_query.tv_usec / 1000000.0;
	
	for(iter=singleshortestpath_set.begin(); iter!=singleshortestpath_set.end();iter++){
		cout << iter->first << ": " <<  iter->second.GetValue() << endl;
	}
	

	cerr << "toal query time: " << time_query << endl;
	cerr << "toal time: " << time << endl;

	
	return 0;
}
