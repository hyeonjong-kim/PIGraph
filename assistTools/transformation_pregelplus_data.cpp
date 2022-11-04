#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <queue>


using namespace std;

vector<string> split(string input, char delimiter) {
	vector<string> answer;
    stringstream ss(input);
    string temp;

    while (getline(ss, temp, delimiter)) {
        answer.push_back(temp);
    }

    return answer;
}

int main(){
    string path = "/home/hjkim/data/soc-LiveJournal1.txt";
    char delimiter  = '\t';
    ifstream data(path);
    string s;
    vector<string> v;
    map<int, queue<int>> m;
    int count = 0;

    while(getline(data, s)){
        v = split(s, delimiter);
        if(m.count(stoi(v[0])) == 0){
            queue<int> q;
            q.push(stoi(v[1]));
            m.insert(make_pair(stoi(v[0]),q));
            count++;
        }
        else{
            m.find(stoi(v[0]))->second.push(stoi(v[1]));
        }
	}
    
    map<int, queue<int>>::iterator iter;
    ofstream writeFile(path+"_for_pregelpuls");
    for(iter = m.begin(); iter != m.end(); iter++){
        s = to_string(iter->first) + "\t" + to_string(iter->second.size());
        while (!iter->second.empty())
        {   
            s += (" " + to_string(iter->second.front()));
            iter->second.pop();
        }
        s += "\n";
        writeFile.write(s.c_str(), s.size());
    }

    data.close();
    writeFile.close();
    return 0;
}
