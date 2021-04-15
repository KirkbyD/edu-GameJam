#include "cWrapperCollisionListener.hpp"
#include "BulletCollision/BroadphaseCollision/btOverlappingPairCache.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include <btBulletCollisionCommon.h>

///interface for iterating all overlapping collision pairs, no matter how those pairs are stored (array, set, map etc)
///this is useful for the collision dispatcher.
///Found in the bullet library as part of btCollisionDispatcher.cpp
class btCollisionPairCallback : public btOverlapCallback
{
	const btDispatcherInfo& m_dispatchInfo;
	btCollisionDispatcher* m_dispatcher;

public:
	btCollisionPairCallback(const btDispatcherInfo& dispatchInfo, btCollisionDispatcher* dispatcher)
		: m_dispatchInfo(dispatchInfo),
		m_dispatcher(dispatcher)
	{
	}

	virtual ~btCollisionPairCallback() {}

	virtual bool processOverlap(btBroadphasePair& pair)
	{
		(*m_dispatcher->getNearCallback())(pair, *m_dispatcher, m_dispatchInfo);
		return false;
	}
};

namespace nPhysics {
	cWrapperCollisionListener::cWrapperCollisionListener(iCollisionListener* collisionListener, btCollisionConfiguration* collisionConfiguration)
		: mCollisionListener(collisionListener)
		, btCollisionDispatcher(collisionConfiguration)
	{ }

	void cWrapperCollisionListener::dispatchAllCollisionPairs(btOverlappingPairCache* pairCache, const btDispatcherInfo& dispatchInfo, btDispatcher* dispatcher) {
		btBroadphasePairArray pairs;
		if (mCollisionListener != nullptr)
			pairs = pairCache->getOverlappingPairArray();

		btCollisionPairCallback collisionCallback(dispatchInfo, this);
		BT_PROFILE("processAllOverlappingPairs");
		pairCache->processAllOverlappingPairs(&collisionCallback, dispatcher, dispatchInfo);
		
		// Combined my original listener with a manifold pair check.
		// This avoids my ghost object character controller reporting 
		// collisions with every other object, every frame!
		for (size_t pairIt = 0; pairIt < pairs.size(); pairIt++) {
			btManifoldArray ManifoldArray;

			if (pairs[pairIt].m_algorithm)
			{
				pairs[pairIt].m_algorithm->getAllContactManifolds(ManifoldArray);
			}

			for (int manIt = 0; manIt < ManifoldArray.size(); manIt++)
			{
				for (int contIt = 0; contIt < ManifoldArray[manIt]->getNumContacts(); contIt++)
				{
					const btManifoldPoint& Point = ManifoldArray[manIt]->getContactPoint(contIt);

					if (Point.getDistance() < 2.f) // Was at 0.f, made larger for a better tolerance
					{
						btRigidBody* bodyA = reinterpret_cast<btRigidBody*>(pairs[pairIt].m_pProxy0->m_clientObject);
						btRigidBody* bodyB = reinterpret_cast<btRigidBody*>(pairs[pairIt].m_pProxy1->m_clientObject);

						if (!bodyA || !bodyB) {
							//cout recieved null body in a collision
						}

						//Dispatches collision to interface level for resolution in game code.
						else
							mCollisionListener->Collide(reinterpret_cast<iPhysicsComponent*>(bodyA->getUserPointer()),
								reinterpret_cast<iPhysicsComponent*>(bodyB->getUserPointer()));
					}
				}
			}
		}
		return;
	}
}