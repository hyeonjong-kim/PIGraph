#include "../../include/io/HDFS.h"

bool HDFS::hdfsInit(const char* hostInfo, tPort port){
    this->fs = hdfsConnect(hostInfo, port);
    cerr << "flag1" << endl;
}

bool HDFS::hdfsOpen(const char* path, int flags, int bufferSize, short replication, tSize blocksize){
    this->file = hdfsOpenFile(this->fs, path, flags, bufferSize, replication, blocksize);
    if(!file) {
        cerr << "[ERROR]Fail to open hdfs file: " << path << endl;
        return false;
    }
    return true;
}

bool HDFS::hdfsWriteFile(char* buffer){
    tSize num_written_bytes = hdfsWrite(this->fs, this->file, (void*)buffer, strlen(buffer)+1);
    /*
    if(hdfsWrite(fs, file, (void*)buffer, strlen(buffer)+1) == -1){
        cerr << "[ERROR]Fail to write hdfs file" << endl;
        return false;
    }
    */
    if (hdfsFlush(this->fs, this->file)) {
        cerr << "[ERROR]Fail to flush data" << endl;
        return false;
    }
    cerr << "[INFO]Success to write hdfs file" << endl;
    return true;
}

string& HDFS::hdfsReadFile(){
    char* readBuffer = new char[512];
    string result = "";
    size_t pos = 0;
    size_t readLength = 0;
    do{
        readLength = hdfsPread(this->fs, this->file, pos, (void*)readBuffer, 512);
        pos += strlen(readBuffer) + 1;
        if(readLength !=0)result += string(readBuffer);
    } while (readLength != 0);

    return result;
}