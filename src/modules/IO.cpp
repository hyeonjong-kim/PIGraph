#include "../../include/modules/IO.h"

void IO::setHDFSInfo(const char* hostInfo, tPort port){
    this->hostInfo_HDFS =  hostInfo;
    this->port_HDFS = port;
}

string IO::readHDFSFile(const char* path, int flags, int bufferSize, short replication, tSize blocksize){
    this->fs_HDFS = hdfsConnect(this->hostInfo_HDFS, this->port_HDFS);
    this->file_HDFS = hdfsOpenFile(this->fs_HDFS, path, flags, bufferSize, replication, blocksize);
    if(!this->file_HDFS) {
        cerr << "[ERROR]Fail to open hdfs file: " << path << endl;
    }

    char* readBuffer = new char[1024];
    string result = "";
    size_t pos = 0;
    size_t readLength = 0;
    do{
        bzero(readBuffer, 1024);
        readLength = hdfsRead(this->fs_HDFS, this->file_HDFS, (void*)readBuffer, 1024-1);
        result += string(readBuffer);
    } while (readLength != 0);
    
    hdfsCloseFile(this->fs_HDFS, this->file_HDFS);
    hdfsDisconnect(this->fs_HDFS);
    
    return result;
}

bool IO::writeHDFSFile(const char* path, int flags, string result, int bufferSize, short replication, tSize blocksize){
    this->fs_HDFS = hdfsConnect(this->hostInfo_HDFS, this->port_HDFS);
    this->file_HDFS = hdfsOpenFile(this->fs_HDFS, path, flags, bufferSize, replication, blocksize);
     if(!this->file_HDFS) {
        cerr << "[ERROR]Fail to open hdfs file: " << path << endl;
    }
}