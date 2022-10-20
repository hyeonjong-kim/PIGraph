#include "IPC.h"

int IPC::createShm(key_t key){
    int shmId = shmget(key, 1024, 0666|IPC_CREAT);
    return shmId;
}

bool IPC::setData(int shmId, string data){
    char *shm = (char*) shmat(shmId,(void*)0,0);
    
    if(shm == (char*) -1){
        perror("shmat fild");
        return false;
    }
    sprintf(shm, "%s", (char*) data.c_str());
    this->detachShm(shm);
    return true;
}

string IPC::getData(int shmId){
    char *shm = (char*) shmat(shmId,(void*)0,0);
    if(shm == (char*) -1){
        perror("shmat fild");
    }

    string msg = string(shm);
    this->detachShm(shm);
    return msg;
}

bool IPC::deleteShm(int shmId){
    if(shmctl(shmId, IPC_RMID, NULL) == -1){
        return false;
    }
    
    return true;
}

bool IPC::detachShm(const void *shmaddr){
    if(shmdt(shmaddr) == -1){
        return false;
    }
    
    return true;
}

char* IPC::getShm(int shmId){
    return (char*) shmat(shmId,(void*)0,0);
}