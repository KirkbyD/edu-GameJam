#include "cPhysicsWorld.hpp"
#include <physics/interfaces/eComponentType.h>
#include "cBallComponent.hpp"
#include "cPlaneComponent.hpp"
#include "cBoxComponent.hpp"
#include "cCapsuleComponent.hpp"
#include "cTriangleMeshComponent.hpp"
#include "cCharacterController.hpp"
#include "nConvert.hpp"

nPhysics::cPhysicsWorld::cPhysicsWorld() : mDebugRenderer(nullptr), DrawDebug(false) {
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	mCollisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	mDispatcher = new cWrapperCollisionListener(nullptr, mCollisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	mOverlappingPairCache = new btDbvtBroadphase();

	//Enable ghost object functionality!
	mGhostPairCallback = new btGhostPairCallback();
	mOverlappingPairCache->getOverlappingPairCache()->setInternalGhostPairCallback(mGhostPairCallback);

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	mSolver = new btSequentialImpulseConstraintSolver;

	mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mOverlappingPairCache, mSolver, mCollisionConfiguration);
	
	//Initialise gravity to 0
	mDynamicsWorld->setGravity(btVector3(0, 0, 0));
}

nPhysics::cPhysicsWorld::~cPhysicsWorld() {
	//Remove the rigidbodies from the dynamics world and delete them
	for (int i = mDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)	{
		btCollisionObject* obj = mDynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState()) {
			delete body->getMotionState();
		}
		mDynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	//Delete order very specific.
	//taken from the Bullet's hello world example
	delete mDynamicsWorld;
	delete mSolver;
	delete mOverlappingPairCache;
	delete mGhostPairCallback;
	delete mDispatcher;
	delete mCollisionConfiguration;
}

void nPhysics::cPhysicsWorld::Update(float dt) {
	//Handle collision listener inside bullet via custom mDispatcher.
	mDynamicsWorld->stepSimulation(dt, 10);

	if(DrawDebug)
		mDynamicsWorld->debugDrawWorld();
}

bool nPhysics::cPhysicsWorld::AddComponent(iPhysicsComponent* component) {
	if (!component) {
		return false;
	}

	switch (component->GetComponentType()) {
	case eComponentType::ball:
		mDynamicsWorld->addRigidBody(((cBallComponent*)component)->mBody);
		mShapeVector.push_back(((cBallComponent*)component)->mBody->getCollisionShape());
		return true;
	case eComponentType::box:
		mDynamicsWorld->addRigidBody(((cBoxComponent*)component)->mBody);
		mShapeVector.push_back(((cBoxComponent*)component)->mBody->getCollisionShape());
		return true;
	case eComponentType::capsule:
		mDynamicsWorld->addRigidBody(((cCapsuleComponent*)component)->mBody);
		mShapeVector.push_back(((cCapsuleComponent*)component)->mBody->getCollisionShape());
		return true;
	case eComponentType::plane:
		mDynamicsWorld->addRigidBody(((cPlaneComponent*)component)->mBody);
		mShapeVector.push_back(((cPlaneComponent*)component)->mBody->getCollisionShape());
		return true;
	case eComponentType::triangleMesh:
		mDynamicsWorld->addRigidBody(((cTriangleMeshComponent*)component)->mBody);
		mShapeVector.push_back(((cTriangleMeshComponent*)component)->mBody->getCollisionShape());
		return true;
	case eComponentType::characterController:
		mDynamicsWorld->addCollisionObject(((cCharacterController*)component)->mGhostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
		mDynamicsWorld->addAction(((cCharacterController*)component)->mController);
		((cCharacterController*)component)->mController->setGravity(mDynamicsWorld->getGravity() * 3.f);
		return true;
	default:
		return false;
	}
}

bool nPhysics::cPhysicsWorld::RemoveComponent(iPhysicsComponent* component) {
	switch (component->GetComponentType()) {
	case eComponentType::ball:
		mDynamicsWorld->removeRigidBody(((cBallComponent*)component)->mBody);
		return true;
	case eComponentType::box:
		mDynamicsWorld->removeRigidBody(((cBoxComponent*)component)->mBody);
		return true;
	case eComponentType::capsule:
		mDynamicsWorld->removeRigidBody(((cCapsuleComponent*)component)->mBody);
		return true;
	case eComponentType::plane:
		mDynamicsWorld->removeRigidBody(((cPlaneComponent*)component)->mBody);
		return true;
	case eComponentType::triangleMesh:
		mDynamicsWorld->removeRigidBody(((cTriangleMeshComponent*)component)->mBody);
		return true;
	case eComponentType::characterController:
		mDynamicsWorld->removeCollisionObject(((cCharacterController*)component)->mGhostObject);
		mDynamicsWorld->removeAction(((cCharacterController*)component)->mController);
		return true;
	default:
		return false;
	}
}

void nPhysics::cPhysicsWorld::SetGravity(glm::vec3 gravity) {
	mDynamicsWorld->setGravity(nConvert::ToBullet(gravity));
}

void nPhysics::cPhysicsWorld::SetCollisionListener(iCollisionListener* listener) {
	mDispatcher->SetListener(listener);
}

void nPhysics::cPhysicsWorld::SetDebugRenderer(iDebugRenderer* renderer) {
	if (renderer == nullptr) {
		DrawDebug = false;
		mDynamicsWorld->setDebugDrawer(nullptr);
	}
	else {
		mDebugRenderer = new cWrapperDebugRenderer(renderer);
		mDynamicsWorld->setDebugDrawer(mDebugRenderer);
		DrawDebug = true;
	}
}

void nPhysics::cPhysicsWorld::SetWind(glm::vec3 wind)
{
	//todo?
	return;
}

glm::vec3 nPhysics::cPhysicsWorld::GetWind()
{
	//TODO?
	return glm::vec3(0.f);
}
