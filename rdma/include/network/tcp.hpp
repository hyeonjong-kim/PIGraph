#ifndef TCP_H
#define TCP_H

#include <iostream>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <thread>
#include <chrono>
#include <vector>
#include <sstream>
#include <map>
#include <linux/sockios.h>

using namespace std;

class tcp{
    private:
        int server_socket, new_socket, client_sock = 0;
        struct sockaddr_in address;
        struct sockaddr_in serv_addr;
        int opt = 1;
        int addrlen = sizeof(address);
        char buffer[104867600] = {0};
        int port;
        int valread;
        char* server_addr;
        int num_host;
        socklen_t opt_len = sizeof(opt);
        int client_port;
        string result;
        string read_char;
        string send_msg ="";

    public:
        tcp(int socket_num, int port, char _server_addr[], int num_host, int _client_port);
        tcp(){};
        void SetInfo(int socket_num, int port, char _server_addr[], int num_host, int _client_port);
        ~tcp();
        void SetSocket();
        void ConnectSocket();
        string Readmsg();
        void Sendmsg(string _msg);
        void SendCheckmsg();
        char* GetServerAddr(){return this->server_addr;}
        void CloseSocket();
        void ShutdownSocket();
        map<string, string> ReadRDMAInfo();
        void SendRDMAInfo(string _msg);
        string ReadCheckMsg();
        
};

#endif