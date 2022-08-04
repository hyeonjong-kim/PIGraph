#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../include/zk/zkTools.h"

int main(){
  zkTools zktools;
  zhandle_t* zh = zktools.zkInit("localhost.ib:2181");
  char* buffer = new char[512];
  char* previousBuffer = new char[512];
  zktools.zkWget(zh, "/PiGraph/Query/CPU/MN", buffer);
  strcpy(previousBuffer, buffer);
  
  while(true){
    if(buffer != "none"){
      string jobPath = "/PiGraph/job/"+string(buffer);
      cerr << jobPath << endl;
    }
    sleep(2);
  }
  
  return 0;
}