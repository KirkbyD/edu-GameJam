#include "cBoxComponent.hpp"
#include "nConvert.hpp"

namespace nPhysics {
	cBoxComponent::cBoxComponent(const sBoxDef& def, const unsigned& id)
		: iBoxComponent(id)
	{
		btCollisionShape* shape = new btBoxShape(nConvert::ToBullet(def.HalfExtents));

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(nConvert::ToBullet(def.Position));
		transform.setRotation(nConvert::ToBullet(def.Rotation));

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

	cBoxComponent::~cBoxComponent()	{
		mBody->setUserPointer(0);
		delete mBody->getCollisionShape();
		delete mBody->getMotionState();
		delete mBody;
	}

	bool cBoxComponent::GetVelocity(glm::vec3& velocityOut)
	{
		velocityOut = nConvert::ToSimple(mBody->getLinearVelocity());
		return true;
	}

	void cBoxComponent::GetTransform(glm::mat4& transformOut) {
		btTransform transform;
		mBody->getMotionState()->getWorldTransform(transform);
		nConvert::ToSimple(transform, transformOut);
	}

	void cBoxComponent::SetTransform(glm::mat4 TransformIn) {
		btTransform transformOut;
		nConvert::ToBullet(TransformIn, transformOut);
		mBody->setCenterOfMassTransform(transformOut);
	}

	void cBoxComponent::ApplyImpulse(glm::vec3 impulseIn) {
		mBody->activate(true);
		mBody->applyCentralImpulse(nConvert::ToBullet(impulseIn));
	}

	void cBoxComponent::Stop() {
		mBody->setLinearVelocity(nConvert::ToBullet(glm::vec3(0.f)));
	}

	bool cBoxComponent::GetBuildInfo(glm::vec3& position, glm::vec3& velocity, float& radius, float& mass)
	{
		return false;
	}

	void nPhysics::cBoxComponent::SetPosition(glm::vec3 positionIn)
	{
		mBody->setCenterOfMassTransform(btTransform(btMatrix3x3(), nConvert::ToBullet(positionIn)));
		mBody->setAngularVelocity(nConvert::ToBullet(glm::vec3(0.f, mBody->getAngularVelocity().y(), 0.f)));
	}
}