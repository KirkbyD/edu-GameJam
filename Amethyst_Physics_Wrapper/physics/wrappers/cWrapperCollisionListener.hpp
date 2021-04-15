#pragma once
#include <physics/interfaces/iCollisionListener.h>
#include <iPhysCollisionListener.hpp>

namespace nPhysics {
	//Concrete class for library level iPhysCollisionListener.
	//Holds pointer to interface level interface for dispatch to game code.
	class cWrapperCollisionListener : public phys::iPhysCollisionListener {
	private:
		iCollisionListener* mCollisionListener;

	public:
		//Constructor
		//iCollisionListener is stored and used, but never deleted.
		cWrapperCollisionListener(iCollisionListener* collisionListener) : mCollisionListener(collisionListener) { }

		//Destructor
		virtual ~cWrapperCollisionListener() {
			mCollisionListener = 0;
		}

		//todo move body into .cpp
		virtual void Collide(phys::cCollisionBody* bodyA, phys::cCollisionBody* bodyB) {
			if (!bodyA || !bodyB) {
				//cout recieved null body in a collision
			}
			//Dispatches collision to itnerface level for resolution in game code.
			mCollisionListener->Collide(reinterpret_cast<iPhysicsComponent*>(bodyA->GetUserPointer()),
										reinterpret_cast<iPhysicsComponent*>(bodyB->GetUserPointer()));
		}
	};
}