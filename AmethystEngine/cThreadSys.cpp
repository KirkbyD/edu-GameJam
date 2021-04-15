#include "cThreadSys.hpp"
#include "cThreadPool.hpp"
#include "cJobQueue.hpp"

cThreadSys::~cThreadSys() {
	threadPool->Terminate();
	threadPool = nullptr;
	delete threadPool;

	jobQueue->~cJobQueue();
	jobQueue = nullptr;
	delete jobQueue;
}

void cThreadSys::InitThreadPool() {
	if (threadPool != nullptr)
		return;						// YO WTF YOU DOIN???
	threadPool = new cThreadPool(jobQueue);
}

void cThreadSys::QueueJob(std::function<void(sData)> func, sData data) {
	sData d = data;
	auto func_data = std::make_pair(func, &d);
	jobQueue->AddJob(func_data);
}

void cThreadSys::InitJobQueue() {
	if (jobQueue != nullptr)
		return;						// YO WTF YOU DOIN???
	
	jobQueue = new cJobQueue();
}

// Executes the queued jobs in order until the queue is empty...
void cThreadSys::InitWorkerThreads() {
	threadPool->Init();
}