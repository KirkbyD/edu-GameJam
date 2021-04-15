#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cLookAt.hpp"

void cLookAt::Init(sData data) {
	this->_subsystem = data.getSubsystem();
	this->_target = data.getTarget();
}

bool cLookAt::Update(double deltatime) {
	cCommandManager* pcmdmngr = cCommandManager::GetCommandManager();

	sData data;
	data.setSubsystem(_subsystem);
	data.setCMD(LOOKAT);
	data.setTarget(_target);
	pcmdmngr->IssueCommand(data);

	// THIS SHOULD NEVER HAPPEN
	if (data.getResult() != OK)
		return false;

	return true;
}

float cLookAt::SmootherStep(float startdist, float enddist, float currdist) { return 0.0f; }
float cLookAt::Clamp(float x) { return 0.0f; }
void cLookAt::AddSerialCommand(iCommand* pCommand) { }
void cLookAt::AddParallelCommand(iCommand* pCommand) { }
