#include "cPhysicsWorld.hpp"
#include <physics/interfaces/eComponentType.h>
#include "cBallComponent.hpp"
#include "cPlaneComponent.hpp"
#include "cClothComponent.hpp"

namespace nPhysics {
	cPhysicsWorld::cPhysicsWorld() {
		mWorld = new phys::cWorld();
		mCollisionListener = 0;
	}

	cPhysicsWorld::~cPhysicsWorld() {
		delete mWorld;
		delete mCollisionListener;
	}

	void cPhysicsWorld::Update(float dt) {
		mWorld->Update(dt);

		// Get collisions
		//mCollisionListener->ClearInterfaceLevelCollisions();
		//std::vector<std::pair<phys::cRigidBody*, phys::cRigidBody*>> collisions = mCollisionListener->GetLibraryLevelCollisions();
		//// Convert Collisions
		//for (auto coll : collisions) {
		//	sCollisionInfo collInfo;

		//	for (auto component : this->mComponents) {
		//		switch (component->GetComponentType()) {
		//		case eComponentType::ball:
		//			if (((cBallComponent*)component)->mRigidBody == coll.first) {
		//				collInfo.compA = component;
		//			}
		//			else if (((cBallComponent*)component)->mRigidBody == coll.second) {
		//				collInfo.compB = component;
		//			}
		//			break;
		//		case eComponentType::plane:
		//			if (((cPlaneComponent*)component)->mRigidBody == coll.first) {
		//				collInfo.compA = component;
		//			}
		//			else if (((cPlaneComponent*)component)->mRigidBody == coll.second) {
		//				collInfo.compB = component;
		//			}
		//			break;
		//		default:
		//			break;
		//		}
		//		if (collInfo.compA != nullptr && collInfo.compB != nullptr)
		//			break;
		//	}
		//	mCollisionListener->AddInterfaceLevelCollision(collInfo);
		//}

		//report collisions
		//upcast rigid bodies back to i physics components to update colision listener...
	}

	bool cPhysicsWorld::AddComponent(iPhysicsComponent* component) {
		switch (component->GetComponentType()) {
		case eComponentType::ball:
			mComponents.push_back(component);
			return mWorld->AddBody(((cBallComponent*)component)->mRigidBody);
		case eComponentType::plane:
			mComponents.push_back(component);
			return mWorld->AddBody(((cPlaneComponent*)component)->mRigidBody);
		case eComponentType::cloth:
			mComponents.push_back(component);
			return mWorld->AddBody(((cClothComponent*)component)->mBody);
		default:
			return false;
		}
	}

	bool cPhysicsWorld::RemoveComponent(iPhysicsComponent* component) {
		switch (component->GetComponentType()) {
		case eComponentType::ball:
			//mComponents.push_back(component); 
			// find component and remove form here ^
			return mWorld->RemoveBody(((cBallComponent*)component)->mRigidBody);
		case eComponentType::plane:
			return mWorld->RemoveBody(((cPlaneComponent*)component)->mRigidBody);
		case eComponentType::cloth:
			return mWorld->RemoveBody(((cClothComponent*)component)->mBody);
		default:
			return false;
		}
	}

	void cPhysicsWorld::SetGravity(glm::vec3 gravity) {
		mWorld->SetGravity(gravity);
	}

	glm::vec3 cPhysicsWorld::GetWind() {
		glm::vec3 windOut;
		mWorld->GetWind(windOut);
		return windOut;
	}

	void cPhysicsWorld::SetWind(glm::vec3 wind)	{
		mWorld->SetWind(wind);
	}

	void cPhysicsWorld::SetCollisionListener(iCollisionListener* listener) {
		if (listener == nullptr)
			mCollisionListener = 0;
		else {
			mCollisionListener = new cWrapperCollisionListener(listener);
			mWorld->SetCollisionListener(mCollisionListener);
		}
	}

	void cPhysicsWorld::SetDebugRenderer(iDebugRenderer* renderer) {
		return;
	}
}