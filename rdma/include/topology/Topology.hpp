#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <cstdlib>
#include <netdb.h>
#include <sys/types.h>
#include <sys/sysinfo.h>

#include "PageRank.hpp"
#include "ThreadPool.hpp"
#include "Parser.hpp"
#include "tcp.hpp"
#include "RDMA.hpp"
#include "ResourceChecker.hpp"



class Topology{
    Topology(int argc, const char *argv[],);

};

#endif