#pragma once
#include "iMediatorInterface.hpp"
#include "cAnimationComponent.hpp"
#include "cGameObjectFactory.hpp"
#include "MediatorGlobals.hpp"
#include "cError.hpp"
#include "cVAOManager.hpp"
#include "cRenderer.hpp"

class cAnimationManager : iMediatorInterface {
	static cAnimationManager stonAnimaMngr;		// Singleton Enforcement Pointer
	iMediatorInterface* pMediator;				// Mediator Pointer
	cError error;								// Error class for reporting errors via mediator

	std::map<std::string, cAnimationComponent*> mpAnima;
	std::map<std::string, std::vector<glm::mat4x4>[2]> mpFinalTransformation;
	std::map<std::string, std::vector<glm::mat4x4>[2]> mpOffsets;
	std::map<std::string, std::vector<glm::mat4x4>[2]> mpObjectBoneTransforms;
	unsigned next_UniqueComponentID = 0;
	const size_t System_Hex_Value = (0x09 << 16);
	const size_t Module_Hex_Value = ((uint64_t)0x000 << 32);
	// Module_Hex_Value | System_Hex_Value | next_UniqueComponentID++
	//std::map<std::string, cAnimationComponent*>::iterator itAnimaComps;
	
	// FACTORY OBJECT TO HANDLE THE CREATION OF NEW ANIMATION OBJECTS
	cGameObjectFactory _fact_game_obj;

	cAnimationManager();

	void IterateComponent(cAnimationComponent* component, float dt);
	void ResolvePreviousAnimation(std::string currAnim, cAnimationComponent* component, cComplexObject* pParent);
	void DispatchNewAnimation(std::string currAnim, cAnimationComponent* component, cComplexObject* pParent);
	void BlendTransforms(std::pair<std::string, cAnimationComponent*> it);

public:
	static cAnimationManager* GetAnimationManager();
	std::map<std::string, cAnimationComponent*>& GetAnimationComponents() { return mpAnima; }
	void DeconstructAnimationComponents();

	// move to manager, call from loader
	cAnimationComponent* LoadMeshFromFile(const std::string& friendlyName, const std::string& filename);

	void Update(float dt, GLint shaderProgID, cRenderer* pRenderer, cVAOManager* pVAOManager);
	void Render(cRenderer* pRenderer, GLint shaderProgID, cVAOManager* pVAOManager);

	void setMediatorPointer(iMediatorInterface* pMediator);
	virtual sData RecieveMessage(sData& data);
};
