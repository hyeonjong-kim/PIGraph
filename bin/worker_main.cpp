#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "../include/modules/Setting.h"
#include "../include/modules/IO.h"
#include "../include/modules/Graph.h"
#include "../include/modules/Network.h"
#include "../include/modules/Processing.h"

using namespace std;

struct mesg_buffer {
    long mesg_type;
    char mesg_text[1024];
} message;

int main(int argc, const char *argv[]){
    Setting* setting = new Setting();
    Graph* graph = new Graph();
    Network* network = new Network();
    Processing* processing = new Processing();
    

    setting->argParse(argc, argv);
    IO* io = new IO;
    io->setHDFSInfo((char*)(setting->HDFS_host.c_str()), setting->HDFS_port);
    string readFile = io->readHDFSFile((char*)(setting->filePath.c_str()), O_RDONLY);
    graph->setInfo(0, 1, 0);
    graph->createGraph(readFile);
    network->setNetwork(setting->networkType, 1, setting->workers, setting->port, graph->getRecvPos(), setting->mu, setting->numMutex);
    processing->setInfo(graph, network, setting->superstep);
    processing->execute();
    cerr << "working!!!!!" << endl;
    /*
    string readFile;
    int fds[2];
    pipe(fds);
    pid_t c_pid = fork();
    if (c_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (c_pid > 0) {    
        //parent
        char buffer[1024];
        key_t key;
        int msgid;
        key = ftok("progfile", 65);
        msgid = msgget(key, 0666 | IPC_CREAT);
        readFile = "";
        while (readFile.back() != 'Q')
        {
            msgrcv(msgid, &message, sizeof(message), 1, 0);
            readFile += string(message.mesg_text);
        }

        wait(nullptr);
    } else {
        //child
        IO* io = new IO;
        io->setHDFSInfo((char*)(setting->HDFS_host.c_str()), setting->HDFS_port);
        readFile = io->readHDFSFile((char*)(setting->filePath.c_str()), O_RDONLY);
        readFile += "Q";
        
        key_t key;
        int msgid;
        key = ftok("progfile", 65);
        msgid = msgget(key, 0666 | IPC_CREAT);
        message.mesg_type = 1;
        for (size_t i = 0; i < readFile.size(); i+=sizeof(message.mesg_text)){
            bzero(message.mesg_text, sizeof(message.mesg_text));
            strcpy(message.mesg_text, (readFile.substr(i, sizeof(message.mesg_text))).c_str());
            msgsnd(msgid, &message, sizeof(message), 0);
        }
        cerr << "[INFO]Sucess to read HDFS file on " << getpid() << endl;
        exit(EXIT_SUCCESS);
    }
    readFile.pop_back();
    //cerr << readFile << endl;
    */
    
    return 0;
}