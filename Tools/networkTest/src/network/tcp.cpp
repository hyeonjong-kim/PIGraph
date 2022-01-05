#include "tcp.hpp"

tcp::tcp(int socket_num, int port, char* _server_addr){
    this->port = port + socket_num;
    this->server_addr = _server_addr;
}

tcp::~tcp(){
    
}

void tcp::SetSocket(){
    if ((this->server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0){
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
    serv_addr.sin_port = htons(this->port);
    
    if(inet_pton(AF_INET, server_addr, &serv_addr.sin_addr)<=0){
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }
}

void tcp::ConnectSocket(){
    thread connect_server([](int& new_socket, int& server_fd, struct sockaddr_in& address, int& addrlen){
       if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){
            perror("accept");
         exit(EXIT_FAILURE);}
    }, ref(this->new_socket), ref(this->server_socket), ref(this->address), ref(this->addrlen));

    thread connect_client([](int& sock, struct sockaddr_in& serv_addr){
        while(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
           cout << "Wait connection" << endl;
        }
        cout << "Connection success" << endl;
    }, ref(this->client_sock), ref(this->serv_addr));

    connect_server.join();
    connect_client.join();
}

void tcp::Sendmsg(string _msg){
    char msg[1024];
    strcpy(msg, _msg.c_str());
    write(this->client_sock , msg , strlen(msg));
    
}

vector<string> tcp::Readmsg(){
    this->valread = read(this->new_socket , this->buffer, sizeof(char)*1024);
    string s(buffer);
    vector<string> v = split(s, ' ');
    return v;
}