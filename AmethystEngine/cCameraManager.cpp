#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cCameraManager.hpp"
#include <iostream>

#pragma region SINGLETON
cCameraManager cCameraManager::stonActMngr;
cCameraManager* cCameraManager::GetCameraManager() { return &(cCameraManager::stonActMngr); }
cCameraManager::cCameraManager()
	: pMediator(nullptr)
	, next_UniqueComponentID(0)
{
	std::cout << "Camera Manager Created" << std::endl;
}
#pragma endregion

cCameraManager::~cCameraManager() {
	//Delete all cameras
	for (auto it = map_pCameras.begin();
		it != map_pCameras.end();
		it++) {
		delete it->second;
	}
	map_pCameras.clear();
}

void cCameraManager::Initialize() {
	next_UniqueComponentID = 0;
	CreateCamera("Default");
	it_pCameras = map_pCameras.find("Default");
}

void cCameraManager::CreateCamera(std::string Name) {
	map_pCameras[Name] = new cCamera(next_UniqueComponentID++);
}

void cCameraManager::CreateCamera(sCameraDef def, std::string Name) {
	map_pCameras[Name] = new cCamera(next_UniqueComponentID++, def);
}

void cCameraManager::Update() {
	for (auto cameraIT : map_pCameras) {
		if (cameraIT.second->isPortal)
		{
			cameraIT.second->PortalViewUpdate();
		}
		else
		{
			cameraIT.second->MoveFront();
			cameraIT.second->Move();
		}
	}
}

//Enforced that you cannot delete active camera.
bool cCameraManager::DeleteCamera(std::string Name) {
	//check if exists
	if (map_pCameras.find(Name) != map_pCameras.end()) {
		//Check if is active camera
		if (it_pCameras == map_pCameras.find(Name))
			return false;

		delete map_pCameras[Name];
		map_pCameras.erase(Name);
		return true;
	}
	return false;
}

//Enforced that you cannot delete active camera.
bool cCameraManager::DeleteCamera(cCamera* pCamera) {
	//check if exists
	for (auto it = map_pCameras.begin();
		it != map_pCameras.end(); 
		it++) {
		if (it->second == pCamera) {
			////Check if is active camera
			if (it_pCameras == it)
				return false;

			delete it->second;
			map_pCameras.erase(it);
			return true;
		}
	}
	return false;
}

cCamera* cCameraManager::GetActiveCamera() {
	return it_pCameras->second;
}

cCamera* cCameraManager::GetCameraByName(std::string Name) {
	//check if exists
	if (map_pCameras.find(Name) != map_pCameras.end()) {
		return map_pCameras[Name];
	}
	return nullptr;
}

bool cCameraManager::SetActiveCamera(std::string Name) {
	if (Name == it_pCameras->first)
		return true; //already set to what you want.

	//check if exists
	if (map_pCameras.find(Name) != map_pCameras.end()) {
		it_pCameras = map_pCameras.find(Name);
		return true;
	}
	return false;
}

bool cCameraManager::SetActiveCamera(cCamera* pCamera) {
	if (pCamera == it_pCameras->second)
		return true;	//already set to what you want.

	//check if exists
	for (auto it = map_pCameras.begin();
		it != map_pCameras.end();
		it++) {
		if (it->second == pCamera) {
			it_pCameras = it;			
			return true;
		}
	}
	return false;
}


#pragma region MEDIATOR SETUP
void cCameraManager::setMediatorPointer(iMediatorInterface* pMediator) { this->pMediator = pMediator; }
#pragma endregion


#pragma region MEDIATOR COMMUNICATIONS
sData cCameraManager::RecieveMessage(sData& data) {
	switch (data.getCMD()) {
	case GET_ACTIVE_FRONT:
		data.addVec4Data(this->it_pCameras->second->GetFront());
		break;

	case GET_POSITION:
		data.addVec4Data(this->it_pCameras->second->GetPosition());
		break;

	case SET_POSITION:
		this->it_pCameras->second->SetPosition((glm::vec3)data.getVec4Data()[0]);
		break;

	case LOOKAT:
		this->it_pCameras->second->SetFollowObject(mpModel[data.getTarget()]);
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
