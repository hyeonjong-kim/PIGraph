#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

using namespace std;

class IPC{
    public:
        int createShm(key_t key, size_t size, int shmflg);
        string getData(int shmId);
        bool setData(int shmId, string data);
        bool detachShm(const void *shmaddr);
        bool deleteShm(int shmId);
};