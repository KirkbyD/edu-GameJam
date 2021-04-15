#pragma once
#include <cRigidBody.hpp>
#include <iShape.hpp>
#include "cPhysicsWorld.hpp"

namespace nPhysics {
	class iRigidBodyWrapper {
	protected:
		friend nPhysics::cPhysicsWorld;
		phys::cRigidBody* mRigidBody;
		virtual ~iRigidBodyWrapper() {}
		iRigidBodyWrapper(phys::sRigidBodyDef& def, phys::iShape* shape)
			: mRigidBody(&phys::cRigidBody(def, shape)) {}

	private:
		// Delete constructors so that they cannot be used
		iRigidBodyWrapper() = delete;
		iRigidBodyWrapper(const iRigidBodyWrapper& other) = delete;
		iRigidBodyWrapper& operator=(const iRigidBodyWrapper& other) = delete;
	};
}