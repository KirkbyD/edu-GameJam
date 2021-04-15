#pragma once
#include <functional>
#include <queue>
#include <thread>

class cThreadPool;
class cJobQueue;
class cWorkerThreads {
	cThreadPool& mPool;
	cJobQueue* mJobQueue;
	std::thread* mThread;

public:
	cWorkerThreads(cThreadPool& pool, cJobQueue* job_queue) :mPool(pool), mJobQueue(job_queue) {
		mThread = new std::thread(&cWorkerThreads::ExecuteJob, this);
	}
	~cWorkerThreads() { }

	//void Join() { mThread->join(); }
	void Detach() { mThread->detach(); }
	void ExecuteJob();
};
