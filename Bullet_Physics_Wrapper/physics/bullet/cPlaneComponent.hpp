#pragma once
#include <physics/interfaces/iPlaneComponent.h>
#include <btBulletDynamicsCommon.h>
#include "cPhysicsWorld.hpp"

namespace nPhysics {
	class cPlaneComponent : public iPlaneComponent {
	private:
		friend nPhysics::cPhysicsWorld;
		btRigidBody* mBody;

	public:
		// Only contructor, builds the library's RigidBody and shape.
		cPlaneComponent(const sPlaneDef& def, const unsigned& id);
		virtual ~cPlaneComponent();

		bool GetVelocity(glm::vec3& velocityOut) override;

		//Retrieve physics transformation for rendering purposes.
		void GetTransform(glm::mat4& transformOut) override;
		virtual void SetTransform(glm::mat4 TransformIn) override;
		virtual bool GetBuildInfo(glm::vec3& normal, float& constant) override;
		virtual void SetPosition(glm::vec3 positionIn) override;
	};
}