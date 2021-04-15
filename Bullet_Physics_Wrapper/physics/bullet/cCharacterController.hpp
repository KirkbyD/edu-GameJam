#pragma once
#include <physics/interfaces/iCharacterController.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "cPhysicsWorld.hpp"

namespace nPhysics {
	class cCharacterController : public iCharacterController {
	private:
		//Friends so the world can access these componen ts freely, it needs them a lot.
		friend nPhysics::cPhysicsWorld;

		btKinematicCharacterController* mController;
		btPairCachingGhostObject* mGhostObject;

	public:
		// Only constructor, builds the library's btKinematicCharacterController.
		cCharacterController(const sCharacterDef& def, const unsigned& id);
		virtual ~cCharacterController();

		// Inherited via iPhysicsComponent
		virtual void GetTransform(glm::mat4& transformOut) override;
		virtual void SetTransform(glm::mat4 TransformIn) override;
		virtual bool GetVelocity(glm::vec3& velocityOut) override;
		//Assumes incoming position is from the model it's keyed to and adjusts height accordingly.
		virtual void SetPosition(glm::vec3 positionIn) override;

		// Inherited via iCharacterController
		virtual void SetOrientation(glm::quat orientationIn) override;
		virtual void SetLinearVelocity(glm::vec3 velIn) override;
		virtual void Stop() override;
		virtual void Jump(glm::vec3 jumpVel) override;
	};
}