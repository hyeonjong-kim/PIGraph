#include <infiniband/verbs.h>
#include <iostream>
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
#include<cstdio>

#include "ThreadPool.hpp"
#include "tcp.hpp"
#define PORT 1
using namespace std;


struct ibv_context* createContext() {
    
    int ret;

    ret = ibv_fork_init();
    if (ret) {
        fprintf(stderr, "Failure: ibv_fork_init (errno=%d)\n", ret);
        exit(EXIT_FAILURE);
    }

    struct ibv_device **dev_list;
    dev_list = ibv_get_device_list(NULL);

    if (!dev_list) {
        int errsave = errno;
        fprintf(stderr, "Failure: ibv_get_device_list (errno=%d)\n", errsave);
        exit(EXIT_FAILURE);        
    }

    struct ibv_context *context;

    if (dev_list[0]) {
        struct ibv_device *device = dev_list[0];
        printf("IB device: %s\n", ibv_get_device_name(device));
        context = ibv_open_device(device);
        assert(context);
        
    }
  return context;
}

struct ibv_qp* createQueuePair(struct ibv_pd* pd, struct ibv_cq* cq) {
  struct ibv_qp_init_attr queue_pair_init_attr;
  memset(&queue_pair_init_attr, 0, sizeof(queue_pair_init_attr));
  queue_pair_init_attr.qp_type = IBV_QPT_RC;
  queue_pair_init_attr.sq_sig_all = 1;       
  queue_pair_init_attr.send_cq = cq;         
  queue_pair_init_attr.recv_cq = cq;         
  queue_pair_init_attr.cap.max_send_wr = 1;  
  queue_pair_init_attr.cap.max_recv_wr = 1;  
  queue_pair_init_attr.cap.max_send_sge = 1; 
  queue_pair_init_attr.cap.max_recv_sge = 1; 

  return ibv_create_qp(pd, &queue_pair_init_attr);
}

bool changeQueuePairStateToInit(struct ibv_qp* queue_pair) {
  struct ibv_qp_attr init_attr;
  memset(&init_attr, 0, sizeof(init_attr));
  init_attr.qp_state = ibv_qp_state::IBV_QPS_INIT;
  init_attr.port_num = PORT;
  init_attr.pkey_index = 0;
  init_attr.qp_access_flags = IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ | IBV_ACCESS_REMOTE_WRITE;

  return ibv_modify_qp(queue_pair, &init_attr, IBV_QP_STATE | IBV_QP_PKEY_INDEX | IBV_QP_PORT | IBV_QP_ACCESS_FLAGS) == 0 ? true : false;
}

bool changeQueuePairStateToRTR(struct ibv_qp* queue_pair, int ib_port, uint32_t destination_qp_number, uint16_t destination_local_id) {
  struct ibv_qp_attr rtr_attr;
  memset(&rtr_attr, 0, sizeof(rtr_attr));
  rtr_attr.qp_state = ibv_qp_state::IBV_QPS_RTR;
  rtr_attr.path_mtu = ibv_mtu::IBV_MTU_1024;
  rtr_attr.rq_psn = 0;
  rtr_attr.max_dest_rd_atomic = 1;
  rtr_attr.min_rnr_timer = 0x12;
  rtr_attr.ah_attr.is_global = 0;
  rtr_attr.ah_attr.sl = 0;
  rtr_attr.ah_attr.src_path_bits = 0;
  rtr_attr.ah_attr.port_num = ib_port;
  
  rtr_attr.dest_qp_num = destination_qp_number;
  rtr_attr.ah_attr.dlid = destination_local_id;

  return ibv_modify_qp(queue_pair, &rtr_attr, IBV_QP_STATE | IBV_QP_AV | IBV_QP_PATH_MTU | IBV_QP_DEST_QPN | IBV_QP_RQ_PSN | IBV_QP_MAX_DEST_RD_ATOMIC | IBV_QP_MIN_RNR_TIMER) == 0 ? true : false;
}

uint16_t getLocalId(struct ibv_context* context, int ib_port) {
  ibv_port_attr port_attr;
  ibv_query_port(context, ib_port, &port_attr);
  return port_attr.lid;
}

uint32_t getQueuePairNumber(struct ibv_qp* qp) {
  return qp->qp_num;
}

bool changeQueuePairStateToRTS(struct ibv_qp* queue_pair) {
  struct ibv_qp_attr rts_attr;
  memset(&rts_attr, 0, sizeof(rts_attr));
  rts_attr.qp_state = ibv_qp_state::IBV_QPS_RTS;
  rts_attr.timeout = 0x12;
  rts_attr.retry_cnt = 7;
  rts_attr.rnr_retry = 7;
  rts_attr.sq_psn = 0;
  rts_attr.max_rd_atomic = 1;

  return ibv_modify_qp(queue_pair, &rts_attr, IBV_QP_STATE | IBV_QP_TIMEOUT | IBV_QP_RETRY_CNT | IBV_QP_RNR_RETRY | IBV_QP_SQ_PSN | IBV_QP_MAX_QP_RD_ATOMIC) == 0 ? true : false;
}

