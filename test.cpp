#include <iostream>
#include <netdb.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <time.h>
#include <vector>

using namespace std;

#define MST (-7)

int main(int argc, char *argv[])
{
	
    vector<string> s;
    s.push_back("MN");
    s.push_back("SN01");
    vector<char*> tmp(2);
	for (size_t i = 0; i < 2; i++)
    {
        hostent * record = gethostbyname(s[i].c_str());
        
        char* ip_address = inet_ntoa(* (in_addr * )record->h_addr);
        cerr << ip_address << endl;
        tmp[i] = ip_address;
        /*
        in_addr * address = (in_addr * )record->h_addr;
        char* ip_address = inet_ntoa(* address);
        cerr <<  ip_address << endl;
        
        */
    }
    cerr << tmp[0] << endl; 
    
	// log this information to ipaddr.log
	return 0;
}