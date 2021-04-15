#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cTrigger.hpp"


void cTrigger::Init(sData data) {
	this->_position = (glm::vec3)data.getVec4Data()[0];
	this->_radius = data.getFloatData()[0];
	this->_triggered = false;
	if (!data.getStringData().empty()) {
		for (std::string s : data.getStringData()) {
			this->_vec_trigTargets.push_back(s);
		}
	}
}

bool cTrigger::Update(double deltatime) {
	cCommandManager* pcmdmngr = cCommandManager::GetCommandManager();

	// check to see if anything has caused the trigger to fire...
	if (_triggered)
		return _triggered;

	sData data;
	data.setSubsystem(CMD);
	data.setCMD(CHECK_TRIG_POSITION);
	data.addVec4Data(_position);
	data.addFloatData(_radius);
	data.setBoolData(_triggered);
	if (!this->_vec_trigTargets.empty()) {
		for (std::string s : this->_vec_trigTargets)
			data.addStringData(s);
	}
	pcmdmngr->IssueCommand(data);
	this->_triggered = data.getBoolData();

	return _triggered;
}

// DO NOT USE!!!!
float cTrigger::SmootherStep(float startdist, float enddist, float currdist) {
	return 0.0f;
}

// DO NOT USE!!!!
float cTrigger::Clamp(float x) {
	return 0.0f;
}

void cTrigger::AddSerialCommand(iCommand* pCommand) { }
void cTrigger::AddParallelCommand(iCommand* pCommand) { }
