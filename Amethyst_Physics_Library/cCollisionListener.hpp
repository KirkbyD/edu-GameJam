#pragma once
#include <vector>
#include "cRigidBody.hpp"

namespace phys {
	class cCollisionListener {
	private:
		//something to hold collisions
		std::vector<std::pair<cRigidBody*, cRigidBody*>> m_CollidedObjects;

		// Constructors not to be used.
		cCollisionListener(const cCollisionListener& other) = delete;
		cCollisionListener& operator=(const cCollisionListener& other) = delete;

	public:
		cCollisionListener();
		~cCollisionListener();

		void AddCollision(cRigidBody* bodyA, cRigidBody* bodyB);
		bool GetCollisions(std::vector<std::pair<cRigidBody*, cRigidBody*>>& collisionsOut);
		void ClearCollisions();
	};
}