#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include "sData.hpp"

class cJobQueue {
public:
    typedef std::function<void(sData)> Update;      // Update Function Pointer Declaration

private:
    std::pair<Update, sData*> mJob;
    std::queue<std::pair<Update, sData*>> mJobs;
    std::mutex mtxLock;
    std::condition_variable mCV;

public:
    cJobQueue() { }
	~cJobQueue() {
        // clean up any outstanding commands on the queue
        while (!mJobs.empty())
            mJobs.pop();

        // clear the command/job currently stored and delete the pointer
        if (mJob.second != nullptr) {
            mJob.second = nullptr;
            delete mJob.second;
        }
    }

    // places a job within the job queue
    void AddJob(std::pair<Update, sData*> job) {
        mtxLock.lock();
        mJobs.push(job);
        mtxLock.unlock();
        mCV.notify_all();
    }

    // function to remove an object from the front of the queue
    // retrieves the front job in the job queue
    std::pair<Update, sData*>& RemoveJob() {
        std::unique_lock<std::mutex> ulk(mtxLock);
        mCV.wait(ulk, [this]() { return !mJobs.empty(); });
        mJob = mJobs.front();
        mJobs.pop();
        ulk.unlock();
        mCV.notify_all();
        return mJob;
    }

    bool empty() { return mJobs.empty(); }
    size_t size() { return mJobs.size(); }
};
