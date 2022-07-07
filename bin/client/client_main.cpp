#include "../../include/modules/Parsing.h"
#include "../../include/modules/Communication.h"

int main(int argc, const char *argv[]){
    Parsing* parsing = new Parsing();
    Communication communication;
    
    parsing->run(argc, argv);
    map<string,string>& m = parsing->getConfig();
    communication.client(m);
}