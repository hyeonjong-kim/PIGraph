#ifndef IPOIB_H
#define IPOIB_H

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

class IPoIB{
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
        IPoIB(int socket_num, int port, char _server_addr[], int num_host, int _client_port);
        IPoIB(){}
        ~IPoIB(){}
        
        
        
        void setInfo(int socket_num, int _port, char _server_addr[], int _num_host, int _client_port);
        
        void setSocket();
        void connectSocket();
        
        void sendMsg(string _msg);
        string readMsg();
        void sendRDMAInfo(string _msg);
        map<string, string> readRDMAInfo();
        void sendCheckMsg();
        string readCheckMsg();
        void sendAliveMsg(string _msg);
        string readAliveMsg();

        void closeSocket();
        char* getServerAddr(){return this->server_addr;}
        
};

#endif