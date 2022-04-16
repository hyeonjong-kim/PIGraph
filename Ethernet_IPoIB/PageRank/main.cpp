#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <cstdlib>
#include <netdb.h>
#include <time.h>
#include <list>

#include "PageRank.hpp"
#include "ThreadPool.hpp"
#include "Parser.hpp"
#include "tcp.hpp"

using namespace argparse;

int internalBucket;
int internalHashFunction(int x){
	return (x % internalBucket);
}

int externalBucket;
int externalHashFunction(int x){
	return (x % externalBucket);
}

vector<string> split_simple(string& input, char delimiter) {
	vector<string> answer;
	stringstream ss(input);
    string temp;

    while (getline(ss, temp, delimiter)) {
        answer.push_back(temp);
    }

    return answer;
}


vector<vector<string>> split(string& input, char delimiter, int _msg_processing_thread_num) {
	vector<vector<string>> answer;
	for (size_t i = 0; i < _msg_processing_thread_num; i++)
	{
		vector<string> answer_element;
		answer.push_back(answer_element);
	}
	
	thread t_[_msg_processing_thread_num];
	int interval = input.size()/_msg_processing_thread_num;
	int start = 0;
	string input_tmp[_msg_processing_thread_num];

	for (size_t i = 0; i < _msg_processing_thread_num; i++){
		input_tmp[i]= "";
		input_tmp[i] = input.substr(start, interval);

		if(i < _msg_processing_thread_num-1){
			while (input_tmp[i].back() != '\n'){
				input_tmp[i] += input[start+interval];
				interval++;
			}
		}

		t_[i] = thread([&answer, i, &input_tmp, delimiter](){
			stringstream ss(input_tmp[i]);
    		string temp;
    		while (getline(ss, temp, delimiter)) {
				
				answer[i].push_back(temp);
    		}
		});

		start += interval;
	}

	for (size_t i = 0; i < _msg_processing_thread_num; i++){
		t_[i].join();
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
      .names({"-N", "--network"})
      .description("network mode")
      .required(true);
	parser.add_argument()
      .names({"-p", "--partitioning"})
      .description("partitioning option")
      .required(true);
	parser.enable_help();
	
	auto err = parser.parse(argc, argv);
	if (err) {
		std::cerr << err << std::endl;
		return -1;
	}
	
	//Parameter
	int num_thread = thread::hardware_concurrency();
	int num_mutex = stoi(parser.get<string>("m"));
	string file_name = parser.get<string>("f");
	string host_file = "hostfile/hostinfo.txt";
	int num_host = stoi(parser.get<string>("n"));
	int superstep = stoi(parser.get<string>("s"));
	string network_mode = parser.get<string>("N");
	int p_option= stoi(parser.get<string>("p"));
	int msg_processing_thread_num = num_thread/num_host;
    
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

	if(network_mode.compare("ethernet")==0){
	}
	else if(network_mode.compare("ipoib")==0){
		host_file = "hostfile/hostinfo_ib.txt";
	}
	else{
		cerr << "network mode error" << endl;
		return 0;
	}

	ThreadPool::ThreadPool threadPool(num_thread);
	ThreadPool::ThreadPool connectionThread(num_host);

	map<int, queue<double>>* messages = new map<int, queue<double>>;
	map<int, PageRank> pagerank_set;
	mutex mu[num_mutex];
	mutex socketmu[num_host];
	internalBucket = num_mutex;
	externalBucket = num_host;
	
	tcp *t = new tcp[num_host];
	
	vector<char[15]> server_ip(num_host);
	vector<string> v;
	hostfile.open(host_file);

	std::vector<std::future<void>> futures;
	for(int i=0; i<num_host; i++){
		hostfile.getline(buf, 100);
		s = buf;
		s = HostToIp(s);
		strcpy(server_ip[i], s.c_str());
		t[i].SetInfo(i, 3141592, server_ip[i], num_host, 3141592+hostnum, num_mutex);
		t[i].SetSocket();
		
		auto f = [&t, i](){t[i].ConnectSocket();};
		futures.emplace_back(connectionThread.EnqueueJob(f));
	}

	for (auto& f_ : futures) {
    	f_.wait();
  	}

	cerr<< "Read file" <<endl;
	
	ifstream data(file_name);
	
	map<int, int> check_vertex_num;

	gettimeofday(&start_reading, NULL);
	while(getline(data, s)){
        v = split_simple(s, delimiter);

		if(externalHashFunction(stoi(v[0])) == hostnum){
			if(pagerank_set.count(stoi(v[0])) == 1){
				pagerank_set.find(stoi(v[0]))->second.AddOutEdge(stoi(v[1]));
			}
			else{
				PageRank p(stoi(v[0]),stoi(v[1]), NULL, messages, t, num_host, socketmu);
				pagerank_set.insert(pair<int, PageRank>(stoi(v[0]), p));
			}
		}

		if(externalHashFunction(stoi(v[1])) == hostnum){
			if(pagerank_set.count(stoi(v[1])) == 1){
				pagerank_set.find(stoi(v[1]))->second.AddInEdge(stoi(v[0]));
			}
			else{
				PageRank p(stoi(v[1]), NULL, stoi(v[0]), messages, t, num_host, socketmu);
				pagerank_set.insert(pair<int, PageRank>(stoi(v[1]), p));
				
			}
		}

		if(check_vertex_num.count(stoi(v[0])) != 1)check_vertex_num.insert(pair<int,int>(stoi(v[0]), 1));
		if(check_vertex_num.count(stoi(v[1])) != 1)check_vertex_num.insert(pair<int,int>(stoi(v[1]), 1));
	}
	gettimeofday(&end_reading, NULL);
	data.close();
	
	double time_reading = end_reading.tv_sec + end_reading.tv_usec / 1000000.0 - start_reading.tv_sec - start_reading.tv_usec / 1000000.0;
	cerr << "Time of reading file: " << time_reading << endl;

	map<int, PageRank>::iterator iter;
	for(iter=pagerank_set.begin(); iter!=pagerank_set.end();iter++){
		queue<double> q;
		messages->insert(make_pair(iter->first, q));
	}

	for(iter=pagerank_set.begin(); iter!=pagerank_set.end();iter++){
		iter->second.SetValue(1.0/check_vertex_num.size());
	}

	for(int j = 0; j < num_host; j++){
		auto f = [&t, j](){
			t[j].SendCheckmsg();
			string s = "";
			while(s.compare("1\n")!= 0){
				s = t[j].ReadCheckmsg();
			}
			cerr <<  t[j].GetServerAddr() << " is complete read file" <<  endl;
		};

		futures.emplace_back(connectionThread.EnqueueJob(f));
	}

	for (auto& f_ : futures) {
    	f_.wait();
  	}

	cerr<< "start graph query" <<endl;
	futures.clear();
	gettimeofday(&start_query, NULL);
	for (int i = 0; i < superstep; i++) {
		cerr << "supertstep " << i <<  endl;

		for(iter=pagerank_set.begin(); iter!=pagerank_set.end();iter++){
			auto f = [iter](){iter->second.Compute();};
			futures.emplace_back(threadPool.EnqueueJob(f));
		}

		for (auto& f_ : futures) {
    		f_.wait();
  		}
		futures.clear();

		for(int j = 0; j < num_host; j++){
			auto f = [&t, j](){
				string end_msg = "Q";
				t[j].Sendmsg(end_msg, 0);
			};

			futures.emplace_back(connectionThread.EnqueueJob(f));
		}
		
		for (auto& f_ : futures) {
    		f_.wait();
  		}
		futures.clear();

		for(int j = 0; j < num_host; j++){
			auto f = [&t, j, &mu, num_host, &pagerank_set,&messages, msg_processing_thread_num](){
				struct timeval start_tmp  = {};
    			struct timeval end_tmp = {};
				
				string read_msg = t[j].Readmsg();
				vector<vector<string>> result = split(read_msg, '\n', msg_processing_thread_num);
				thread t[msg_processing_thread_num];
				
				for (size_t u = 0; u < msg_processing_thread_num; u++){
					t[u] = thread([&result, &pagerank_set, &messages, &mu, u](){
						vector<string> msg;
						for(int k = 0; k < result[u].size(); k++){
							msg = split_simple(result[u][k], ' ');
							int mu_num = internalHashFunction(stoi(msg[0]));
							mu[mu_num].lock();
							messages->find(stoi(msg[0]))->second.push(stod(msg[1]));
							mu[mu_num].unlock();
						}
					});
				}
				
				for (size_t u = 0; u < msg_processing_thread_num; u++){
					t[u].join();
				}
			};

			futures.emplace_back(connectionThread.EnqueueJob(f));
		}

		for (auto& f_ : futures) {
    		f_.wait();
  		}
		futures.clear();

		
	}
	
	gettimeofday(&end_query, NULL);

	
	for(int o = 0; o<num_host;o++)t[o].CloseSocket();

	gettimeofday(&end, NULL);
	
	double time = end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0;
	double time_query = end_query.tv_sec + end_query.tv_usec / 1000000.0 - start_query.tv_sec - start_query.tv_usec / 1000000.0;

	for(iter=pagerank_set.begin(); iter!=pagerank_set.end();iter++){
		cerr << iter->first << ": " << iter->second.GetValue() << endl;
	}
	
	cerr << "Time of reading file: " << time_reading << endl;
	cerr << "toal query time: " << time_query << endl;
	cerr << "query + reading + preprocessing: " << time_reading + time_query<< endl;
	cerr << "toal time: " << time << endl;

	return 0;
}
