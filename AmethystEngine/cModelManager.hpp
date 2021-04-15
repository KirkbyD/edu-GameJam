#pragma once
#include "iMediatorInterface.hpp"
#include "cModelObject.hpp"
#include "cGameObjectFactory.hpp"
#include "MediatorGlobals.hpp"

extern std::map<std::string, cModelObject*> mpModel;
extern std::map<std::string, cModelObject*>::iterator itModelObjects;

class cModelManager : iMediatorInterface {
	cModelManager();
	static cModelManager stonModelMngr;

	iMediatorInterface* pMediator;

	// FACTORY OBJECT TO HANDLE THE CREATION OF NEW MODEL OBJECTS
	cGameObjectFactory _fact_game_obj;


public:
	static cModelManager* GetModelManager();
	void DeconstructGraphicsComponents();

	void setMediatorPointer(iMediatorInterface* pMediator);

	virtual sData RecieveMessage(sData& data);

	void ToggleWireframe(std::string model);
	void ToggleWireframe();

	void ToggleVisible(std::string model);
	void ToggleVisible();

	void CreateModel(sData& data);

	bool GetPosition(sData& data);
	bool GetOrientation(sData& data);

	bool SetPosition(sData& data);
	bool SetOrientation(sData& data);
	bool SetVelocity(sData& data);
};
