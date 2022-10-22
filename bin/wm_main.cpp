#include <signal.h>

#include "../include/modules/Resourse.h"
#include "../include/modules/Execution.h"
#include "../include/utils/ThreadPool.h"


Resource* resource = new Resource();
Execution* execution = new Execution();
ThreadPool::ThreadPool* threadPool =  new ThreadPool::ThreadPool(3);

void interruptHandler(int sig){
    delete execution;
    delete resource;
    
    cerr << "[INFO]WORK MANAGER STOP" << endl;
    exit(0);
}

int main(){
    signal(SIGINT, interruptHandler);
    signal(SIGKILL, interruptHandler);
    signal(SIGSTOP, interruptHandler);
    signal(SIGTERM, interruptHandler);
    
    std::vector<std::future<void>> futures;
    auto f1 = [](){resource->resourceChecker();};
    auto f2 = [](){execution->executeWorker();};
    futures.emplace_back(threadPool->EnqueueJob(f1));
    futures.emplace_back(threadPool->EnqueueJob(f2));
    for (auto& f_ : futures){
    	f_.wait();
  	}
    
    return 0;
}