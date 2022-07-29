#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../include/zk/zkTools.h"

int main(void){
  zkTools zktools;

  zhandle_t* zh = zktools.zkInit("localhost.ib:2181");
  double tmp = 213.134;
  zktools.zkDelete(zh, "/test");
  char* buffer = new char[128];
  strcpy(buffer, to_string(tmp).c_str());
  zktools.zkCreatePersistent(zh, "/test", "0.0");
  //cerr << strlen((char*)to_string(tmp).c_str()) << endl;
  cerr << buffer << endl;
  zktools.zkSet(zh, "/test", buffer);
  
  return 0;
}