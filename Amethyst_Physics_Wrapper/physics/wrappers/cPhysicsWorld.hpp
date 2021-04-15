#pragma once
#include <physics/interfaces/iPhysicsWorld.h>
#include <physics/interfaces/iDebugRenderer.h>
#include <cWorld.hpp>
#include "cWrapperCollisionListener.hpp"

namespace nPhysics {
	class cPhysicsWorld : public iPhysicsWorld {
	private:
		phys::cWorld* mWorld;

		cWrapperCollisionListener* mCollisionListener;
		//debug renderer
		std::vector<iPhysicsComponent*> mComponents;

	public:
		cPhysicsWorld();
		virtual ~cPhysicsWorld();

		virtual void Update(float dt);
		virtual bool AddComponent(iPhysicsComponent* component);
		virtual bool RemoveComponent(iPhysicsComponent* component);
		virtual void SetGravity(glm::vec3 gravity);
		virtual glm::vec3 GetWind();
		virtual void SetWind(glm::vec3 wind);
		virtual void SetCollisionListener(iCollisionListener* listener) override;
		virtual void SetDebugRenderer(iDebugRenderer* renderer) override;
	};
}