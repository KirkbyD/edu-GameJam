#include "cPlaneComponent.hpp"
#include <nConvert.hpp>

nPhysics::cPlaneComponent::cPlaneComponent(const sPlaneDef& def, const unsigned& id)
	: iPlaneComponent(id) {
	btCollisionShape* shape = new btStaticPlaneShape(nConvert::ToBullet(def.Normal), def.Constant);

	btTransform transform;
	transform.setIdentity();

	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0.f, myMotionState, shape, btVector3(0.f, 0.f, 0.f));
	rbInfo.m_restitution = 1.0f;
	mBody = new btRigidBody(rbInfo);
	mBody->setUserPointer(this);
}

nPhysics::cPlaneComponent::~cPlaneComponent() {
	mBody->setUserPointer(0);
	delete mBody->getCollisionShape();
	delete mBody->getMotionState();
	delete mBody;
}

void nPhysics::cPlaneComponent::GetTransform(glm::mat4& transformOut) {
	btTransform transform;
	mBody->getMotionState()->getWorldTransform(transform);
	nConvert::ToSimple(transform, transformOut);
}

void nPhysics::cPlaneComponent::SetTransform(glm::mat4 TransformIn) {
	//TODO
}

bool nPhysics::cPlaneComponent::GetBuildInfo(glm::vec3& normal, float& constant) {
	// CollisionShape Data Members
	normal = nConvert::ToSimple(((btStaticPlaneShape*)mBody->getCollisionShape())->getPlaneNormal());
	constant = ((btStaticPlaneShape*)mBody->getCollisionShape())->getPlaneConstant();

	return true;
}

void nPhysics::cPlaneComponent::SetPosition(glm::vec3 positionIn)
{
}

// implement later if planes need to move
bool nPhysics::cPlaneComponent::GetVelocity(glm::vec3& velocityOut) { return false; }
