#include "cCapsuleComponent.hpp"
#include "nConvert.hpp"

namespace nPhysics {
	cCapsuleComponent::cCapsuleComponent(const sCapsuleDef& def, const unsigned& id)
		: iCapsuleComponent(id)
	{
		btCollisionShape* shape = new btCapsuleShape(btScalar(def.Radius), btScalar(def.Height));

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(nConvert::ToBullet(def.Position));
		transform.setRotation(nConvert::ToBullet(def.Rotation));

		mOrientationInv = glm::inverse(def.Rotation);

		btScalar mass(def.Mass);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic) {
			shape->calculateLocalInertia(mass, localInertia);
		}

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
		rbInfo.m_restitution = 0.2;
		mBody = new btRigidBody(rbInfo);
		mBody->setUserPointer(this);

		mBody->setLinearVelocity(nConvert::ToBullet(def.Velocity));
	}

	cCapsuleComponent::~cCapsuleComponent() {
		mBody->setUserPointer(0);
		delete mBody->getCollisionShape();
		delete mBody->getMotionState();
		delete mBody;
	}

	bool cCapsuleComponent::GetVelocity(glm::vec3& velocityOut)
	{
		velocityOut = nConvert::ToSimple(mBody->getLinearVelocity());
		return true;
	}

	void cCapsuleComponent::GetTransform(glm::mat4& transformOut) {
		btTransform transform;
		mBody->getMotionState()->getWorldTransform(transform);
		nConvert::ToSimple(transform, transformOut);
	}

	void cCapsuleComponent::SetTransform(glm::mat4 TransformIn) {
		btTransform transformOut;
		nConvert::ToBullet(TransformIn, transformOut);
		mBody->setCenterOfMassTransform(transformOut);
	}

	void cCapsuleComponent::ApplyImpulse(glm::vec3 impulseIn) {
		mBody->activate(true);
		mBody->applyCentralImpulse(nConvert::ToBullet(impulseIn));
	}

	void cCapsuleComponent::Stop() {
		mBody->setLinearVelocity(nConvert::ToBullet(glm::vec3(0.f)));
	}

	bool cCapsuleComponent::GetBuildInfo(glm::vec3& position, glm::vec3& velocity, float& radius, float& mass)
	{
		return false;
	}

	void nPhysics::cCapsuleComponent::SetPosition(glm::vec3 positionIn)
	{
		mBody->setCenterOfMassTransform(btTransform(btMatrix3x3(), nConvert::ToBullet(positionIn)));
		mBody->setAngularVelocity(nConvert::ToBullet(glm::vec3(0.f, mBody->getAngularVelocity().y(), 0.f)));
	}
}