struct ibv_mr* registerMemoryRegion(struct ibv_pd* pd, void* buffer, size_t size) {
  return ibv_reg_mr(pd, buffer, size, IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ | IBV_ACCESS_REMOTE_WRITE);
}

std::string HostToIp(const std::string& host) {
    hostent* hostname = gethostbyname(host.c_str());
    if(hostname)
        return std::string(inet_ntoa(**(in_addr**)hostname->h_addr_list));
    return {};
}

static void post_recv(struct ibv_qp *qp, struct ibv_mr *mr, void *addr, uint32_t length)
{
    int ret;

    struct ibv_sge sge = {
        .addr   = (uint64_t)(uintptr_t)addr,
        .length = length,
        .lkey   = mr->lkey,
    };

    struct ibv_recv_wr recv_wr = {
        .wr_id   = (uint64_t)(uintptr_t)addr,
        .sg_list = &sge,
        .num_sge = 1,
    };

    struct ibv_recv_wr *bad_wr;
    ret = ibv_post_recv(qp, &recv_wr, &bad_wr);
    assert(ret == 0);

    printf("post recv wr\n");
}

static void post_send(struct ibv_qp *qp, struct ibv_mr *mr, void *addr, uint32_t length)
{
    int ret;
    struct ibv_sge sge = {
        .addr   = (uint64_t)(uintptr_t)addr,
        .length = length,
        .lkey   = mr->lkey,
    };

    struct ibv_send_wr send_wr = {
        .wr_id      = (uint64_t)(uintptr_t)addr,
        .sg_list    = &sge,
        .num_sge    = 1,
        .opcode     = IBV_WR_SEND_WITH_IMM,
        .imm_data   = rand(),
    };

    struct ibv_send_wr *bad_wr;
    ret = ibv_post_send(qp, &send_wr, &bad_wr);
    assert(ret == 0);

    printf("post send wr: imm_data=0x%08x, byte_len=%u\n", send_wr.imm_data, length);
}


template <typename T> 
T FromString ( const std::string &Text)
{
    std::stringstream ss(Text);
    void * result;
    ss >> result;
    return (T)result;
}

static void post_rdma_write(struct ibv_qp *qp, struct ibv_mr *mr, void *addr, uint32_t length, string r_addr, string r_key)
{
  int ret;
  r_addr.erase(r_addr.find_last_not_of(" \n\r\t")+1);

  struct ibv_sge sge = {
      .addr   = (uint64_t)(uintptr_t)addr,
      .length = length,
      .lkey   = mr->lkey,
  };

  struct ibv_send_wr send_wr = {
      .wr_id      = (uint64_t)(uintptr_t)addr,
      .sg_list    = &sge,
      .num_sge    = 1,
      .opcode     = IBV_WR_RDMA_WRITE,
      .imm_data   = rand(),
      .wr = {
        .rdma = {
          .remote_addr = FromString<uint64_t>(r_addr),
          .rkey = stoul(r_key),
        }
      }
  };

  struct ibv_send_wr *bad_wr;
  ret = ibv_post_send(qp, &send_wr, &bad_wr);
  assert(ret == 0);

  printf("post rdma write wr: imm_data=0x%08x, byte_len=%u\n", send_wr.imm_data, length);
}

static void post_rdma_read(struct ibv_qp *qp, struct ibv_mr *mr, void *addr, uint32_t length, string r_addr, string r_key)
{
  int ret;
  r_addr.erase(r_addr.find_last_not_of(" \n\r\t")+1);

  struct ibv_sge sge = {
      .addr   = (uint64_t)(uintptr_t)addr,
      .length = length,
      .lkey   = mr->lkey,
  };

  struct ibv_send_wr send_wr = {
      .wr_id      = (uint64_t)(uintptr_t)addr,
      .sg_list    = &sge,
      .num_sge    = 1,
      .opcode     = IBV_WR_RDMA_READ,
      .imm_data   = rand(),
      .wr = {
        .rdma = {
          .remote_addr = FromString<uint64_t>(r_addr),
          .rkey = stoul(r_key),
        }
      }
  };

  struct ibv_send_wr *bad_wr;
  ret = ibv_post_send(qp, &send_wr, &bad_wr);
  assert(ret == 0);

  printf("post rdma write wr: imm_data=0x%08x, byte_len=%u\n", send_wr.imm_data, length);
}

