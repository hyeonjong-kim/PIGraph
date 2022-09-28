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
#include <mutex>
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
        int internalBucket;
        mutex* vertex_mu;

        map<int, int>* recv_pos;
        double* send_msg_buf;
        map<int, int> send_pos;

        int buffer_size;
        int send_buffer_size;
        
    public:
        IPoIB(int socket_num, int port, char _server_addr[], int num_host, int _client_port, int _buffer_size, map<int, int>* _recv_pos, int mu_num);
        IPoIB(){}
        ~IPoIB(){}
        
        void setInfo(int socket_num, int _port, char _server_addr[], int _num_host, int _client_port, int _buffer_size, map<int, int>* _recv_pos, int mu_num);
        
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

        void exchangeInfo();
        void combinerSum(int vertex_id, double value);
        void combinerMin(int vertex_id, double value);
        void combinerMax(int vertex_id, double value);

        int internalHashFunction(int x) {return (x % this->internalBucket);}
        double* getSendMsgBuf(){return this->send_msg_buf;}
        int getSendMsgBufSize(){return this->send_buffer_size;}
        void closeSocket();
        char* getServerAddr(){return this->server_addr;}
        vector<string> split(string& input, char delimiter);
        
};

vector<string> IPoIB::split(string& input, char delimiter) {
	vector<string> answer;
    stringstream ss(input);
    string temp;

    while (getline(ss, temp, delimiter)) {
        answer.push_back(temp);
    }

    return answer;
}

IPoIB::IPoIB(int socket_num, int port, char _server_addr[], int num_host, int _client_port, int _buffer_size, map<int, int>* _recv_pos, int mu_num){
    this->port = port + socket_num;
    this->server_addr = _server_addr;
    this->num_host = num_host;
    this->client_port = _client_port;
    this->buffer_size = _buffer_size;
    this->recv_pos = _recv_pos;
    this->internalBucket = mu_num;
    this->vertex_mu = new mutex[mu_num];
}

void IPoIB::setInfo(int socket_num, int _port, char* _server_addr, int _num_host, int _client_port, int _buffer_size, map<int, int>* _recv_pos, int mu_num){
    this->port = _port + socket_num;
    this->server_addr = _server_addr;
    this->num_host = _num_host;
    this->client_port = _client_port;
    this->buffer_size = _buffer_size;
    this->recv_pos = _recv_pos;
    this->internalBucket = mu_num;
    this->vertex_mu = new mutex[mu_num];
}

