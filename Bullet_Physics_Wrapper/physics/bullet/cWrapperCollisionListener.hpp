#pragma once
#include <physics/interfaces/iCollisionListener.h>
#include "BulletCollision/CollisionDispatch/btCollisionDispatcher.h"

namespace nPhysics {
	//Concrete class for library level iPhysCollisionListener.
	//Holds pointer to interface level interface for dispatch to game code.
	class cWrapperCollisionListener : public btCollisionDispatcher {
	private:
		iCollisionListener* mCollisionListener;

	public:
		//Constructor
		//iCollisionListener is stored and used, but never deleted.
		cWrapperCollisionListener(iCollisionListener* collisionListener, btCollisionConfiguration* collisionConfiguration);

		//Destructor
		virtual ~cWrapperCollisionListener() {
			mCollisionListener = 0;
		}

		void SetListener(iCollisionListener* listenerIn) { mCollisionListener = listenerIn; }

		virtual void dispatchAllCollisionPairs(btOverlappingPairCache* pairCache, const btDispatcherInfo& dispatchInfo, btDispatcher* dispatcher);
	};
}