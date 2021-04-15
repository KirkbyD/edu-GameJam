#pragma once
#include <physics/interfaces/iPhysicsWorld.h>
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "btBulletDynamicsCommon.h"
#include <vector>
#include "cWrapperCollisionListener.hpp"
#include "cWrapperDebugRenderer.hpp"

namespace nPhysics {
	class cPhysicsWorld : public iPhysicsWorld {
	private:
		btDefaultCollisionConfiguration* mCollisionConfiguration;
		cWrapperCollisionListener* mDispatcher;
		btBroadphaseInterface* mOverlappingPairCache;
		btGhostPairCallback* mGhostPairCallback;
		btSequentialImpulseConstraintSolver* mSolver;
		btDiscreteDynamicsWorld* mDynamicsWorld;

		std::vector<btCollisionShape*> mShapeVector;

		cWrapperDebugRenderer* mDebugRenderer;
		bool DrawDebug;

	public:
		cPhysicsWorld();
		virtual ~cPhysicsWorld();

		virtual void Update(float dt);
		virtual bool AddComponent(iPhysicsComponent* component);
		virtual bool RemoveComponent(iPhysicsComponent* component);
		virtual void SetGravity(glm::vec3 gravity);
		virtual void SetCollisionListener(iCollisionListener* listener);
		virtual void SetDebugRenderer(iDebugRenderer* renderer);

		// Inherited via iPhysicsWorld
		virtual void SetWind(glm::vec3 wind) override;
		virtual glm::vec3 GetWind() override;
	};
}