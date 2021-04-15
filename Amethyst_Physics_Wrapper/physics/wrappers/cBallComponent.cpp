#include "cBallComponent.hpp"

nPhysics::cBallComponent::cBallComponent(const sBallDef& def, const unsigned& id)
	: iBallComponent(id)
	, mBall(new phys::cSphere(def.Radius)) {
	phys::sRigidBodyDef rbDef(def.Mass, def.Position, def.Velocity);
	mRigidBody = new phys::cRigidBody(rbDef, mBall);
	mRigidBody->SetUserPointer(this);
}

bool nPhysics::cBallComponent::GetVelocity(glm::vec3& velocityOut) {
	if (mRigidBody->IsStatic()) return false;
	mRigidBody->GetVelocity(velocityOut);
	return true;
}

void nPhysics::cBallComponent::GetTransform(glm::mat4& transformOut) {
	mRigidBody->GetTransform(transformOut);
}

void nPhysics::cBallComponent::ApplyImpulse(glm::vec3 impulseIn) {
	mRigidBody->ApplyImpulse(impulseIn);
	//TODO - make this function instead
	//mRigidBody->ApplyForce(impulseIn);
}

void nPhysics::cBallComponent::Stop() {
	//todo
}

bool nPhysics::cBallComponent::GetBuildInfo(glm::vec3& position, glm::vec3& velocity, float& radius, float& mass) {
	glm::mat4 transform;
	mRigidBody->GetTransform(transform);
	position = transform[3];
	mRigidBody->GetVelocity(velocity);
	mRigidBody->GetMass(mass);
	radius = mBall->GetRadius();
	return false;
}

void nPhysics::cBallComponent::SetTransform(glm::mat4 TransformIn)
{
	//TODO
	return;
}

void nPhysics::cBallComponent::SetPosition(glm::vec3 positionIn)
{
}
