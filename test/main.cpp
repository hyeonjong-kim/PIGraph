#include "../include/zk/zkTools.h"
#include <unistd.h>
#include <map>
#include <string>

int main(){
    
    zkTools zk;
    zhandle_t* zh = zk.zkInit("localhost:2181");
    char* buffer = new char[5];
    
    zk.zkWget(zh, "/PiGraph/Query/CPU/SN01", buffer);
    map<string, char*> resourceBuffer_GPU;
    resourceBuffer_GPU.insert({"asdf", buffer});
    
    while(true){
        cerr << resourceBuffer_GPU.find("asdf")->second << endl;
        sleep(3);
    }
    
    zk.zkClose(zh);
    
   /*
    char* buffer = new char;
    map<string, char*> resourceBuffer_GPU;
    resourceBuffer_GPU.insert({"asdf", buffer});
    cerr << resourceBuffer_GPU.find("asdf")->second << endl;
    cerr << &buffer << endl;
    */
    return 0;
}