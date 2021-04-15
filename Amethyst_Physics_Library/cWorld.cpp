#include "cWorld.hpp"
#include "nCollide.hpp"
#include <glm\gtx\projection.hpp>
#include <glm\gtc\random.hpp>

namespace phys {
	cWorld::cWorld()
		: mDt(0.f)
		, mGravity(glm::vec3(0.f, 0.f, 0.f)) 
		, mWind(glm::vec3(-4.f, 0.f, 0.f)) 
		, mCollisionListener(nullptr)
		, mDebugRenderer(nullptr)
	{

	}

	void cWorld::GetGravity(glm::vec3& gravityOut) {
		gravityOut = mGravity;
	}

	void cWorld::SetGravity(glm::vec3& gravityIn) {
		mGravity = gravityIn;
	}

	void cWorld::GetWind(glm::vec3& windOut) {
		windOut = mWind;
	}

	void cWorld::SetWind(glm::vec3& windIn)	{
		mWind = windIn;
	}

	//Randomizes wind direction gradually using its current direction as the mean for gaussian distribution.
	void cWorld::MutateWind() {
		mWind = glm::gaussRand(mWind, glm::vec3(1.f));
		//Cap speeds
		if (mWind.x > 10.f) mWind.x = 10.f;
		else if (mWind.x < -10.f) mWind.x = -10.f;
		if (mWind.y > 15.f) mWind.y = 15.f;
		else if (mWind.y < 0.f) mWind.y = 0.f;
		if (mWind.z > 10.f) mWind.z = 10.f;
		else if (mWind.z < -10.f) mWind.z = -10.f;
	}

	void cWorld::Update(float dt) {
		size_t numBodies = mBodies.size();
		if (numBodies == 0) return;

		if(mWind != glm::vec3(0.f))
			MutateWind();

		// Step 1: Integrate
		mDt = dt;
		for (size_t i = 0; i < numBodies; i++) {
			IntegrateBody(mBodies[i], mDt);
		}

		// Step 2: Handle Collisions
		//SUPER SIMPLE, NOT GREAT
		//collision listener collector vctor, pair collisins bodies
		std::vector<std::pair<cCollisionBody*, cCollisionBody*>> collisions;
		for (size_t idxA = 0; idxA < numBodies - 1; idxA++) {
			for (size_t idxB = idxA + 1; idxB < numBodies; idxB++) {
				//this structure hits all unique pairs once!
				if (Collide(mBodies[idxA], mBodies[idxB])) {
					collisions.push_back(std::make_pair(mBodies[idxA], mBodies[idxB]));
				}
			}
		}

		// Step 3: Clear the accelerations for all rigid bodies.
		for (size_t i = 0; i < numBodies; i++) {
			mBodies[i]->ClearAccelerations();

			//Check Terminal Velocity
			//if (mBodies[i]->mMaxVelocity.y != 0.f 
			//	&& mBodies[i]->mMaxVelocity.y < fabs(mBodies[i]->mVelocity.y)) {
			//	//exceeding terminal velocity
			//	mBodies[i]->mVelocity.y = mBodies[i]->mMaxVelocity.y * glm::sign(mBodies[i]->mVelocity.y);
			//}

			//XZ velocity max
			/*if (mBodies[i]->mVelocity.x != 0 && mBodies[i]->mVelocity.z != 0) {
				float xzVel = glm::length(glm::vec2(mBodies[i]->mVelocity.x, mBodies[i]->mVelocity.z));
				float xzMax = glm::length(glm::vec2(mBodies[i]->mMaxVelocity.x, mBodies[i]->mMaxVelocity.z));
				if (xzVel > xzMax) {
					float multiplier = xzMax / xzVel;
					mBodies[i]->mVelocity.x *= multiplier;
					mBodies[i]->mVelocity.z *= multiplier;
				}
			}*/
		}

		// Step 4: Tell everyone about the collisions
		if (mCollisionListener) {
			for (size_t i = 0; i < collisions.size(); i++) {
				mCollisionListener->Collide(collisions[i].first, collisions[i].second);
			}
		}
	}

