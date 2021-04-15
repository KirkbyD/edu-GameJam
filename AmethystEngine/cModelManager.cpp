#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include <iostream>
#include "cModelManager.hpp"

extern std::vector<cModelObject*> g_vec_pGameObjects;


#pragma region SINGLETON
cModelManager cModelManager::stonModelMngr;
cModelManager* cModelManager::GetModelManager() { return &(cModelManager::stonModelMngr); }
cModelManager::cModelManager() {
	std::cout << "Model Manager Created" << std::endl;
	pMediator = nullptr;
}
#pragma endregion



void cModelManager::DeconstructGraphicsComponents() {
	for (auto a : g_vec_pGameObjects) {
		delete a;
	}
}


#pragma region MEDIATOR_COMMUNICATION
void cModelManager::setMediatorPointer(iMediatorInterface* pMediator) {
	this->pMediator = pMediator;
	return;
}



sData cModelManager::RecieveMessage(sData& data) {
	data.setResult(OK);

	switch (data.getCMD()) {
	case WIREFRAME:
		if (!data.getCMDArgs().empty()) {
			if (data.getCMDArgs()[0] == ALL)
				ToggleWireframe();
			else
				data.setResult(INVALID_COMMAND_ARGS);
		}
		else {
			if (data.getStringData().empty())
				data.addStringData(itModelObjects->second->friendlyName);
			ToggleWireframe(data.getStringData()[0]);
		}
		break;

	case VISIBLE:
		if (!data.getCMDArgs().empty()) {
			if (data.getCMDArgs()[0] == ALL)
				ToggleVisible();
			else
				data.setResult(INVALID_COMMAND_ARGS);
		}
		else {
			if (data.getStringData().empty())
				data.addStringData(itModelObjects->second->friendlyName);
			ToggleVisible(data.getStringData()[0]);
		}
		break;

	case CREATE:
		CreateModel(data);
		break;

	case GET_POSITION:
		if (!GetPosition(data)) data.setResult(NOK);
		break;

	case GET_ORIENTATION:
		if (!GetOrientation(data)) data.setResult(NOK);
		break;

	case SET_POSITION:
		if (!SetPosition(data)) data.setResult(NOK);
		break;

	case SET_ORIENTATION:
		if (!SetOrientation(data)) data.setResult(NOK);
		break;

	case SET_VELOCITY:
		if (!SetVelocity(data)) data.setResult(NOK);
		break;

	case !UNKN_CMD:
		data.setResult(INVALID_COMMAND);
		break;

	default:
		data.setResult(UNKNOWN_COMMAND);
		break;
	}
	return data;
}
#pragma endregion



#pragma region CREATE
void cModelManager::CreateModel(sData &data) {
	if (_fact_game_obj.CreateMediatedGameObject("model", data.getStringData()[0], data.getStringData()[1]) == NULL)
		data.setResult(NOK);
}
#pragma endregion



#pragma region WIREFRAME
void cModelManager::ToggleWireframe(std::string model) { mpModel[model]->isWireframe = !mpModel[model]->isWireframe; }
void cModelManager::ToggleWireframe() {
	for (std::pair<std::string, cModelObject*> mapped_cmo : mpModel)
		mapped_cmo.second->isWireframe = !mapped_cmo.second->isWireframe;
}
#pragma endregion



#pragma region VISIBILITY
void cModelManager::ToggleVisible(std::string model) { mpModel[model]->isVisible = !mpModel[model]->isVisible; }
void cModelManager::ToggleVisible() {
	for (std::pair<std::string, cModelObject*> mapped_cmo : mpModel)
		mapped_cmo.second->isVisible = !mapped_cmo.second->isVisible;
}
#pragma endregion



#pragma region GETS
bool cModelManager::GetPosition(sData& data) {
	size_t prevSize = data.getVec4Data().size();
	data.addVec4Data(mpModel[data.getTarget()]->positionXYZ.x, mpModel[data.getTarget()]->positionXYZ.y, mpModel[data.getTarget()]->positionXYZ.z);
	if (data.getVec4Data().size() == (prevSize + 1)) return true;
	else return false;
}

bool cModelManager::GetOrientation(sData& data) {
	size_t prevSize = data.getQuatData().size();
	data.addQuatData(mpModel[data.getTarget()]->getQOrientation());
	if (data.getVec4Data().size() == (prevSize + 1)) return true;
	else return false;
}
#pragma endregion



#pragma region SETS
bool cModelManager::SetPosition(sData& data) {
	mpModel[data.getTarget()]->positionXYZ = glm::vec3(data.getVec4Data()[0]);
	if (mpModel[data.getTarget()]->positionXYZ == glm::vec3(data.getVec4Data()[0])) return true;
	else return false;
}

bool cModelManager::SetOrientation(sData& data) {
	mpModel[data.getTarget()]->setOrientation(data.getQuatData()[0]);
	if (mpModel[data.getTarget()]->getQOrientation() == glm::quat(data.getQuatData()[0])) return true;
	else return false;
}

bool cModelManager::SetVelocity(sData& data) {
	mpModel[data.getTarget()]->velocity = glm::vec3(data.getVec4Data()[0]);
	if (mpModel[data.getTarget()]->velocity == glm::vec3(data.getVec4Data()[0])) return true;
	else return false;
}
#pragma endregion
