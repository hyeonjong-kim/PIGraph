// Server side C/C++ program to demonstrate Socket programming
#include "tcp.hpp"

#define PORT 3141592

vector<string> split(string input, char delimiter) {
            vector<string> answer;
            stringstream ss(input);
            string temp;

            while (getline(ss, temp, delimiter)) {
                answer.push_back(temp);
            }
            return answer;
        }

int main(int argc, char const *argv[])
{	
	string  s = "fdasfd" + "\\"+"fdsf";
	vector<string> v;
	v = = split(s, '\\');
	for(int i = 0; i < v.size();i++)cout << v[i] << endl;
	/*
	tcp t(3141592, 0, "192.168.0.100");
	string s = "123";
	vector<string> v;
	
	t.SetSocket();
	t.ConnectSocket();
	t.Sendmsg("msg1 ");
	t.Sendmsg("msg2 ");
	t.Sendmsg("msg3 ");
	t.Sendmsg("EOF");
	while((s.compare("EOF") != 0)){
		v = t.Readmsg();
		for(int i=0;i<v.size();i++){
			s = v[i];
			cout << s <<endl;
		}
	}
	*/
}
