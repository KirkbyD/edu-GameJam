#pragma once
#include "iBallComponent.h"
#include "iPlaneComponent.h"
#include "iPhysicsWorld.h"
#include "iClothComponent.h"
#include "iCollisionListener.h"
#include "iBoxComponent.h"
#include "iCapsuleComponent.h"
#include "iTriangleMeshComponent.h"
#include "iCharacterController.h"

namespace nPhysics {
	//iPhysicsFactoru
	//
	// Abstract factory to allow our game to hook up to wrapper factories of different libraries.
	class iPhysicsFactory {
	public:
		virtual ~iPhysicsFactory() {}
		virtual iPhysicsWorld* CreateWorld() = 0;

		// components
		virtual iBallComponent* CreateBall(const sBallDef& def, const unsigned& id) = 0;
		virtual iBoxComponent* CreateBox(const sBoxDef& def, const unsigned& id) = 0;
		virtual iCapsuleComponent* CreateCapsule(const sCapsuleDef& def, const unsigned& id) = 0;
		virtual iClothComponent* CreateCloth(const sClothDef& def, const unsigned& id) = 0;
		virtual iPlaneComponent* CreatePlane(const sPlaneDef& def, const unsigned& id) = 0;
		virtual iTriangleMeshComponent* CreateTriangleMesh(const sTriangleMeshDef& def, const unsigned& id) = 0;
		virtual iCharacterController* CreateCharacterController(const sCharacterDef& def, const unsigned& id) = 0;
	};
}