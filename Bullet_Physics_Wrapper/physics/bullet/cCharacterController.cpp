#include "cCharacterController.hpp"
#include "nConvert.hpp"

namespace nPhysics {
	cCharacterController::cCharacterController(const sCharacterDef& def, const unsigned& id)
		: iCharacterController(id) {

		btTransform Transform;
		Transform.setIdentity();
		Transform.setOrigin(nConvert::ToBullet(def.Position));
		Transform.setRotation(nConvert::ToBullet(def.Rotation));
		//btTransform bullet_matrix(nConvert::ToBullet(def.Rotation), nConvert::ToBullet(def.Position));

		btConvexShape* ConvexShape = new btCapsuleShape(def.Radius, def.Height);	

		mGhostObject = new btPairCachingGhostObject();
		mGhostObject->setWorldTransform(Transform);
		mGhostObject->setCollisionShape(ConvexShape);
		mGhostObject->setCollisionFlags(mGhostObject->getCollisionFlags() | btCollisionObject::CF_CHARACTER_OBJECT);
		mGhostObject->setUserPointer(this);

		mController = new btKinematicCharacterController(mGhostObject, ConvexShape, def.StepHeight);
	
		//using as an offset for the animated model.
		mController->setHalfHeight(def.Height / 2 + def.Radius);
	}

	cCharacterController::~cCharacterController() {
		//Handles deleting its own shape instead of leaving it to the world.
		mGhostObject->setUserPointer(0);
		delete mGhostObject->getCollisionShape();
		delete mController->getGhostObject();
		mGhostObject = 0;
		delete mController;
		mController = 0;
	}

	void cCharacterController::GetTransform(glm::mat4& transformOut) {
		nConvert::ToSimple(mGhostObject->getWorldTransform(), transformOut);
		transformOut[3][1] -= mController->getHalfHeigtht();
	}

	void cCharacterController::SetTransform(glm::mat4 TransformIn) {
		//TODO: code converter
		//mGhostObject->setWorldTransform(nConvert::ToBullet(TransformIn));
	}

	bool cCharacterController::GetVelocity(glm::vec3& velocityOut) {
		velocityOut = nConvert::ToSimple(mController->getLinearVelocity());
		return true;
	}
	
	void cCharacterController::SetPosition(glm::vec3 positionIn) {
		positionIn.y += mController->getHalfHeigtht();
		btTransform Transform = mGhostObject->getWorldTransform();
		Transform.setOrigin(nConvert::ToBullet(positionIn));
		mGhostObject->setWorldTransform(Transform);
	}

	void cCharacterController::SetOrientation(glm::quat orientationIn) {
		// may need to store initial transformation as an offset for this.
		auto Transform = mGhostObject->getWorldTransform();
		Transform.setRotation(nConvert::ToBullet(orientationIn));
		mGhostObject->setWorldTransform(Transform);
	}

	void cCharacterController::SetLinearVelocity(glm::vec3 velIn) {
		mController->setLinearVelocity(nConvert::ToBullet(velIn));
	}

	void cCharacterController::Stop() {
		btVector3 currVel = mController->getLinearVelocity();
		//preserve vertical velocity if falling
		mController->setLinearVelocity(btVector3(0.f, currVel.y(), 0.f));
	}

	void cCharacterController::Jump(glm::vec3 jumpVel) {
		mController->jump(nConvert::ToBullet(jumpVel));
	}
}