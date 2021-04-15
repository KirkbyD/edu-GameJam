#pragma once
#pragma once
#include <physics/interfaces/iCapsuleComponent.h>
#include <BulletCollision\CollisionShapes\btCollisionShape.h>
#include "cPhysicsWorld.hpp"

namespace nPhysics {
	class cCapsuleComponent : public iCapsuleComponent {
	private:
		//Friends so the world can access these componen ts freely, it needs them a lot.
		friend nPhysics::cPhysicsWorld;
		btRigidBody* mBody;
		glm::quat mOrientationInv;

	public:
		// Only contructor, builds the library's RigidBody and shape.
		cCapsuleComponent(const sCapsuleDef& def, const unsigned& id);
		virtual ~cCapsuleComponent();

		bool GetVelocity(glm::vec3& velocityOut) override;

		//Retrieve physics transformation for rendering purposes.
		void GetTransform(glm::mat4& transformOut) override;
		virtual void SetTransform(glm::mat4 TransformIn) override;

		// Inherited via iCapsuleComponent
		// rename to void ApplyCentralForce to make more sense with bullet?
		virtual void ApplyImpulse(glm::vec3 impulseIn) override;

		virtual void Stop() override;

		virtual bool GetBuildInfo(glm::vec3& position, glm::vec3& velocity, float& radius, float& mass) override;

		// Inherited via iCapsuleComponent
		virtual void SetPosition(glm::vec3 positionIn) override;
	};
}