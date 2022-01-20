#ifndef RDMA_H
#define RDMA_H

#include <iostream>
#include <infiniband/verbs.h>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <netdb.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <assert.h>
#include <inttypes.h>
#include <typeinfo>
#include <cstdio>
#include <map>

#include "tcp.hpp"

#define PORT 1

using namespace std;

class RDMA {
    private:
        tcp* t = new tcp();
        map<string, string> RDMAInfo;
        struct ibv_context* context;
        struct ibv_pd* protection_domain;
        int cq_size;
        struct ibv_cq* completion_queue;
        struct ibv_qp* qp;
        struct ibv_mr *mr;
        struct ibv_mr *send_mr;
        uint16_t lid;
        uint32_t qp_num;
        string bulk_msg = "";
        char send_msg[10485760];
        char recv_msg[10485760];
    
    public:
        RDMA(tcp* _t);
        RDMA();
        ~RDMA();
        
        void setInfo(tcp* _t);
        
        struct ibv_context* CreateContext();
        struct ibv_qp* CreateQueuePair(struct ibv_pd* pd, struct ibv_cq* cq);
        struct ibv_mr* RegisterMemoryRegion(struct ibv_pd* pd, void* buffer, size_t size);

        bool ChangeQueuePairStateToInit(struct ibv_qp* queue_pair);
        bool ChangeQueuePairStateToRTR(struct ibv_qp* queue_pair, int ib_port, uint32_t destination_qp_number, uint16_t destination_local_id);
        bool ChangeQueuePairStateToRTS(struct ibv_qp* queue_pair);

        uint16_t GetLocalId(struct ibv_context* context, int ib_port);
        uint32_t GetQueuePairNumber(struct ibv_qp* qp);

        void ConnectRDMA();
        void ExchangeInfo();
        
        void PostRdmaWrite(struct ibv_qp *qp, struct ibv_mr *mr, void *addr, uint32_t length, string r_addr, string r_key);
        void SendMsg(string _msg);

        char* GetRecvMsg(){return this->send_msg;}
        string GetBulkMsg(){return this->bulk_msg;}
        char* ReadMsg();

        void ClearRecvMsg();
        
        bool PollCompletion(struct ibv_cq* cq);
};

#endif