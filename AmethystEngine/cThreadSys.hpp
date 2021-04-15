#pragma once
#include <thread>
#include <functional>
#include "sData.hpp"

class cThreadPool;
class cJobQueue;

class cThreadSys {
	cThreadPool* threadPool;
	cJobQueue* jobQueue;

public:

	cThreadSys() : threadPool(nullptr), jobQueue(nullptr) { }
	~cThreadSys();

	void InitJobQueue();
	void InitThreadPool();
	void InitWorkerThreads();

	void QueueJob(std::function<void(sData)> func, sData data);

};
