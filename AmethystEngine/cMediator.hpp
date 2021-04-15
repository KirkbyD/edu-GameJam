#pragma once
#include "Globals.hpp"
#include "MediatorGlobals.hpp"
#include "iMediatorInterface.hpp"
#include "cModelManager.hpp"
#include "cLightManager.hpp"
#include "cCommandManager.hpp"
#include "cPhysicsManager.hpp"
#include "cAIManager.hpp"
#include "cActorManager.hpp"
#include "cAnimationManager.hpp"
#include "cCameraManager.hpp"

// NOT PART OF MEDIATOR FIGURE OUT HOW TO CALL THIS WITHOUT IT NEEDING TO BE HERE
// NOTE: WILL BE MOVED OUT IF THE SAME ISSUE HAPPENS WITH THIS THAT HAPPENED WITH THE OTHER COMMANDS FOR OBJECT CONTROL
#include "cFileManager.hpp"

// Add additional includes here for any other subsystem that uses the mediator.

class cMediator : public iMediatorInterface {
	void ToggleDataDisplay();
	void ToggleDebugDisplay();
	void ToggleGodMode();
	void ToggleHover();
	void ControlSystem(sData& data);

	bool CheckTriggerPosition(sData& data);

public:
	cCommandManager* pCmdMngr;
	cLightManager* pLgtMngr;
	cModelManager* pMdlMngr;
	cFileManager* pFilMngr;
	cAudioManager* pAudMngr;
	cPhysicsManager* pPhyMngr;
	cActorManager* pActMngr;
	cAIManager* pAIMngr;
	cAnimationManager* pAniMngr;
	cCameraManager* pCamMngr;

	// RENAME THIS NO IDEA WHAT IT SHOULD BE!!!!
	void ConnectEndpointClients();

	// this will be needed later when the subsystems are interacting with the world and need to send messages to the world and
	// subsequently other subsystems in the engine.
	// void GenerateWorld();

	virtual sData RecieveMessage(sData& data);

	// Any Game Logic to be added below here
	// vector of game objects to comnmunicate with one and other.
	bool GetPosition(sData& data);
	bool SetPosition(sData& data);
	bool SetLookAt(sData& data);
};
