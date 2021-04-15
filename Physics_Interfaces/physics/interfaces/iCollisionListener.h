#pragma once
#include "iPhysicsComponent.h"
#include <vector>

namespace nPhysics {
	class iCollisionListener {
	public:
		virtual ~iCollisionListener() {};

		//Called at wrapper level, resolved in concrete class in game code.
		virtual void Collide(iPhysicsComponent* compA, iPhysicsComponent* compB) = 0;
	};
}