#include "cTriangleMeshComponent.hpp"
#include <nConvert.hpp>

nPhysics::cTriangleMeshComponent::cTriangleMeshComponent(const sTriangleMeshDef& def, const unsigned& id)
	: iTriangleMeshComponent(id) {
	btTriangleMesh* mesh = new btTriangleMesh();
	nConvert::ToBullet(def.Mesh, def.Scale, def.Position, def.Rotation, mesh);
	mMesh = new btBvhTriangleMeshShape(mesh, false);
	btCollisionShape* shape = mMesh;

	btTransform transform;
	transform.setIdentity();

	// Since btBvhTriangleMeshShape is set to to origin, all ransforms are 
	// handled in the mesh conversion function.
	// Store them for purposes of getting transform back out. could avoid this
	// in game code by checking for type.
	mOffsetPosition = nConvert::ToBullet(def.Position);
	mOffsetRotation = nConvert::ToBullet(def.Rotation);

	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0.f, myMotionState, shape, btVector3(0.f, 0.f, 0.f));
	rbInfo.m_restitution = 1.0f;
	mBody = new btRigidBody(rbInfo);
	mBody->setUserPointer(this);
}

nPhysics::cTriangleMeshComponent::~cTriangleMeshComponent() {
	mBody->setUserPointer(0);
	delete mBody->getCollisionShape();
	delete mBody->getMotionState();
	delete mBody;
}

void nPhysics::cTriangleMeshComponent::GetTransform(glm::mat4& transformOut) {
	btTransform transform;
	mBody->getMotionState()->getWorldTransform(transform);
	transform.setOrigin(mOffsetPosition);
	transform.setRotation(mOffsetRotation);
	nConvert::ToSimple(transform, transformOut);
}

void nPhysics::cTriangleMeshComponent::SetTransform(glm::mat4 TransformIn) {
	//TODO
}

bool nPhysics::cTriangleMeshComponent::GetBuildInfo(glm::vec3& normal, float& constant) {
	// CollisionShape Data Members
	//Keep the string name of my mesh maybe?
	//todo
	//normal = nConvert::ToSimple(((btStaticTriangleMeshShape*)mBody->getCollisionShape())->getTriangleMeshNormal());
	//constant = ((btStaticTriangleMeshShape*)mBody->getCollisionShape())->getTriangleMeshConstant();

	return false;
}

void nPhysics::cTriangleMeshComponent::SetPosition(glm::vec3 positionIn)
{
}

// implement later if TriangleMeshs need to move
bool nPhysics::cTriangleMeshComponent::GetVelocity(glm::vec3& velocityOut) { return false; }
