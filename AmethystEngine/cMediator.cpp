#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__, __func__)
#else
#define DBG_NEW
#endif

#include "cMediator.hpp"
#include <iostream>

void cMediator::ConnectEndpointClients() {
	pCmdMngr = cCommandManager::GetCommandManager();
	pLgtMngr = cLightManager::GetLightManager();
	pMdlMngr = cModelManager::GetModelManager();
	pFilMngr = cFileManager::GetFileManager();
	pAudMngr = cAudioManager::GetAudioManager();
	pPhyMngr = cPhysicsManager::GetPhysicsManager();
	pActMngr = cActorManager::GetActorManager();
	pAIMngr = cAIManager::GetAIManager();
	pAniMngr = cAnimationManager::GetAnimationManager();
	pCamMngr = cCameraManager::GetCameraManager();

	iMediatorInterface* pMediatorInterface = this;
	pCmdMngr->setMediatorPointer(pMediatorInterface);
	pLgtMngr->setMediatorPointer(pMediatorInterface);
	pMdlMngr->setMediatorPointer(pMediatorInterface);
	pPhyMngr->setMediatorPointer(pMediatorInterface);
	pAudMngr->setMediatorPointer(pMediatorInterface);
	pActMngr->setMediatorPointer(pMediatorInterface);
	pAIMngr->setMediatorPointer(pMediatorInterface);
	pAniMngr->setMediatorPointer(pMediatorInterface);
	pCamMngr->setMediatorPointer(pMediatorInterface);
}



sData cMediator::RecieveMessage(sData& data) {
	switch (data.getSubsystem()) {
		case COMPLEX:
			// something to send to some place here (potentially the same as models???)
			break;

		case MODELS:
			pMdlMngr->RecieveMessage(data);
			break;

		case LIGHTS:
			pLgtMngr->RecieveMessage(data);
			break;

		case PHYSICS:
			pPhyMngr->RecieveMessage(data);
			break;

		case AUDIO:
			break;

		case ACTORS:
			pActMngr->RecieveMessage(data);
			break;

		case AI:
			pAIMngr->RecieveMessage(data);
			break;

		case CAMERA:
			pCamMngr->RecieveMessage(data);
			break;

		case CMD:
			switch (data.getCMD()) {
			case TGM:
				//Toggle god mode for game jam
				ToggleGodMode();
				break;

			case HOVER:
				//Hover for game jam!
				ToggleHover();
				break;

				// Special case for non subsystem specified command
			case WIREFRAME:
				data.setSubsystem(MODELS);
				pMdlMngr->RecieveMessage(data);
				break;

				// Special case for non subsystem specified command
			case VISIBLE:
				if (isModelSelect) {
					data.setSubsystem(MODELS);
					pMdlMngr->RecieveMessage(data);
				}
				else if (isLightSelect) {
					data.setSubsystem(LIGHTS);
					pLgtMngr->RecieveMessage(data);
				}
				else if (isComplexSelect) {
					data.setSubsystem(COMPLEX);
					// should send to a complex manager??? or the model manager???
				}
				break;

			case CREATE:
				if (isModelSelect) {
					data.setSubsystem(MODELS);
					pMdlMngr->RecieveMessage(data);
				}
				else if (isLightSelect) {
					data.setSubsystem(LIGHTS);
					pLgtMngr->RecieveMessage(data);
				}
				break;

			case SELECT:
				ControlSystem(data);
				break;

			case SAVE_FILES:
				pFilMngr->SaveData();
				// add stuff for saving individual files based off of the string data passed in
				break;

			case DATA_DISPLAY:
				ToggleDataDisplay();
				break;

			case DEBUG_MODE:
				ToggleDebugDisplay();
				break;

				// Tell physics to do this and pass it to physics
			case CHECK_TRIG_POSITION:
				data.setBoolData(CheckTriggerPosition(data));
				break;

			case !UNKN_CMD:
				data.setResult(INVALID_COMMAND);
				break;

			default:
				data.setResult(UNKNOWN_COMMAND);
				break;
			}

		default:
			data.setResult(UNKNOWN_SUBSYSTEM);
			break;
	}
	return data;
}



#pragma region COMMAND EXECUTIONS
void cMediator::ToggleDataDisplay() { isDataDisplay = !isDataDisplay; }
void cMediator::ToggleDebugDisplay() { isDebugDisplay = !isDebugDisplay; }
void cMediator::ToggleGodMode() { cheatGodMode = !cheatGodMode; }
void cMediator::ToggleHover() { cheatHover = !cheatHover; }

void cMediator::ControlSystem(sData &data) {
	switch (data.getSubsystem()) {
	case MODELS:
		isModelSelect = true;
		isLightSelect = false;
		isComplexSelect = false;
		isAudioSelect = false;
		break;

	case LIGHTS:
		isModelSelect = false;
		isLightSelect = true;
		isComplexSelect = false;
		isAudioSelect = false;
		break;

	case COMPLEX:
		isModelSelect = false;
		isLightSelect = false;
		isComplexSelect = true;
		isAudioSelect = false;
		break;

	case AUDIO:
		isModelSelect = false;
		isLightSelect = false;
		isComplexSelect = false;
		isAudioSelect = true;
		break;

	case ACTORS:
	case PHYSICS:
	case CAMERA:
		data.setResult(INVALID_SUBSYSTEM);
		break;

	default:
		data.setResult(UNKNOWN_SUBSYSTEM);
		break;
	}
}

bool cMediator::CheckTriggerPosition(sData& data) {
	if (!data.getStringData().empty()) {
		for (cModelObject* cmo : g_vec_pGameObjects) {
			for (std::string s : data.getStringData()) {
				if (cmo->friendlyName == s) {
					float currentDist = glm::distance(cmo->positionXYZ, (glm::vec3)data.getVec4Data()[0]);
					if (currentDist < data.getFloatData()[0]) { return true; }
				}
			}
		}
	}
	else {
		for (cModelObject* cmo : g_vec_pGameObjects) {
			if (cmo->inverseMass > 0.0f) {
				float currentDist = glm::distance(cmo->positionXYZ, (glm::vec3)data.getVec4Data()[0]);
				if (currentDist < data.getFloatData()[0]) { return true; }
			}
		}
	}
	return false;
}

bool cMediator::GetPosition(sData& data) {
	/*size_t prevSize = data.getVec4Data().size();
	data.addVec4Data(FreeCamera.GetEye());
	if (data.getVec4Data().size() == (prevSize + 1)) return true;
	else*/ return false;
}

bool cMediator::SetPosition(sData& data) {
	/*FreeCamera.SetEye((glm::vec3)data.getVec4Data()[0]);
	if (FreeCamera.GetEye() == glm::vec3(data.getVec4Data()[0])) return true;
	else*/ return false;
}
bool cMediator::SetLookAt(sData& data) {
	//FreeCamera.SetTarget(mpModel[data.getTarget()]);
	return false;
}
#pragma endregion
