#pragma once
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

#include "cWorkerThreads.hpp"

class cJobQueue;
class cThreadPool {
    std::vector<cWorkerThreads*> mWorkers;
    std::mutex mtxThreadStatus;       // TODO: This seems like its not entirely needed...
    std::condition_variable mCV;

    cJobQueue* mJobQueue;

    bool mIsRunning;
    int mSize;

    friend class cWorkerThreads;

public:
    // accepts the size of the thread pool, if no size specified sets the size to the hardware default available
    cThreadPool(cJobQueue* job_queue, int nSize = std::thread::hardware_concurrency()) :mSize(nSize), mIsRunning(true), mJobQueue(job_queue) { }
    ~cThreadPool() { Terminate(); }

    // starts up and initializes the worker threads within the thread pool and detatches them
    void Init() {
        for (int i = 0; i < mSize; ++i) {
            cWorkerThreads* pWorkerThread = new cWorkerThreads(*this, mJobQueue);
            mWorkers.push_back(pWorkerThread);
        }
    }

    // shuts down and deletes the worker threads in the pool and the pool itself
    void Terminate() {
        std::unique_lock lock(mtxThreadStatus);
        mIsRunning = false;
        mCV.notify_all();

        // Delete all threads.
        for (cWorkerThreads* worker : mWorkers) {
            worker->Detach();
            delete worker;
        }
        mWorkers.clear();
    }

    bool& isRunning() { return mIsRunning; }
    std::condition_variable& GetCondition() { return this->mCV; }
};
