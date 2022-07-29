#include "../include/modules/Resourse.h"

int main(){
    Resource* resource = new Resource();
    resource->resourceChecker();
    delete resource;
    return 0;
}