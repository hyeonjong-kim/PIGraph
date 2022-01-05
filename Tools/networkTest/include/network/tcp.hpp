#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <chrono>
#include <vector>
#include <sstream>

using namespace std;

class tcp{
    private:
        int server_socket, new_socket, client_sock = 0;
        struct sockaddr_in address;
        struct sockaddr_in serv_addr;
        int opt = 1;
        int addrlen = sizeof(address);
        char buffer[1024] = {0};
        int port;
        int valread;
        char* server_addr;

    public:
        tcp(int socket_num, int port, char _server_addr[15]);
        ~tcp();
        void SetSocket();
        void ConnectSocket();
        vector<string> Readmsg();
        void Sendmsg(string _msg);
        
        vector<string> split(string input, char delimiter) {
            vector<string> answer;
            stringstream ss(input);
            string temp;

            while (getline(ss, temp, delimiter)) {
                answer.push_back(temp);
            }
            return answer;
        }
};