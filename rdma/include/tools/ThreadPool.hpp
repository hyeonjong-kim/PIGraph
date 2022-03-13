#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <iostream>

using namespace std;

class ThreadPool {
public:
    ThreadPool(size_t num_threads);
    ~ThreadPool();

    // job 을 추가한다.
    void EnqueueJob(std::function<void()> job);
    vector<std::thread>& getWokerThreads(){return this->worker_threads_;}
    queue<std::function<void()>>& getJobs(){return this->jobs_;}
    bool checkAllThread();

private:
    // 총 Worker 쓰레드의 개수.
    size_t num_threads_;
    // Worker 쓰레드를 보관하는 벡터.
    std::vector<std::thread> worker_threads_;
    // 할일들을 보관하는 job 큐.
    std::queue<std::function<void()>> jobs_;
    // 위의 job 큐를 위한 cv 와 m.
    std::condition_variable cv_job_q_;
    std::mutex m_job_q_;
    
    int* threadState;

    // 모든 쓰레드 종료
    bool stop_all;
    
    // Worker 쓰레드
    void WorkerThread(size_t threadNum);
};

#endif