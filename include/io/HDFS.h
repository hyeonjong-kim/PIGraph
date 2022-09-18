#include <iostream>
#include "../../hadoop-2.10.1/include/hdfs.h"
#include <string.h>

using namespace std;

class HDFS{
    private:
        hdfsFS fs;
        hdfsFile file;
    public:
        
        bool hdfsInit(const char* hostInfo, tPort port);
        bool hdfsWriteFile(char* buffer);
        bool hdfsOpen(const char* path, int flags, int bufferSize=0, short replication=0, tSize blocksize=0);
        string& hdfsReadFile();

};