	bool cWorld::AddBody(cCollisionBody* body) {
		if (!body) return false; //can't add nothing
		std::vector<cCollisionBody*>::iterator itBody = std::find(mBodies.begin(), mBodies.end(), body);
		if (itBody == mBodies.end()) {
			//not already there, so add it
			mBodies.push_back(body);
			return true;
		}
		return false;
	}

	bool cWorld::RemoveBody(cCollisionBody* body) {
		if (!body) return false; //can't remove nothing
		std::vector<cCollisionBody*>::iterator itBody = std::find(mBodies.begin(), mBodies.end(), body);
		if (itBody == mBodies.end()) return false; //wasn't here, wasn't removed
		mBodies.erase(itBody);
		return true;
	}

	void cWorld::IntegrateBody(cCollisionBody* body, float dt) {
		//figure out what kind of body it is
		//pass it on to the right integrator method
		if (body->GetBodyType() == eBodyType::rigid) {
			IntegrateRigidBody(dynamic_cast<cRigidBody*>(body), dt);
		}
		else if (body->GetBodyType() == eBodyType::soft) {
			//Soft bodies take in wind as well as gravity
			dynamic_cast<cSoftBody*>(body)->Integrate(dt, mGravity+mWind);
		}
	}

	void cWorld::IntegrateRigidBody(cRigidBody* body, float dt) {
		if (body->IsStatic()) return;

		body->mPreviousPosition = body->mPosition;
		body->mAcceleration *= body->mInvMass;
		mIntegrator.RK4(body->mPosition, body->mVelocity, body->mAcceleration, mGravity, dt);
		body->mVelocity *= glm::pow(0.95, dt);
	}
	
	bool cWorld::Collide(cCollisionBody* bodyA, cCollisionBody* bodyB) {
		//figure out what kind of body it is
		eBodyType typeA = bodyA->GetBodyType();
		eBodyType typeB = bodyB->GetBodyType();

		//pass it on to the right integrator method
		if (typeA == eBodyType::rigid && typeB == eBodyType::rigid) {
			return Collide(dynamic_cast<cRigidBody*>(bodyA), dynamic_cast<cRigidBody*>(bodyB));
		}
		else {
			if (typeA == eBodyType::soft && typeB == eBodyType::rigid) {
				return CollideSoftRigid(dynamic_cast<cSoftBody*>(bodyA), dynamic_cast<cRigidBody*>(bodyB));
			}
			if (typeA == eBodyType::rigid && typeB == eBodyType::soft) {
				return CollideSoftRigid(dynamic_cast<cSoftBody*>(bodyB), dynamic_cast<cRigidBody*>(bodyA));
			}
		}
		return false;
	}

	bool cWorld::Collide(cRigidBody* bodyA, cRigidBody* bodyB) {
		eShapeType shapeTypeA = bodyA->GetBodyType();
		eShapeType shapeTypeB = bodyB->GetBodyType();

		if (shapeTypeA == eShapeType::plane) {
			if (shapeTypeB == eShapeType::plane) {
				return false;
			}
			if (shapeTypeB == eShapeType::sphere) {
				return CollideSpherePlane(bodyB, (cSphere*)(bodyB->GetShape()),
					bodyA, (cPlane*)(bodyA->GetShape()));
			}
		}
		if (shapeTypeA == eShapeType::sphere) {
			if (shapeTypeB == eShapeType::plane) {
				return CollideSpherePlane(bodyA, (cSphere*)(bodyA->GetShape()),
					bodyB, (cPlane*)(bodyB->GetShape()));
			}
			if (shapeTypeB == eShapeType::sphere) {
				return CollideSphereSphere(bodyA, (cSphere*)(bodyA->GetShape()),
					bodyB, (cSphere*)(bodyB->GetShape()));
			}
		}
		return false;
	}

