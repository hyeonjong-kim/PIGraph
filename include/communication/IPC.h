#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

using namespace std;

class IPC{
    public:
        int createShm(key_t key);
        char* getShm(int shmId);
        string getData(int shmId);
        bool setData(int shmId, string data);
        bool detachShm(const void *shmaddr);
        bool deleteShm(int shmId);
};