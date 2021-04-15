#pragma once
#include <physics/interfaces/iPlaneComponent.h>
#include <Shapes/cPlane.hpp>
#include "cPhysicsWorld.hpp"

namespace nPhysics {
	class cPlaneComponent : public iPlaneComponent {
	private:
		friend nPhysics::cPhysicsWorld;
		phys::cRigidBody* mRigidBody;
		phys::cPlane* mPlane;

	public:
		// Only contructor, builds the library's RigidBody and shape.
		cPlaneComponent(const sPlaneDef& def, const unsigned& id);

		bool GetVelocity(glm::vec3& velocityOut) override;

		//Retrieve physics transformation for rendering purposes.
		void GetTransform(glm::mat4& transformOut) override;

		virtual bool GetBuildInfo(glm::vec3& normal, float& constant) override;

		// Inherited via iPlaneComponent
		virtual void SetTransform(glm::mat4 TransformIn) override;

		// Inherited via iPlaneComponent
		virtual void SetPosition(glm::vec3 positionIn) override;
	};
}