	bool cWorld::CollideSoftRigid(cSoftBody* softBody, cRigidBody* rigidBody)	{
		//Check if Rigid Body is inside Bounding Box
		eShapeType shapeType = rigidBody->GetBodyType();

		if (shapeType == eShapeType::plane) {
			return CollideSoftBodyPlane(softBody, rigidBody, dynamic_cast<cPlane*>(rigidBody->GetShape()));
		}
		else if (shapeType == eShapeType::sphere) {
			return CollideSoftBodySphere(softBody, rigidBody, dynamic_cast<cSphere*>(rigidBody->GetShape()));
		}
		return false;
	}

	bool cWorld::CollideSpherePlane(cRigidBody* sphereBody, cSphere* sphereShape, cRigidBody* planeBody, cPlane* planeShape) {
		glm::vec3 c = sphereBody->mPreviousPosition;
		float r = sphereShape->GetRadius();
		glm::vec3 v = sphereBody->mPosition - sphereBody->mPreviousPosition;
		glm::vec3 n = planeShape->GetNormal();
		float d = planeShape->GetConstant();
		float t(1.f);
		glm::vec3 q;
		int result = nCollide::intersect_moving_sphere_plane(c, r, v, n, d, t, q);

		if (result == 0) { //No collision
			return false;
		}
		if (result == -1) { //Already colliding at start of time step
			glm::vec3 pointOnPlane = nCollide::closest_point_on_plane(sphereBody->mPreviousPosition,
				planeShape->GetNormal(), planeShape->GetConstant());
			//Find Corrective Impulse to escape plane
			float distance = glm::distance(sphereBody->mPreviousPosition, pointOnPlane);
			float targetDistance = r;
			glm::vec3 impulse = n * (targetDistance - distance) / mDt;
			sphereBody->mVelocity += impulse;
			//Move Back
			sphereBody->mPosition = sphereBody->mPreviousPosition;
			//Integrate
			IntegrateRigidBody(sphereBody, mDt);
			return true;
		}
		// Collision
		// Reflect via plane normal
		sphereBody->mVelocity = glm::reflect(sphereBody->mVelocity, planeShape->GetNormal());
		// Energy Loss
		glm::vec3 nComponent = glm::proj(sphereBody->mVelocity, planeShape->GetNormal());
		sphereBody->mVelocity -= nComponent * 0.2f;
		// Rewind
		sphereBody->mPosition = (c + v * t);
		// Integrate
		IntegrateRigidBody(sphereBody, mDt * (1.f - t));
		return true;
	}

	bool cWorld::CollideSphereSphere(cRigidBody* bodyA, cSphere* shapeA, cRigidBody* bodyB, cSphere* shapeB) {
		glm::vec3 cA = bodyA->mPreviousPosition;
		glm::vec3 cB = bodyB->mPreviousPosition;
		glm::vec3 vA = bodyA->mPosition - bodyA->mPreviousPosition;
		glm::vec3 vB = bodyB->mPosition - bodyB->mPreviousPosition;
		float rA = shapeA->GetRadius();
		float rB = shapeB->GetRadius();
		float t(1.f);
		int result = nCollide::intersect_moving_sphere_sphere(cA, rA, vA, cB, rB, vB, t);

		if (result == 0) { // No collision
			return false;
		}
		//Get masses to make collisions look good
		float ma = bodyA->mMass;
		float mb = bodyB->mMass;
		float mt = ma + mb;
		if (result == -1) { //Already colliding at start of time step
			//Find Corrective Impulse for both spheres
			float initialDistance = glm::distance(bodyA->mPreviousPosition, bodyB->mPreviousPosition);
			float targetDistance = rA + rB;
			glm::vec3 impulseToA = glm::normalize(bodyA->mPreviousPosition - bodyB->mPreviousPosition);
			impulseToA *= (targetDistance - initialDistance);
			bodyA->mPosition = bodyA->mPreviousPosition;
			bodyB->mPosition = bodyB->mPreviousPosition;
			// Impulse scaled by relative mass
			bodyA->mVelocity += impulseToA * (mb / mt);
			bodyB->mVelocity -= impulseToA * (ma / mt);
			//Integrate
			IntegrateRigidBody(bodyA, mDt);
			IntegrateRigidBody(bodyB, mDt);
			return true;
		}
		// Collision
		// Move to prev pos
		bodyA->mPosition = bodyA->mPreviousPosition + vA * t;
		bodyB->mPosition = bodyB->mPreviousPosition + vB * t;
		// Change velocities to actual instead of or step motion.
		vA = bodyA->mVelocity;
		vB = bodyB->mVelocity;
		// Find post collision velocity
		// Formula at https://en.wikipedia.org/wiki/Inelastic_collision
		// CoefficientOfRestitution - lower is less elastic, higher is more.
		float c = 0.2f; //	todo, bind to rigid body. Multiply each bodies together || take average for this calc. Its a material based value.
		bodyA->mVelocity = (c * mb * (vB - vA) + ma * vA + mb * vB) / mt;
		bodyA->mVelocity = (c * ma * (vA - vB) + ma * vA + mb * vB) / mt;
		// Integrate
		IntegrateRigidBody(bodyA, mDt * (1.f - t));
		IntegrateRigidBody(bodyB, mDt * (1.f - t));
		return true;
	}

