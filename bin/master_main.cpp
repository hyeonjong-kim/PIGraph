#include "../../include/modules/Communication.h"

int main(int argc, const char *argv[]){
    Communication *communication = new Communication();
    map<string,string> m = communication->master();
    map<string,string>::iterator iter;
    for(iter=m.begin(); iter != m.end(); iter++)cerr << iter->first << " " << iter->second << endl;
    return 0;
}