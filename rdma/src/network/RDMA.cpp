#include "RDMA.hpp"

vector<string> RDMA::split(string& input, char delimiter) {
	vector<string> answer;
    stringstream ss(input);
    string temp;

    while (getline(ss, temp, delimiter)) {
        answer.push_back(temp);
    }

    return answer;
}

RDMA::RDMA(tcp* _t, double* _recv_msg, int _buffer_size, map<int, vector<int>> _recv_pos, mutex* _vertex_mu, int mu_num){
  this->t = _t;
  this->context = this->CreateContext();
  this->protection_domain = ibv_alloc_pd(this->context);
  this->cq_size = 64;
  this->channel = ibv_create_comp_channel(this->context);

  this->completion_queue = ibv_create_cq(this->context, this->cq_size, this->cq_context, this->channel, 0);
  this->completion_queue_recv = ibv_create_cq(this->context, this->cq_size, this->cq_context, this->channel, 0);

  this-> qp = this->CreateQueuePair(this->protection_domain, this->completion_queue, this->completion_queue_recv);
  this->lid = this->GetLocalId(this->context, PORT);
  this->qp_num = this->GetQueuePairNumber(this->qp);
  this->recv_msg = _recv_msg;
  this->buffer_size = _buffer_size;
  this->recv_mr = RegisterMemoryRegion(this->protection_domain, this->recv_msg, _buffer_size);
  this->recv_pos = _recv_pos;
  this->vertex_mu = _vertex_mu;
  this->internalBucket = mu_num;
}

RDMA::RDMA(){

}

void RDMA::setInfo(tcp* _t, double* _recv_msg, int _buffer_size, map<int, vector<int>> _recv_pos, mutex* _vertex_mu, int mu_num){
  this->t = _t;
  this->context = this->CreateContext();
  this->protection_domain = ibv_alloc_pd(this->context);
  this->cq_size = 64;
  this->channel = ibv_create_comp_channel(this->context);
  
  this->completion_queue = ibv_create_cq(this->context, this->cq_size, this->cq_context, this->channel, 0);
  this->completion_queue_recv = ibv_create_cq(this->context, this->cq_size, this->cq_context, this->channel, 0);

  this-> qp = this->CreateQueuePair(this->protection_domain, this->completion_queue, this->completion_queue_recv);
  this->lid = this->GetLocalId(this->context, PORT);
  this->qp_num = this->GetQueuePairNumber(this->qp);
  this->recv_msg = _recv_msg;
  this->buffer_size = _buffer_size;
  this->recv_mr = RegisterMemoryRegion(this->protection_domain, this->recv_msg, _buffer_size*sizeof(double));
  this->recv_pos = _recv_pos;
  this->vertex_mu = _vertex_mu;
  this->internalBucket = mu_num;
}

RDMA::~RDMA(){
  
}

template <typename T> 
T FromString (const std::string &Text)
{
    std::stringstream ss(Text);
    void * result;
    ss >> result;
    return (T)result;
}

struct ibv_context* RDMA::CreateContext() {
    
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
      context = ibv_open_device(device);
      assert(context);
      
  }
  return context;
}

struct ibv_qp* RDMA::CreateQueuePair(struct ibv_pd* pd, struct ibv_cq* cq, struct ibv_cq* cq_recv) {
  struct ibv_qp_init_attr queue_pair_init_attr;
  memset(&queue_pair_init_attr, 0, sizeof(queue_pair_init_attr));
  queue_pair_init_attr.qp_type = IBV_QPT_RC;
  queue_pair_init_attr.sq_sig_all = 1;       
  queue_pair_init_attr.send_cq = cq;         
  queue_pair_init_attr.recv_cq = cq_recv;         
  queue_pair_init_attr.cap.max_send_wr = 32;  
  queue_pair_init_attr.cap.max_recv_wr = 32;  
  queue_pair_init_attr.cap.max_send_sge = 1; 
  queue_pair_init_attr.cap.max_recv_sge = 1; 
  

  return ibv_create_qp(pd, &queue_pair_init_attr);
}