	bool cWorld::CollideSoftBodySphere(cSoftBody* softBody, cRigidBody* sphereBody, cSphere* sphereShape) {
		//check if sphere bounding box intersects SoftBody bounding box.
		float rB = sphereShape->GetRadius();
		glm::vec3 sphereMax = sphereBody->mPosition + rB;
		glm::vec3 sphereMin = sphereBody->mPosition - rB;
		glm::vec3 softMin = softBody->mMinCorner;
		glm::vec3 softMax = softBody->mMaxCorner;

		//check soft body min corner vs sphere max && vice versa
		if ((sphereMax.x > softMin.x&& sphereMax.y > softMin.y&& sphereMax.z > softMin.z)
			&& (sphereMin.x < softMax.x && sphereMin.y < softMax.y && sphereMin.z < softMax.z)) {
			//check each node vs the sphere.
			//count for multinode collision
			bool collided = false;
			int numNodes = softBody->NumNodes();
			for (size_t idx = 0; idx < numNodes; idx++)
			{
				cSoftBody::cNode* node = softBody->mNodes[idx];

				glm::vec3 cA = node->previousPosition;
				glm::vec3 cB = sphereBody->mPreviousPosition;
				glm::vec3 vA = node->Position - node->previousPosition;
				glm::vec3 vB = sphereBody->mPosition - sphereBody->mPreviousPosition;
				float rA = node->Radius;
				//float rB = sphereShape->GetRadius();
				float t(1.f);
				int result = nCollide::intersect_moving_sphere_sphere(cA, rA, vA, cB, rB, vB, t);

				if (result == 0) { // No collision
					continue;
				}
				// Collision
				collided = true;
				if (node->isFixed()) { // No math with a static node
					continue;
				}
				if (result == -1) { //Already colliding at start of time step
					//Find Corrective Impulse for both spheres
					float initialDistance = glm::distance(node->previousPosition, sphereBody->mPreviousPosition);
					float targetDistance = rA + rB;
					//Find Corrective Impulse to escape plane
					glm::vec3 impulseToA = node->previousPosition - sphereBody->mPreviousPosition;
					impulseToA *= (targetDistance - initialDistance);
					node->Position = node->previousPosition;
					// Impulse
					node->Velocity += impulseToA;
					//Integrate
					softBody->IntegrateNode(node, mDt);
					continue;
				}
				// Move to prev pos
				node->Position = node->previousPosition + vA * t;
				sphereBody->mPosition = sphereBody->mPreviousPosition + vB * t;
				// Find post collision velocity
				node->Velocity += sphereBody->mVelocity;
				// Integrate
				softBody->IntegrateNode(node, mDt * (1.f - t));
				continue;
			}			
			return collided;
		}
		return false;
	}

