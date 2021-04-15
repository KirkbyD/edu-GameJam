#include "cRigidBody.hpp"
#include <glm\ext\matrix_transform.hpp>


phys::cRigidBody::cRigidBody(const sRigidBodyDef& def, iShape* shape)
    : cCollisionBody(eBodyType::rigid)
    , mShape(shape)
    , mPosition(def.Position)
    , mVelocity(def.Velocity)
    , mMaxVelocity(def.MaxVelocity)
    , mMass(def.Mass)
    , mAcceleration(0.f)
    , mPreviousPosition(def.Position) {
    if (mMass == 0.f) {
        mInvMass = 0.f;
    }
    else {
        mInvMass = 1.f / mMass;
    }
}

phys::cRigidBody::~cRigidBody() {
    //Don't delete shape here, world handles that...
}

void phys::cRigidBody::GetVelocity(glm::vec3& velocityOut) {
	velocityOut = mVelocity;
}

void phys::cRigidBody::GetTransform(glm::mat4& transformOut) {
    transformOut = glm::mat4(1.f);
    // rotations
    transformOut = glm::translate(transformOut, mPosition);
}

void phys::cRigidBody::GetMass(float& mass) {
    mass = mMass;
}

void phys::cRigidBody::ApplyForce(const glm::vec3& force) {
    // f = m a
    // 1/m F = a
    mAcceleration += force * mInvMass;
}

void phys::cRigidBody::ApplyImpulse(const glm::vec3& impulse) {
    mVelocity += impulse;
}

void phys::cRigidBody::ClearAccelerations() {
    mAcceleration = glm::vec3(0.f);
}
