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
    string path = "/home/hjkim/data/Amazon0601.txt";
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
    ofstream writeFile(path+"_for_Giraph");
    for(iter = m.begin(); iter != m.end(); iter++){
        s = "[" + to_string(iter->first) + "," + to_string(0) + ",[";
       

        while (!iter->second.empty())
        {   
            s += ("[" + to_string(iter->second.front()) + ",0" + "],");
            iter->second.pop();
        }
        
        s =s.substr(0, s.length()-1);
        s += "]]\n";
        //cout << s;
        writeFile.write(s.c_str(), s.size());
    }

    data.close();
    writeFile.close();

    return 0;

}