#include "cBallComponent.hpp"
#include "nConvert.hpp"

nPhysics::cBallComponent::cBallComponent(const sBallDef& def, const unsigned& id) 
	: iBallComponent(id) {
	btCollisionShape* shape = new btSphereShape(def.Radius);

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(nConvert::ToBullet(def.Position));

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

nPhysics::cBallComponent::~cBallComponent() {
	mBody->setUserPointer(0);
	delete mBody->getCollisionShape();
	delete mBody->getMotionState();
	delete mBody;
}

bool nPhysics::cBallComponent::GetVelocity(glm::vec3& velocityOut) {
	//if (mRigidBody->IsStatic()) return false;
	velocityOut = nConvert::ToSimple(mBody->getLinearVelocity());
	return true;
}

void nPhysics::cBallComponent::GetTransform(glm::mat4& transformOut) {
	btTransform transform;
	mBody->getMotionState()->getWorldTransform(transform);
	nConvert::ToSimple(transform, transformOut);
}

void nPhysics::cBallComponent::SetTransform(glm::mat4 TransformIn) {
	btTransform transformOut;
	nConvert::ToBullet(TransformIn, transformOut);
	mBody->setCenterOfMassTransform(transformOut);
}

void nPhysics::cBallComponent::SetPosition(glm::vec3 positionIn) {
	mBody->setCenterOfMassTransform(btTransform(btMatrix3x3(), nConvert::ToBullet(positionIn)));
	//mBody->setLinearVelocity(btVector3(btScalar(0), btScalar(0), btScalar(0)));
	mBody->setAngularVelocity(nConvert::ToBullet(glm::vec3(0.f, mBody->getAngularVelocity().y(), 0.f)));
}

void nPhysics::cBallComponent::ApplyImpulse(glm::vec3 impulseIn) {
	mBody->activate(true);
	mBody->applyCentralImpulse(nConvert::ToBullet(impulseIn));
	//mBody->applyCentralForce(nConvert::ToBullet(impulseIn));
}

void nPhysics::cBallComponent::Stop() {
	mBody->setLinearVelocity(nConvert::ToBullet(glm::vec3(0.f)));
}

bool nPhysics::cBallComponent::GetBuildInfo(glm::vec3& position, glm::vec3& velocity, float& radius, float& mass) {
	// Rigid Body Data Members
	position = nConvert::ToSimple(mBody->getCenterOfMassPosition());
	velocity = nConvert::ToSimple(mBody->getLinearVelocity());
	mass = mBody->getMass();

	// CollisionShape Data Members
	radius = ((btSphereShape*)mBody->getCollisionShape())->getRadius();

	return true;
}
