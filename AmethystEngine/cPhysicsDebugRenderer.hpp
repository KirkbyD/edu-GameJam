#pragma once
#include <physics/interfaces/iDebugRenderer.h>
#include "DebugRenderer/cDebugRenderer.hpp"

//cInherits from the iDebugRenderer interface in Physics_Interfaces project.
class cPhysicsDebugRenderer : public nPhysics::iDebugRenderer {
private:
	cDebugRenderer* mSystemRenderer;

public:
	cPhysicsDebugRenderer(cDebugRenderer* SystemRenderer) 
		: nPhysics::iDebugRenderer(), mSystemRenderer(SystemRenderer) {}

	virtual ~cPhysicsDebugRenderer() { }

	// Inherited via iCollisionListener
	// The Collide function is automatically called at the physics library level and chains outward to be resolved here.
	virtual void DrawLine(nPhysics::sDebugLine line) override;
};