bool RDMA::ChangeQueuePairStateToInit(struct ibv_qp* queue_pair) {
  struct ibv_qp_attr init_attr;
  memset(&init_attr, 0, sizeof(init_attr));
  init_attr.qp_state = ibv_qp_state::IBV_QPS_INIT;
  init_attr.port_num = PORT;
  init_attr.pkey_index = 0;
  init_attr.qp_access_flags = IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ | IBV_ACCESS_REMOTE_WRITE;

  return ibv_modify_qp(queue_pair, &init_attr, IBV_QP_STATE | IBV_QP_PKEY_INDEX | IBV_QP_PORT | IBV_QP_ACCESS_FLAGS) == 0 ? true : false;
}

bool RDMA::ChangeQueuePairStateToRTR(struct ibv_qp* queue_pair, int ib_port, uint32_t destination_qp_number, uint16_t destination_local_id) {
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

uint16_t RDMA::GetLocalId(struct ibv_context* context, int ib_port) {
  ibv_port_attr port_attr;
  ibv_query_port(context, ib_port, &port_attr);
  return port_attr.lid;
}

uint32_t RDMA::GetQueuePairNumber(struct ibv_qp* qp) {
  return qp->qp_num;
}

bool RDMA::ChangeQueuePairStateToRTS(struct ibv_qp* queue_pair) {
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

struct ibv_mr* RDMA::RegisterMemoryRegion(struct ibv_pd* pd, void* buffer, size_t size) {
  return ibv_reg_mr(pd, buffer, size, IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ | IBV_ACCESS_REMOTE_WRITE);
}

void RDMA::ExchangeInfo(){
  std::ostringstream oss;
  oss << this->recv_msg;
  this->t->SendRDMAInfo(oss.str()+"\n");
  this->t->SendRDMAInfo(to_string(this->buffer_size)+"\n");
  this->t->SendRDMAInfo(to_string(this->recv_mr->rkey)+"\n");
  this->t->SendRDMAInfo(to_string(this->lid)+"\n");
  this->t->SendRDMAInfo(to_string(this->qp_num)+"\n");
  this->RDMAInfo = this->t->ReadRDMAInfo();
  
  map<int,vector<int>>::iterator iter;
  for(iter=this->recv_pos.begin(); iter!=this->recv_pos.end(); iter++){
    this->t->Sendmsg(to_string(iter->first) + " " + to_string(iter->second[0])+ " " + to_string(iter->second[1])+ "\n");
  }
  
  this->t->Sendmsg("Q");
  this->send_msg = new double[stoi(RDMAInfo.find("len")->second)];
  fill_n(this->recv_msg, this->buffer_size, numeric_limits<double>::max());
  fill_n(this->send_msg, stoi(RDMAInfo.find("len")->second), numeric_limits<double>::max());

  this->send_mr = RegisterMemoryRegion(this->protection_domain, this->send_msg , stoi(RDMAInfo.find("len")->second) * sizeof(double));

  string result = this->t->Readmsg();
  vector<string> msg_split = split(result, '\n');
  vector<string> value_split;
  for(int k = 0; k < msg_split.size(); k++){
    value_split = split(msg_split[k], ' ');
    if(value_split.size() == 3 && this->send_pos.count(stoi(value_split[0]))==0){
      vector<int> pos;
      pos.push_back(stoi(value_split[1]));
      pos.push_back(stoi(value_split[2]));
      this->send_pos.insert(make_pair(stoi(value_split[0]), pos));
      this->send_pos_cnt.insert(make_pair(stoi(value_split[0]), 0));
    }
  }
  
  this->vertex_num = this->send_pos.size();
  

}

void RDMA::ConnectRDMA(){
  this->ExchangeInfo();
  this->ChangeQueuePairStateToInit(this->qp);
  this->ChangeQueuePairStateToRTR(this->qp, PORT, stoi(this->RDMAInfo.find("qp_num")->second), stoi(this->RDMAInfo.find("lid")->second));
  this->ChangeQueuePairStateToRTS(this->qp);
  //cout << "Connect RDMA based on " << this->t->GetServerAddr() << endl;
}

void RDMA::PostRdmaWrite(struct ibv_qp *qp, struct ibv_mr *mr, void *addr, uint32_t length, string r_addr, string r_key){
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
      .opcode     = IBV_WR_RDMA_WRITE_WITH_IMM,
      .send_flags = 0,
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
}

bool RDMA::PollCompletion(struct ibv_cq* cq) {
  
  int num_wr = 2;
  struct ibv_wc wc;
  int ret;

  int solicited_only = 0;

  ibv_req_notify_cq(cq, solicited_only);
  ret = ibv_get_cq_event(channel, &cq, &(this->cq_context));
  ret = ibv_poll_cq(cq, 1, &wc);
  
  if (ret < 0) {
      fprintf(stderr, "Failure: ibv_poll_cq\n");
      return false;
  }
  
  if (wc.status != IBV_WC_SUCCESS) {
      fprintf(stderr, "Completion errror\n");
      return false;
  }
  ibv_req_notify_cq(cq, 0);
  ibv_ack_cq_events(cq, 1);

  /*
  ret = ibv_poll_cq(cq, 1, &wc);

  if (ret == 0)
      continue;

  if (ret < 0) {
      fprintf(stderr, "Failure: ibv_poll_cq\n");
      return false;
  }
  
  if (wc.status != IBV_WC_SUCCESS) {
      fprintf(stderr, "Completion errror\n");
      return false;
  }
  */
  
  
  /*
  while (num_wr > 0) {
      ret = ibv_poll_cq(cq, 1, &wc);

      if (ret == 0)
          continue; 

      if (ret < 0) {
          fprintf(stderr, "Failure: ibv_poll_cq\n");
          return false;
      }
      
      if (wc.status != IBV_WC_SUCCESS) {
          fprintf(stderr, "Completion errror\n");
          return false;
      }

      num_wr--;        
  }
  */
  return true;
}

void RDMA::SendMsg(string vertex_id, double value){
  if(vertex_id != "Q"){
    this->vertex_mu[this->internalHashFunction(stoi(vertex_id))].lock();
    int start_pos = this->send_pos.find(stoi(vertex_id))->second[0];
    int end_pos = this->send_pos.find(stoi(vertex_id))->second[1];
    int cnt = this->send_pos_cnt.find(stoi(vertex_id))->second;

    this->send_msg[start_pos + cnt] = value;
    if(end_pos > start_pos+cnt+1)this->send_msg[start_pos+cnt+1] = numeric_limits<double>::max();
    this->send_pos_cnt.find(stoi(vertex_id))->second++;
    this->vertex_mu[this->internalHashFunction(stoi(vertex_id))].unlock();
  }
  else{
    map<int, int>::iterator iter;
    this->PostRdmaWrite(this->qp, this->send_mr, this->send_msg, stoi(this->RDMAInfo.find("len")->second)* sizeof(double), this->RDMAInfo.find("addr")->second, this->RDMAInfo.find("rkey")->second);
    
    thread ReadRDMAmsg([this](){
      this->PostRdmaRead(this->qp, this->recv_mr, this->recv_msg, this->buffer_size);
      this->PollCompletion(this->completion_queue_recv);
    });
    
    this->PollCompletion(this->completion_queue);
    ReadRDMAmsg.join();

    for(iter=this->send_pos_cnt.begin();iter!=this->send_pos_cnt.end();iter++){
      if(iter->second != 0){
        this->t->Sendmsg(to_string(iter->first) + "\n");
        iter->second = 0;
      }
    }
  }
}

bool RDMA::ReadWakeVertex(){
  this->t->Sendmsg("Q");
  this->wake_vertex = this->t->Readmsg();
}

void RDMA::CloseRDMA(){
  ibv_destroy_qp(this->qp);
  ibv_destroy_cq(this->completion_queue);
  ibv_dereg_mr(this->send_mr);
  ibv_dereg_mr(this->recv_mr);
  ibv_dealloc_pd(this->protection_domain);
};

void RDMA::PostRdmaRead(struct ibv_qp *qp, struct ibv_mr *mr, void *addr, uint32_t length){
   struct ibv_sge sge = {
        .addr = (uint64_t)(uintptr_t)addr,
        .length = length,
        .lkey = mr->lkey
    };

    struct ibv_recv_wr recv_wr = {
        .wr_id = (uint64_t)(uintptr_t)sge.addr,
        .next = NULL,
        .sg_list = &sge,
        .num_sge = 1,
    };

    struct ibv_recv_wr *bad_wr;

    ibv_post_recv(qp, &recv_wr, &bad_wr);
}