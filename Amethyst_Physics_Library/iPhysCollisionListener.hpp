#pragma once
#include "cCollisionBody.hpp"

namespace phys {
	// iPhysCollisionListener
	// Interface level, chain back to the game code the opposite directin of the rest of the library.
	// Here -> wrapper level concrete -> interface level interface -> game code concrete
	class iPhysCollisionListener {
	public:
		virtual void Collide(cCollisionBody* bodyA, cCollisionBody* bodyB) = 0;
	};
}