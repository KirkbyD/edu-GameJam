#pragma once
#include <physics/interfaces/iPhysicsFactory.h>
#include "cPhysicsWorld.hpp"
#include "cBallComponent.hpp"
#include "cPlaneComponent.hpp"
#include "cClothComponent.hpp"

namespace nPhysics {
	class cPhysicsFactory : public iPhysicsFactory {
	public:
		cPhysicsFactory();
		virtual ~cPhysicsFactory() {}

		// Inherited via iPhysicsFactory
		virtual iPhysicsWorld* CreateWorld() override;
		virtual iBallComponent* CreateBall(const sBallDef& def, const unsigned& id) override;
		virtual iBoxComponent* CreateBox(const sBoxDef& def, const unsigned& id) override;
		virtual iPlaneComponent* CreatePlane(const sPlaneDef& def, const unsigned& id) override;
		virtual iClothComponent* CreateCloth(const sClothDef& def, const unsigned& id) override;
		virtual iCapsuleComponent* CreateCapsule(const sCapsuleDef& def, const unsigned& id) override;
		virtual iTriangleMeshComponent* CreateTriangleMesh(const sTriangleMeshDef& def, const unsigned& id) override;
		virtual iCharacterController* CreateCharacterController(const sCharacterDef& def, const unsigned& id) override;
	};
}