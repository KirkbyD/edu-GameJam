#pragma once
#include "cCollisionBody.hpp"
#include "cRigidBody.hpp"
#include "cSoftBody.hpp"
#include "Shapes/cSphere.hpp"
#include "Shapes/cPlane.hpp"
#include "cIntegrator.hpp"
#include "iPhysCollisionListener.hpp"
#include "iPhysDebugRenderer.hpp"
#include <vector>

namespace phys {
	// cWorld
	// A rigid body simulator.
	// 
	// Pointers to cRigidBody instances can be added and removed.
	// 
	// cWorld will operate on the contained cRigidBody's during timesteps
	// triggered by calls to cWorld::Update(dt)
	// 
	// cWorld does not own cRigidBody pointers, and will not
	// delete them when it is deleted.
	class cWorld {
	private:
		// Acceleration due to gravity applied to each collision body
		// during each timestep.
		glm::vec3 mGravity;
		// Acceleration due to wind applied to each soft body
		// during each timestep.
		glm::vec3 mWind;
		// All the rigid bodies currently in the world.
		// Not owned by cWorld, will not be deleted in the destructor.
		std::vector<cCollisionBody*> mBodies;
		// The delta time input cached during each Update(dt) call.
		float mDt;

		// Our handy dandy integrator.
		cIntegrator mIntegrator;

		iPhysCollisionListener* mCollisionListener;
		iPhysDebugRenderer* mDebugRenderer;

		// Constructors not to be used.
		cWorld(const cWorld& other) = delete;
		cWorld& operator=(const cWorld& other) = delete;

	protected:
		//Integrate Body()
		//Entry point for a single step of integration
		//Determines body types and dispatches appropriate functions
		void IntegrateBody(cCollisionBody* body, float dt);

		// IntegrateRigidBody
		// Performs a single numerical integration step.
		// Safe for null pointers.
		// Safe for static rigid bodies.
		void IntegrateRigidBody(cRigidBody* body, float dt);

		// Collide
		// Entry point for collision detection.
		// Returns the result of specific mthods.
		bool Collide(cCollisionBody* bodyA, cCollisionBody* bodyB);
		
		// Collide
		// Entry point for collision detection of two rigid bodies.
		// Returns the result of specific methods.
		bool Collide(cRigidBody* bodyA, cRigidBody* bodyB);

		bool CollideSoftRigid(cSoftBody* softBody, cRigidBody* rigidBody);

		// CollideSpherePlane
		// Handles collision detection and reaction between a sphere and a plane.
		// Returns true if a collision occured.
		// Returns false if no collision occured.
		bool CollideSpherePlane(cRigidBody* sphereBody, cSphere* sphereShape, cRigidBody* planeBody, cPlane* planeShape);

		// CollideSphereSphere
		// Handles collision detection and reaction between two spheres.
		// Returns true if a collision occured.
		// Returns false if no collision occured.
		bool CollideSphereSphere(cRigidBody* sphereBodyA, cSphere* sphereShapeA, cRigidBody* sphereBodyB, cSphere* sphereShapeB);


		bool CollideSoftBodySphere(cSoftBody* softBody, cRigidBody* sphereBody, cSphere* sphereShape);
		bool CollideSoftBodyPlane(cSoftBody* softBody, cRigidBody* sphereBody, cPlane* sphereShape);


		void DrawDebug();

	public:
		// Create a default cWorld
		// The world is initially empty, containing no rigid bodies.
		// The world has no gravity.
		cWorld();

		~cWorld() {}

		//Gravity get/set
		void GetGravity(glm::vec3& gravityOut);
		void SetGravity(glm::vec3& gravityIn);
		
		//Wind get/set
		void GetWind(glm::vec3& windOut);
		void SetWind(glm::vec3& windIn);
		void MutateWind();

		void SetCollisionListener(iPhysCollisionListener* listener) { mCollisionListener = listener; }
		void SetDebugRenderer(iPhysDebugRenderer* renderer) { mDebugRenderer = renderer; }

		void Update(float dt);

		// helpful comment like what does the return bool mean?
		bool AddBody(cCollisionBody* body);
		bool RemoveBody(cCollisionBody* body);

		//todo
		//void DrawDebug();
	};
}