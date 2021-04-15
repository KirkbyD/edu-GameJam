#pragma once
#include <physics/interfaces/iTriangleMeshComponent.h>
#include <btBulletDynamicsCommon.h>
#include "cPhysicsWorld.hpp"

namespace nPhysics {
	class cTriangleMeshComponent : public iTriangleMeshComponent {
	private:
		friend nPhysics::cPhysicsWorld;
		btRigidBody* mBody;
		btBvhTriangleMeshShape* mMesh;
		btVector3 mOffsetPosition; //Our position, since bvhtrimeshshape is relative to origin.
		btQuaternion mOffsetRotation;

	public:
		// Only contructor, builds the library's RigidBody and shape.
		cTriangleMeshComponent(const sTriangleMeshDef& def, const unsigned& id);
		virtual ~cTriangleMeshComponent();

		bool GetVelocity(glm::vec3& velocityOut) override;

		//Retrieve physics transformation for rendering purposes.
		void GetTransform(glm::mat4& transformOut) override;
		virtual void SetTransform(glm::mat4 TransformIn) override;
		virtual bool GetBuildInfo(glm::vec3& normal, float& constant) override;
		virtual void SetPosition(glm::vec3 positionIn) override;
	};
}