void IPoIB::setSocket(){
    if ((this->server_socket = socket(PF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    if (setsockopt(this->server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &(this->opt), sizeof(this->opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(this->port);

    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 1) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((this->client_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(this->client_port);
    if(inet_pton(AF_INET, this->server_addr, &serv_addr.sin_addr)<=0){
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(this->client_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &(this->opt), sizeof(this->opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
}

void IPoIB::connectSocket(){
    thread connect_server([](int& new_socket, int& server_fd, struct sockaddr_in& address, int& addrlen){
        if((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){
            perror("accept");
            exit(EXIT_FAILURE);
        }
        else{
        }
    }, ref(this->new_socket), ref(this->server_socket), ref(this->address), ref(this->addrlen));
    
    thread connect_client([](int& sock, struct sockaddr_in& serv_addr){
        while(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        }
    }, ref(this->client_sock), ref(this->serv_addr));

    connect_server.join();
    connect_client.join();
}

void IPoIB::sendMsg(string _msg){
    if(_msg.compare("Q")!=0){
        this->send_msg += _msg;
        
    }
    else{
        this->send_msg += _msg;
        char msg[this->send_msg.size()];
        strcpy(msg, send_msg.c_str());
        write(this->client_sock , msg , strlen(msg));
        this->send_msg="";
    }
}

string IPoIB::readMsg(){
    this->result = "";
    this->read_char= "";
    
    while(result.back() != 'Q'){
        bzero(this->buffer, sizeof(this->buffer));
        this->valread = read(this->new_socket , this->buffer, sizeof(this->buffer)-1);
        this->read_char = this->buffer;
        this->result += this->read_char;
    }

    this->result=this->result.substr(0, this->result.length()-1);  
    return this->result;
}

void IPoIB::sendRDMAInfo(string _msg){
    this->send_msg = _msg;
    char msg[send_msg.size()];
    strcpy(msg, send_msg.c_str());
    write(this->client_sock , msg , strlen(msg));
}

map<string, string> IPoIB::readRDMAInfo(){
    map<string, string> info;
    string info_name[5] = {"addr", "len", "rkey", "lid", "qp_num"};
    for(int i = 0; i < 5; i++){
        this->result="";
        this->read_char = "";
        while(result.back() != '\n'){
            bzero(this->buffer, sizeof(this->buffer));
            this->valread = read(this->new_socket , this->buffer, 1);
            this->read_char = this->buffer;
            if(this->read_char!=""){
                this->result += this->read_char;
            }
        }
        info.insert({info_name[i], this->result});
    }

    return info;
}

void IPoIB::sendCheckMsg(){
    string checkMsg = "1\n";
    char msg[checkMsg.size()];
    strcpy(msg, checkMsg.c_str());
    write(this->new_socket, msg , strlen(msg));
}

string IPoIB::readCheckMsg(){
    this->result="";
    char buf[2];
    
    while(this->result.back() != '\n'){
        bzero(buf, sizeof(buf));
        this->valread = read(this->client_sock , buf, sizeof(buf)-1);
        this->read_char = buf;

        if(this->read_char!=""){
            this->result += this->read_char;
        }
    }
    
    return this->result;
}

void IPoIB::sendAliveMsg(string _msg){
    if(_msg.compare("Q")!=0){
        this->send_msg += _msg;
    }
    else{
        this->send_msg += _msg;
        char msg[this->send_msg.size()];
        strcpy(msg, send_msg.c_str());
        write(this->new_socket , msg , strlen(msg));
        this->send_msg="";
    }
}

string IPoIB::readAliveMsg(){
    this->result = "";
    this->read_char = "";
    char buf[2];
    
    while(result.back() != 'Q'){
        bzero(buf, sizeof(buf));
        this->valread = read(this->client_sock , buf, sizeof(buf)-1);
        this->read_char = buf;
        this->result += this->read_char;
    }

    this->result=this->result.substr(0, this->result.length()-1);  
    return this->result;
}

void IPoIB::closeSocket(){
    close(this->client_sock);
    close(this->server_socket);
    close(this->new_socket);
}



void IPoIB::combinerSum(int vertex_id, double value){
    this->vertex_mu[this->internalHashFunction(vertex_id)].lock();
    int pos = this->send_pos.find(vertex_id)->second;
    this->send_msg_buf[pos] += value;
    this->vertex_mu[this->internalHashFunction(vertex_id)].unlock();
}
void IPoIB::combinerMin(int vertex_id, double value){
    this->vertex_mu[this->internalHashFunction(vertex_id)].lock();
    int pos = this->send_pos.find(vertex_id)->second;
    if(this->send_msg_buf[pos] > value){
        this->send_msg_buf[pos] = value;
    }
    this->vertex_mu[this->internalHashFunction(vertex_id)].unlock();
}
void IPoIB::combinerMax(int vertex_id, double value){
    this->vertex_mu[this->internalHashFunction(vertex_id)].lock();
    int pos = this->send_pos.find(vertex_id)->second;
    if(this->send_msg_buf[pos] < value){
        this->send_msg_buf[pos] = value;
    }
    this->vertex_mu[this->internalHashFunction(vertex_id)].unlock();
}

void IPoIB::exchangeInfo(){
    this->sendMsg(to_string(this->buffer_size) + "\n");
    this->sendMsg("Q");
    this->send_buffer_size = stoi(this->readMsg());
    this->send_msg_buf = new double[this->send_buffer_size];
    
    map<int,int>::iterator iter;
    for(iter=this->recv_pos->begin(); iter!=this->recv_pos->end(); iter++){
        this->sendMsg(to_string(iter->first) + " " + to_string(iter->second) + "\n");
    }
    this->sendMsg("Q");
    fill_n(this->send_msg_buf, this->send_buffer_size, 0.0);
    
    result = this->readMsg();
    vector<string> msg_split = split(result, '\n');
    vector<string> value_split;
    for(int k = 0; k < msg_split.size(); k++){
        value_split = split(msg_split[k], ' ');
        if(value_split.size() == 2 && this->send_pos.count(stoi(value_split[0]))==0){
            this->send_pos.insert(make_pair(stoi(value_split[0]), stoi(value_split[1])));
        }
    }
}


#endif