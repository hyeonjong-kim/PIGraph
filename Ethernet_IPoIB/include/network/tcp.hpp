#ifndef TCP_H
#define TCP_H

#include <iostream>
#include <sys/socket.h>
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
#include <mutex>
#include <vector>

using namespace std;

class tcp{
    private:
        int server_socket, new_socket, client_sock = 0;
        struct sockaddr_in address;
        struct sockaddr_in serv_addr;
        int opt = 1;
        int addrlen = sizeof(address);
        char buffer[1048676] = {0};
        int port;
        int valread;
        char* server_addr;
        int num_host;
        socklen_t opt_len = sizeof(opt);
        int client_port;
        string result;
        string read_char;
        vector<string> send_msg;
        string alive_msg;
        int send_msg_size;
        mutex* mu;
        int internalBucket;
        string combine_send_msg = "";
        

    public:
        tcp(int socket_num, int port, char _server_addr[], int num_host, int _client_port, int num_mu);
        tcp();
        void SetInfo(int socket_num, int port, char _server_addr[], int num_host, int _client_port, int num_mu);
        ~tcp();
        void SetSocket();
        void ConnectSocket();
        string Readmsg();
        void Sendmsg(string& _msg, int vertex_id);
        string ReadCheckmsg();
        void SendCheckmsg();
        string ReadAliveMsg();
        void SendAliveMsg(string _msg);
        char* GetServerAddr(){return this->server_addr;}
        void CloseSocket();
        void ShutdownSocket();
        int internalHashFunction(int x){return (x % this->internalBucket);}
};

#endif




