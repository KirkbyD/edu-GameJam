#pragma once
#include <physics/interfaces/iBallComponent.h>
#include <Shapes/cSphere.hpp>
#include "cPhysicsWorld.hpp"

namespace nPhysics {
	class cBallComponent : public iBallComponent {
	private:
		//Friends so the world can access these componen ts freely, it needs them a lot.
		friend nPhysics::cPhysicsWorld;
		phys::cRigidBody* mRigidBody;
		phys::cSphere* mBall;

	public:
		// Only contructor, builds the library's RigidBody and shape.
		cBallComponent(const sBallDef& def, const unsigned& id);
		virtual ~cBallComponent() {}

		bool GetVelocity(glm::vec3& velocityOut) override;

		//Retrieve physics transformation for rendering purposes.
		void GetTransform(glm::mat4& transformOut) override;

		// Inherited via iBallComponent
		virtual void ApplyImpulse(glm::vec3 impulseIn) override;
		virtual void Stop() override;

		virtual bool GetBuildInfo(glm::vec3& position, glm::vec3& velocity, float& radius, float& mass) override;

		// Inherited via iBallComponent
		virtual void SetTransform(glm::mat4 TransformIn) override;

		// Inherited via iBallComponent
		virtual void SetPosition(glm::vec3 positionIn) override;
	};
}