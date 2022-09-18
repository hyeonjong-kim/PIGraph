#include <iostream>
#include <map>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>

using namespace std;

struct mesg_buffer {
    long mesg_type;
    char mesg_text[100];
} message;

int main(){
    string readFile;
    pid_t c_pid = fork();
    
    if (c_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (c_pid > 0) {    
        key_t key;
        int msgid;
        key = ftok("progfile", 65);
        msgid = msgget(key, 0666 | IPC_CREAT);
        msgrcv(msgid, &message, sizeof(message), 1, 0);
        printf("Data Received is : %s \n", message.mesg_text);
        msgrcv(msgid, &message, sizeof(message), 1, 0);
        printf("Data Received is : %s \n", message.mesg_text);
        msgctl(msgid, IPC_RMID, NULL);

        wait(nullptr);
    } else {
        //child
        readFile = "24123123";
        key_t key;
        int msgid;
    
        // ftok to generate unique key
        key = ftok("progfile", 65);
        msgid = msgget(key, 0666 | IPC_CREAT);
        message.mesg_type = 1;
        strcpy(message.mesg_text, readFile.c_str());
        cerr << message.mesg_text[0] << endl;
        //message.mesg_text = readFile.c_str();
        msgsnd(msgid, &message, sizeof(message), 0);
        printf("Data send is : %s \n", message.mesg_text);
        
        readFile = "12345678944139403824123123";
        // ftok to generate unique key
        key = ftok("progfile", 65);
        msgid = msgget(key, 0666 | IPC_CREAT);
        message.mesg_type = 1;
        strcpy(message.mesg_text, readFile.c_str());
        cerr << message.mesg_text[0] << endl;
        //message.mesg_text = readFile.c_str();
        msgsnd(msgid, &message, sizeof(message), 0);

        msgsnd(msgid, &message, sizeof(message), 0);
        // display the message
        printf("Data send is : %s \n", message.mesg_text);
        
    }
}