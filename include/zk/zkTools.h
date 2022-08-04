#ifndef ZKTOOLS_H
#define ZKTOOLS_H

#include <iostream>
#include <zookeeper/zookeeper.h>
#include <unistd.h>
#include <string.h>

using namespace std;

class zkTools{
    public:
        zhandle_t* zkInit(const char* hostInfo);
        bool zkCreatePersistent(zhandle_t *zh, char* path, char* data);
        bool zkCreateEphemeral(zhandle_t *zh, char* path, char* data);
        bool zkDelete(zhandle_t *zh, char* path);
        bool zkExists(zhandle_t *zh, char* path);
        bool zkWget(zhandle_t *zh, char* path, char* buffer);
        bool zkGet(zhandle_t *zh, char* path, char* buffer);
        bool zkClose(zhandle_t *zh);
        bool zkSet(zhandle_t *zh, char* path, char* data);
        bool zkGetChildern(zhandle_t *zh, char* path, String_vector* strings);
};

#endif