#include "cCollisionListener.hpp"

namespace phys {
	cCollisionListener::cCollisionListener() { }

	cCollisionListener::~cCollisionListener() {
		ClearCollisions();
	}

	void cCollisionListener::AddCollision(cRigidBody* bodyA, cRigidBody* bodyB)	{
		m_CollidedObjects.push_back(std::make_pair(bodyA, bodyB));
	}

	bool cCollisionListener::GetCollisions(std::vector<std::pair<cRigidBody*, cRigidBody*>>& collisionsOut) {
		if (m_CollidedObjects.empty()) return false;

		collisionsOut = m_CollidedObjects;
		return true;
	}

	void cCollisionListener::ClearCollisions() {
		for (auto it : this->m_CollidedObjects) {
			it.first = nullptr;
			it.second = nullptr;
		}
		m_CollidedObjects.clear();
	}
}