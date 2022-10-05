#include <iostream>
#include <string.h>
#include "../../hadoop-2.10.1/include/hdfs.h"

using namespace std;

class IO{
    private:
        const char* hostInfo_HDFS;
        tPort port_HDFS;
        hdfsFS fs_HDFS;
        hdfsFile file_HDFS;

    public:
        IO(){}
        ~IO(){}

        void setHDFSInfo(const char* hostInfo, tPort port);
        string readHDFSFile(const char* path, int flags, int bufferSize=0, short replication=0, tSize blocksize=0);
        bool writeHDFSFile(const char* path, int flags, string result, int bufferSize=0, short replication=0, tSize blocksize=0);
};