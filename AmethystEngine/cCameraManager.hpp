#pragma once
#include "cError.hpp"
#include "iMediatorInterface.hpp"
#include "cCamera.hpp"

extern std::map<std::string, cModelObject*> mpModel;

class cCameraManager : public iMediatorInterface {
	cCameraManager();
	static cCameraManager stonActMngr;
	iMediatorInterface* pMediator;
	cError error;

	std::map<std::string, cCamera*> map_pCameras;
	std::map<std::string, cCamera*>::iterator it_pCameras;

	unsigned next_UniqueComponentID;
	const size_t System_Hex_Value = (0x07 << 16);
	const size_t Module_Hex_Value = ((uint64_t)0x000 << 32);

public:
	~cCameraManager();

	void Initialize();

	void CreateCamera(std::string Name);
	void CreateCamera(sCameraDef def, std::string Name);

	void Update();

	//Enforced that you cannot delete active camera.
	bool DeleteCamera(std::string Name);
	bool DeleteCamera(cCamera* pCamera);

	cCamera* GetActiveCamera();
	cCamera* GetCameraByName(std::string Name);
	
	bool SetActiveCamera(std::string Name);
	bool SetActiveCamera(cCamera* pCamera);

	// SINGLETON FUNCTIONS
	static cCameraManager* GetCameraManager();

	// MEDIATOR FUNCTIONS
	void setMediatorPointer(iMediatorInterface* pMediator);
	virtual sData RecieveMessage(sData& data);
};
