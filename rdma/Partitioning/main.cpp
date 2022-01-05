#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "Parser.hpp"

using namespace std;
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

int main(int argc, const char *argv[]){
    
    ArgumentParser parser("Partitioing", "Partitioing execution");
	parser.add_argument()
      .names({"-f", "--filename"})
      .description("file name")
      .required(true);
    parser.add_argument()
      .names({"-H", "--hostnum"})
      .description("number of host")
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
    
    string filename = parser.get<string>("f");
    int host_num = stoi(parser.get<string>("H"));
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

    internalBucket = host_num;
    char buf[100];
	string s;
    vector<string> v;
    ifstream in(filename);
    ofstream writeFile[host_num];
    for(int i = 0; i < host_num; i++){
        writeFile[i].open(filename+"_"+to_string(host_num)+"_"+to_string(i));
    }
    
    while (true) {
        in.getline(buf, 100);
        if(in.eof())break;
		s = buf;
        s = s + "\n";
        v = split(s, delimiter);
        int filenum = internalHashFunction(stoi(v[0]));
        writeFile[filenum].write(s.c_str(), s.size());
    }
}