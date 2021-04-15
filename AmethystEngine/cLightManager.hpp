#pragma once
#include <vector>
#include "GLCommon.hpp"
#include "cLightObject.hpp"
#include "cGameObjectFactory.hpp"
#include "iMediatorInterface.hpp"
#include "MediatorGlobals.hpp"

extern std::map<std::string, cLightObject*> mpLight;
extern std::vector<cLightObject*> g_vec_pLightObjects;
extern int vpLightObjectsCurrentPos;

class cLightManager : iMediatorInterface {
	cLightManager();
	static cLightManager stonLightMngr;

	iMediatorInterface* pMediator;

	// FACTORY OBJECT TO HANDLE THE CREATION OF NEW LIGHT OBJECTS
	cGameObjectFactory _fact_game_obj;


public:
	static cLightManager* GetLightManager();

	void DeconstructLightComponents();

	void LoadLightsToGPU(std::vector<cLightObject*> vLightObjects, const GLuint &shaderProgID);



	// Mediator Functions
	void setMediatorPointer(iMediatorInterface* pMediator);
	virtual sData RecieveMessage(sData& data);

	// VISIBILITY FUNCTIONS
	void ToggleVisible(std::string light);
	void ToggleVisible();

};
