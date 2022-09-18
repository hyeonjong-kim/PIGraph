#include "../include/modules/Parsing.h"
#include "../include/modules/Communication.h"

int main(int argc, const char *argv[]){
    Parsing* parsing = new Parsing();
    parsing->argParse(argc, argv);
    
    Communication* communication = new Communication("client");
    communication->client(parsing->getArgConfig());
    return 0;
}