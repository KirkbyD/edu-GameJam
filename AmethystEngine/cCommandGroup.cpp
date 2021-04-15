#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cCommandGroup.hpp"
void cCommandGroup::Init(sData data) { }
bool cCommandGroup::Update(double deltatime) {
	// For serial commands, run the "next" command until it's done.
	if (!this->_serial_list.empty()) {
		iCommand* pCurrent = this->_serial_list.front();

		// Serial commands
		if (pCurrent->Update(deltatime)) {
			delete pCurrent;
			this->_serial_list.pop();
		}
		return false;
	}
	else if (!this->_parallel_list.empty()) {
		bool isDone = false;
		std::vector<bool> _vec_isDone(_parallel_list.size(), false);
		size_t i = 0;

		for (std::vector< iCommand* >::iterator itCommand = this->_parallel_list.begin(); itCommand != this->_parallel_list.end(); ++itCommand, ++i) {
			iCommand* pCurrent = *itCommand;

			if (pCurrent->Update(deltatime))
				_vec_isDone[i] = true;
		}
		for (bool b : _vec_isDone) {
			if (b == true)
				isDone = true;
			else {
				isDone = false;
				break;
			}
		}
		if (isDone) {
			for (iCommand* cmd : _parallel_list)
				delete cmd;
			this->_parallel_list.erase(this->_parallel_list.begin(), this->_parallel_list.end());
			return true;
		}

		return false;
	}
	else
		return true;
}



float cCommandGroup::SmootherStep(float startdist, float enddist, float currdist) {
	return 0.0f;
}

float cCommandGroup::Clamp(float x) {
	return 0.0f;
}

void cCommandGroup::AddSerialCommand(iCommand* cmd) {
	_serial_list.push(cmd);
}

void cCommandGroup::AddParallelCommand(iCommand* cmd) {
	_parallel_list.push_back(cmd);
}
