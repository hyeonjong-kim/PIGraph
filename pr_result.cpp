#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cmath>

using namespace std;

#define pp pair<int, double>
bool cmp(const pp& a, const pp& b) {
	if (a.second == b.second) return a.first < b.first;
	return a.second > b.second;
}

vector<string> split(string input, char delimiter) {
	vector<string> answer;
    stringstream ss(input);
    string temp;

    while(getline(ss, temp, delimiter))answer.push_back(temp);

    return answer;
}

int main(){

    map<int,double> finalResult;
    ifstream data_file("am_result_4");
	vector<string> split_line;
    string read_str;
    while(getline(data_file, read_str)){
        split_line = split(read_str, '\t');
        double tmp = floor(stod(split_line[1])*100000)/100000;
        //finalResult.insert({stoi(split_line[0]), stod(split_line[1])});
        finalResult.insert({stoi(split_line[0]), tmp});
        //cerr.precision(10);
        //cerr << stoi(split_line[0]) << " " <<  tmp << endl;;
    }
    
    vector<pp> vec(finalResult.begin(), finalResult.end());
    sort(vec.begin(), vec.end(), cmp);
    string result = "";
    for(auto num:vec){
        if(num.first == 77967)cerr << num.first << ": " << num.second << endl;
        //cerr << num.first << ": " << num.second << endl;
        result += to_string(num.first) + "\n";
    }
    
    ofstream writeFile;
    writeFile.open("test.txt");
    writeFile.write(result.c_str(), result.size());
}