int main(){
  int num_host = 2;
  string host_file = "/home/hjkim/rdma/hostfile/hostinfo.txt";
  ThreadPool* connectionThread = new ThreadPool(num_host);
  tcp t[num_host];
  vector<char[15]> server_ip(num_host);
	vector<string> v;
	ifstream hostfile(host_file);
	char hostname[256];
	size_t hostnamelen = 256;
	gethostname(hostname, hostnamelen);
	string tmp(hostname);
	int hostnum = 0;
  char buf[100];
  string s;

	for(int i=0; i<num_host; i++){
		hostfile.getline(buf, 100);
		s = buf;
		if(tmp.compare(s) == 0){
			break;
		}
		hostnum++;
	}
	hostfile.close();
  
  hostfile.open(host_file);
  
  for(int i=0; i<num_host; i++){
    hostfile.getline(buf, 100);
    s = buf;
    s = HostToIp(s);
    strcpy(server_ip[i], s.c_str());
    t[i].SetInfo(i, 3141592, server_ip[i], num_host, 3141592+hostnum, 1024);
    t[i].SetSocket();
    connectionThread->EnqueueJob([&t, i](){t[i].ConnectSocket();});
  }
  
  while(true){
			if(connectionThread->getJobs().empty()){
				while(true){
					if(connectionThread->checkAllThread())break;
				}
				break;
			}
	}
  
  

  //Create user context
  struct ibv_context* context = createContext();
  //Create protection domain
  struct ibv_pd* protection_domain = ibv_alloc_pd(context);
  //Create completion queue
  int cq_size = 0x10;
  struct ibv_cq* completion_queue = ibv_create_cq(context, cq_size, nullptr, nullptr, 0);
  //Create queue pair
  struct ibv_qp* qp = createQueuePair(protection_domain, completion_queue);
  //Create memory region
  char buffer[1024 * 1024];
  struct ibv_mr *mr = registerMemoryRegion(protection_domain, buffer, sizeof(buffer));
  
  //Exchange queue pair info
  uint16_t lid = getLocalId(context, PORT);
  uint32_t qp_num = getQueuePairNumber(qp);
  
  //Send RDMA info
  std::ostringstream oss;
  oss << &buffer;
  t[1].SendRDMAInfo(oss.str()+"\n");
  t[1].SendRDMAInfo(to_string(mr->length)+"\n");
  t[1].SendRDMAInfo(to_string(mr->lkey)+"\n");
  t[1].SendRDMAInfo(to_string(mr->rkey)+"\n");
  t[1].SendRDMAInfo(to_string(lid)+"\n");
  t[1].SendRDMAInfo(to_string(qp_num)+"\n");

  //Read RDMA info
  map<string, string> rdmaInfo = t[1].ReadRDMAInfo();

  //Exchange queue pair state
  changeQueuePairStateToInit(qp);
  changeQueuePairStateToRTR(qp, PORT, stoi(rdmaInfo.find("qp_num")->second), stoi(rdmaInfo.find("lid")->second));
  changeQueuePairStateToRTS(qp);

  for (int i=0 ; i<1000 ; i++)buffer[i] = 'f';
  post_rdma_write(qp, mr, buffer, 1000, rdmaInfo.find("addr")->second, rdmaInfo.find("rkey")->second);
  
  int num_wr = 1;
  struct ibv_wc wc;
  int ret;

  while (num_wr > 0) {
    ret = ibv_poll_cq(completion_queue, 1, &wc);

    if (ret == 0)continue; /* polling */

    if (ret < 0) {
      fprintf(stderr, "Failure: ibv_poll_cq\n");
      exit(EXIT_FAILURE);
    }

    if (wc.status != IBV_WC_SUCCESS) {
      fprintf(stderr, "Completion errror\n");
      exit(EXIT_FAILURE);
    }

    switch (wc.opcode) {
      case IBV_WC_SEND:
        printf("poll send wc: wr_id=0x%016" PRIx64 "\n", wc.wr_id);
        break;

      case IBV_WC_RECV:
        printf("poll recv wc: wr_id=0x%016" PRIx64 " byte_len=%u, imm_data=0x%x\n", wc.wr_id, wc.byte_len, wc.imm_data);
        ret = memcmp(buffer + 4096, (char*)(uintptr_t)wc.wr_id, wc.byte_len);
        assert(ret == 0);
        break;

      default:
        exit(EXIT_FAILURE);
    }

    num_wr--;        
  }
  
  ibv_destroy_qp(qp);
  ibv_destroy_cq(completion_queue);
  ibv_dereg_mr(mr);
  ibv_dealloc_pd(protection_domain);

  return 0;
}