	bool cWorld::CollideSoftBodyPlane(cSoftBody* softBody, cRigidBody* planeBody, cPlane* planeShape) {
		// Compute the projection interval radius of b onto L(t) = b.c + t * p.n 
		glm::vec3 n = planeShape->GetNormal();
		glm::vec3 c = (softBody->mMaxCorner + softBody->mMinCorner) * 0.5f;
		
		float e[3] = {
			softBody->mMaxCorner.x - c.x,
			softBody->mMaxCorner.y - c.y,
			softBody->mMaxCorner.z - c.z
		};
		glm::vec3 b[3] = {
			c + e[0],
			c + e[1],
			c + e[2],
		};

		float r = e[0] * fabs(glm::dot(n, b[0])) +
			e[1] * fabs(glm::dot(n, b[1])) +
			e[2] * fabs(glm::dot(n, b[2]));
		// Compute distance of box center from plane
		float s = glm::dot(n, c) - planeShape->GetConstant();
		// Intersection occurs when distance s falls within [-r,+r] interval
		if (fabs(s) <= r) {
			//check each node vs the plane.
			//count for multinode collision
			bool collided = false;
			int numNodes = softBody->NumNodes();
			for (size_t idx = 0; idx < numNodes; idx++)
			{
				cSoftBody::cNode* node = softBody->mNodes[idx];

				glm::vec3 c = node->previousPosition;
				float r = node->Radius;
				glm::vec3 v = node->Position - node->previousPosition;
				glm::vec3 n = planeShape->GetNormal();
				float d = planeShape->GetConstant();
				float t(1.f);
				glm::vec3 q;
				int result = nCollide::intersect_moving_sphere_plane(c, r, v, n, d, t, q);

				if (result == 0) { //No collision
					continue;
				}
				collided = true;
				if (result == -1) { //Already colliding at start of time step
					glm::vec3 pointOnPlane = nCollide::closest_point_on_plane(node->previousPosition,
						planeShape->GetNormal(), planeShape->GetConstant());
					//Find Corrective Impulse to escape plane
					float distance = glm::distance(node->previousPosition, pointOnPlane);
					float targetDistance = r;
					glm::vec3 impulse = n * (targetDistance - distance) / mDt;
					node->Velocity += impulse;
					//Move Back
					node->Position = node->previousPosition;
					//Integrate
					softBody->IntegrateNode(node, mDt);
					continue;
				}
				// Collision
				// Reflect via plane normal
				node->Velocity = glm::reflect(node->Velocity, planeShape->GetNormal());
				// Energy Loss
				glm::vec3 nComponent = glm::proj(node->Velocity, planeShape->GetNormal());
				node->Velocity -= nComponent * 0.2f;
				// Rewind
				node->Position = (c + v * t);
				// Integrate
				softBody->IntegrateNode(node, mDt * (1.f - t));
				continue;
			}
			return collided;
		}

		return false;
	}
	
	void cWorld::DrawDebug() {
		if (!mDebugRenderer) return;

		for (size_t i = 0; i < mBodies.size(); i++)	{
			cCollisionBody* cb = mBodies[i];
			if (cb->GetBodyType() == eBodyType::rigid) {
				cRigidBody* rb = dynamic_cast<cRigidBody*>(cb);
				phys::eShapeType shapeType = rb->GetBodyType();
				if (shapeType == eShapeType::sphere) {
					cSphere* sphere = dynamic_cast<cSphere*>(rb->GetShape());
					glm::mat4 transform(1.f);
					rb->GetTransform(transform);
					mDebugRenderer->PhysDrawSphere(transform, sphere->GetRadius());
				}
				else if (shapeType == eShapeType::plane) {
					cPlane* plane = dynamic_cast<cPlane*>(rb->GetShape());
					mDebugRenderer->PhysDrawPlane(plane->GetNormal(), plane->GetConstant());
				}
			}
			else if (cb->GetBodyType() == eBodyType::soft) {
				//TODO draw soft bodies
			}
		}
	}
}