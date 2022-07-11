#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

class IPC{
    public:
        int createShm(key_t key);
        char* getShm(int shmId);
        string getData(int shmId);
        bool setData(int shmId, string data);
        bool detachShm(const void *shmaddr);
        bool deleteShm(int shmId);
        int initSem(key_t semKey);
        int semLock(int semId);
        int semUnlock(int semId);
};