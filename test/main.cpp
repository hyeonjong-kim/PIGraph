#include <iostream>
#include "../include/io/HDFS.h"
using namespace std;

int main(){
  HDFS h;
  h.hdfsInit("MN.ib", 54310);
  h.hdfsOpen("/user/testfile.txt", O_WRONLY|O_CREAT);
  char* buffer = "fdsa";
  h.hdfsWriteFile(buffer);
  return 0;
}
