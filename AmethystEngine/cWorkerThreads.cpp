#include "cWorkerThreads.hpp"
#include "cJobQueue.hpp"
#include "cThreadPool.hpp"

void cWorkerThreads::ExecuteJob() {
	while (mPool.isRunning()) {
		if (!mJobQueue->empty()) {
			auto update = mJobQueue->RemoveJob();
			//update.first(*update.second);
		}
	}
}
