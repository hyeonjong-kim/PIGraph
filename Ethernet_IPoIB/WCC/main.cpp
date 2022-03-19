#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <cstdlib>
#include <netdb.h>
#include <time.h>
#include <list>

#include "WeaklyConnectedComponent.hpp"
#include "ThreadPool.hpp"
#include "Parser.hpp"
#include "tcp.hpp"

using namespace argparse;

bool CheckHalt(map<int, WeaklyConnectedComponent>& set){
	
	map<int, WeaklyConnectedComponent>::iterator iter;
	for(iter = set.begin(); iter != set.end(); iter++){
		if(iter->second.GetState())return false;
	}

	return true;
}

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

int main(int argc, const char *argv[])
{	
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
      .names({"-N", "--network"})
      .description("network mode")
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
		std::cerr << err << std::endl;
		return -1;
	}
	
	//Parameter
	int num_thread = thread::hardware_concurrency();
	int num_mutex = stoi(parser.get<string>("m"));
	string file_name = parser.get<string>("f");
	string host_file = "../hostfile/hostinfo.txt";
	int num_host = stoi(parser.get<string>("n"));
	int superstep = stoi(parser.get<string>("s"));
	string network_mode = parser.get<string>("N");
	int p_option= stoi(parser.get<string>("p"));
	int source_id = stoi(parser.get<string>("d"));
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
		host_file = "../hostfile/hostinfo_ib.txt";
	}
	else{
		cerr << "network mode error" << endl;
		return 0;
	}

	struct timeval start = {};
    struct timeval end = {};

	ThreadPool::ThreadPool threadPool(num_thread);
	ThreadPool::ThreadPool connectionThread(num_host);

	map<int, queue<double>>* messages = new map<int, queue<double>>;
	map<int, WeaklyConnectedComponent> wcc_set;
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
		t[i].SetInfo(i, 3141592, server_ip[i], num_host, 3141592+hostnum);
		t[i].SetSocket();
		
		auto f = [&t, i](){t[i].ConnectSocket();};
		futures.emplace_back(connectionThread.EnqueueJob(f));
	}

	for (auto& f_ : futures) {
    	f_.wait();
  	}

	cerr<< "Read file" <<endl;
	
	ifstream data(file_name);
	

	gettimeofday(&start, NULL);
	while(getline(data, s)){
        v = split(s, delimiter);

		if(externalHashFunction(stoi(v[0])) == hostnum){
			if(wcc_set.count(stoi(v[0])) == 1){
				wcc_set.find(stoi(v[0]))->second.AddOutEdge(stoi(v[1]));
				wcc_set.find(stoi(v[0]))->second.AddOutEdgeValue(1.0);
			}
			else{
				WeaklyConnectedComponent wcc(stoi(v[0]),stoi(v[1]), NULL, messages, t, num_host, socketmu, 1.0);
				wcc_set.insert(pair<int, WeaklyConnectedComponent>(stoi(v[0]), wcc));
			}
		}

		if(externalHashFunction(stoi(v[1])) == hostnum){
			if(wcc_set.count(stoi(v[1])) == 1){
				wcc_set.find(stoi(v[1]))->second.AddInEdge(stoi(v[0]));
				wcc_set.find(stoi(v[1]))->second.AddInEdgeValue(1.0);
			}
			else{
				WeaklyConnectedComponent wcc(stoi(v[1]), NULL, stoi(v[0]), messages, t, num_host, socketmu, 1.0);
				wcc_set.insert(pair<int, WeaklyConnectedComponent>(stoi(v[1]), wcc));
			}
		}
	}
	gettimeofday(&end, NULL);
	data.close();
	
	double time = end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0;
	cerr << "time of reading file: " << time << endl;

    for(int i = 0; i < num_host; i++)t[i].SendCheckmsg();
	
	for(int j = 0; j < num_host; j++){
		auto f = [&t, j](){
			string s = "";
			while(s.compare("1\n")!= 0){
				s = t[j].CheckReadfile();
			}
			cerr <<  t[j].GetServerAddr() << " is complete read file" <<  endl;
		};

		futures.emplace_back(connectionThread.EnqueueJob(f));
	}

	for (auto& f_ : futures) {
    	f_.wait();
  	}

	map<int, WeaklyConnectedComponent>::iterator iter;
	for(iter=wcc_set.begin(); iter!=wcc_set.end();iter++){
		queue<double> q;
		messages->insert(make_pair(iter->first, q));
	}

	cerr<< "start graph query" <<endl;
	gettimeofday(&start, NULL);
	for (int i = 0; i < superstep; i++) {
		
		if(CheckHalt(wcc_set))break;

		for(iter=wcc_set.begin(); iter!=wcc_set.end();iter++){
			auto f = [iter](){if(iter->second.GetState())iter->second.Compute();};
			futures.emplace_back(threadPool.EnqueueJob(f));
		}

		for (auto& f_ : futures) {
    		f_.wait();
  		}
		
		for(int j = 0; j < num_host; j++){
			auto f = [&t, j](){
				
				t[j].Sendmsg("Q");
			};

			futures.emplace_back(connectionThread.EnqueueJob(f));
		}

		for (auto& f_ : futures) {
    		f_.wait();
  		}

		for(int j = 0; j < num_host; j++){
			auto f = [&t, j, &mu, num_host, &wcc_set,&messages](){
				string read_msg = t[j].Readmsg();
				vector<string> msg;
				vector<string> result;
				result = split(read_msg, '\n');
				for(int k = 0; k < result.size(); k++){
					msg = split(result[k], ' ');
					if(msg.size() ==2 && wcc_set.count(stoi(msg[0])) == 1){
						int mu_num = internalHashFunction(stoi(msg[0]));
						mu[mu_num].lock();
						messages->find(stoi(msg[0]))->second.push(stod(msg[1]));
						wcc_set.find(stoi(msg[0]))->second.IsWake();
						mu[mu_num].unlock();	
					}
				}
			};

			futures.emplace_back(connectionThread.EnqueueJob(f));
		}

		for (auto& f_ : futures) {
    		f_.wait();
  		}

		cerr << "superstep" << i << endl;
	}

	gettimeofday(&end, NULL);

	for(int o; o<num_host;o++)t[o].CloseSocket();

	
	for(iter=wcc_set.begin(); iter!=wcc_set.end();iter++){
		cerr << iter->first << " " << iter->second.GetValue() << endl;
	}
	

	time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000000000.0;
	cerr << "time: " << time << endl;
	return 0;
}
