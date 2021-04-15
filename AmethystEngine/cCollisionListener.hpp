#pragma once
#include <physics/interfaces/iCollisionListener.h>
#include "cAudioManager.hpp"

//Forward declared so that we can call its physics world functions in .cpp
class cPhysicsManager;

//cInherits from the iCollisionListener interface in Physics_Interfaces project.
class cCollisionListener : public nPhysics::iCollisionListener{
private:
	cAudioManager* pAudioManager;
	cPhysicsManager* pPhysicsManager;
public:
	cCollisionListener(cPhysicsManager* physMngr) : nPhysics::iCollisionListener(), pAudioManager(cAudioManager::GetAudioManager()), pPhysicsManager(physMngr) {}
	virtual ~cCollisionListener() { }

	// Inherited via iCollisionListener
	// The Collide function is automatically called at the physics library level and chains outward to be resolved here.
	virtual void Collide(nPhysics::iPhysicsComponent* compA, nPhysics::iPhysicsComponent* compB) override;

	void CharacterCollision(nPhysics::iPhysicsComponent* character, nPhysics::iPhysicsComponent* other);
};