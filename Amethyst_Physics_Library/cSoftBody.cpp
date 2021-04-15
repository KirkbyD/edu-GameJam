#include "cSoftBody.hpp"
#include "nCollide.hpp"

namespace phys{
	cSoftBody::cSpring::cSpring(cNode* nodeA, cNode* nodeB, float springConstant)
		: NodeA(nodeA), NodeB(nodeB)
		, SpringConstant(springConstant)
		, CurrentForceAtoB(glm::vec3(0.f))
	{
		RestingLength = glm::distance(nodeA->Position, nodeB->Position);
	}

	void cSoftBody::cSpring::UpdateSpringForce() {
		//HOOKE'S LAW
		glm::vec3 seperation = NodeB->Position - NodeA->Position;
		float distance = glm::length(seperation);
		float x = distance - RestingLength;

		CurrentForceAtoB = -SpringConstant * glm::normalize(seperation) * x;
	}

	void cSoftBody::cSpring::ApplyForceToNodes() {
		if (!NodeA->isFixed())
			NodeA->Acceleration -= CurrentForceAtoB / NodeA->Mass;
		if (!NodeB->isFixed())
			NodeB->Acceleration += CurrentForceAtoB / NodeB->Mass;
	}

	cSoftBody::cNode* cSoftBody::cSpring::GetOther(cNode* node) {
		return node == NodeA ? NodeB : NodeA;
	}

	cSoftBody::cNode::cNode(const sBodyNodeDef& nodeDef)
		: Position(nodeDef.Position)
		, previousPosition(Position)
		, Mass(nodeDef.Mass)
		, Velocity(glm::vec3(0.f))
		, Acceleration(glm::vec3(0.f))
		, Radius(1.f)
	{ }

	void cSoftBody::cNode::CalculateRadius() {
		float smallestDistance = FLT_MAX;
		size_t numNeighbors = Springs.size();
		for (size_t i = 0; i < numNeighbors; i++) {
			//is his smallest distnce?
			float dist = glm::distance(Springs[i]->GetOther(this)->Position, this->Position);
			if (dist < smallestDistance)
				smallestDistance = dist;
		}
		this->Radius = smallestDistance * 0.45f; //TODO - maybe tweak this value. Make constant?

		//TODO - MAYBE MAKE THIS A CONTAINER FUNCTION INSTEAD?
	}

	bool cSoftBody::cNode::isNeighbor(cNode* other)	{
		for (size_t idx = 0; idx < Springs.size(); idx++) {
			if (Springs[idx]->GetOther(this) == other)
				return true;
		}
		return false;
	}

	void cSoftBody::IntegrateNode(cNode* node, float dt) {
		if (node->isFixed()) return;

		node->Velocity += node->Acceleration * dt;
		node->previousPosition = node->Position;
		node->Position += node->Velocity * dt;
		//Dampen velocity
		node->Velocity *= glm::pow(0.5f, dt);
	}

	void cSoftBody::Integrate(float dt, glm::vec3 accelerationIn) {
		//Apply cumulaive spring forces
		for (cSpring* spring : mSprings) {
			spring->UpdateSpringForce();
			spring->ApplyForceToNodes();
		}

		for (cNode* node : mNodes) {
			if (node->isFixed()) continue;
			//add in global forces
			node->Acceleration += accelerationIn;
			//Integrate Node
			IntegrateNode(node, dt);
		}

		//Handle Internal Collisions
		size_t numNodes = NumNodes();
		for (size_t idxA = 0; idxA < numNodes - 1; idxA++) {
			for (size_t idxB = idxA + 1; idxB < numNodes; idxB++) {
				cNode* bodyA = mNodes[idxA];
				cNode* bodyB = mNodes[idxB];

				glm::vec3 cA = bodyA->previousPosition;
				glm::vec3 cB = bodyB->previousPosition;
				glm::vec3 vA = bodyA->Position - bodyA->previousPosition;
				glm::vec3 vB = bodyB->Position - bodyB->previousPosition;
				float rA = bodyA->Radius;
				float rB = bodyB->Radius;
				float t(1.f);
				int result = nCollide::intersect_moving_sphere_sphere(cA, rA, vA, cB, rB, vB, t);

				if (result == 0) { // No collision
					continue;
				}
				//Get masses to make collisions look good
				float ma = bodyA->Mass;
				float mb = bodyB->Mass;
				float mt = ma + mb;
				if (result == -1) { //Already colliding at start of time step
					//Find Corrective Impulse for both spheres
					float initialDistance = glm::distance(bodyA->previousPosition, bodyB->previousPosition);
					float targetDistance = rA + rB;
					glm::vec3 impulseToA = glm::normalize(bodyA->previousPosition - bodyB->previousPosition);
					impulseToA *= (targetDistance - initialDistance);
					bodyA->Position = bodyA->previousPosition;
					bodyB->Position = bodyB->previousPosition;
					// Impulse scaled by relative mass
					bodyA->Velocity += impulseToA * (mb / mt);
					bodyB->Velocity -= impulseToA * (ma / mt);
					//Integrate
					IntegrateNode(bodyA, dt);
					IntegrateNode(bodyB, dt);
					continue;
				}
				// Collision
				// Move to prev pos
				bodyA->Position = bodyA->previousPosition + vA * t;
				bodyB->Position = bodyB->previousPosition + vB * t;
				// Change velocities to actual instead of or step motion.
				vA = bodyA->Velocity;
				vB = bodyB->Velocity;
				// Find post collision velocity
				// Formula at https://en.wikipedia.org/wiki/Inelastic_collision
				// CoefficientOfRestitution - lower is less elastic, higher is more.
				float c = 0.2f; //	todo, bind to rigid body. Multiply each bodies together || take average for this calc. Its a material based value.
				bodyA->Velocity = (c * mb * (vB - vA) + ma * vA + mb * vB) / mt;
				bodyA->Velocity = (c * ma * (vA - vB) + ma * vA + mb * vB) / mt;
				// Integrate
				IntegrateNode(bodyA, dt * (1.f - t));
				IntegrateNode(bodyB, dt * (1.f - t));			
			}
		}

		//Figure out bounding Box
		mMinCorner = glm::vec3(FLT_MAX);
		mMaxCorner = glm::vec3(-FLT_MAX);
		for (cNode* node : mNodes) {
			//Maxes
			if (node->Position.x > mMaxCorner.x)
				mMaxCorner.x = node->Position.x;
			if (node->Position.y > mMaxCorner.y)
				mMaxCorner.y = node->Position.y;
			if (node->Position.z > mMaxCorner.z)
				mMaxCorner.z = node->Position.z;
			//Minimums
			if (node->Position.x < mMinCorner.x)
				mMinCorner.x = node->Position.x;
			if (node->Position.y < mMinCorner.y)
				mMinCorner.y = node->Position.y;
			if (node->Position.z < mMinCorner.z)
				mMinCorner.z = node->Position.z;
		}
		mMaxCorner += mNodes[0]->Radius;
		mMinCorner -= mNodes[0]->Radius;
	}

	cSoftBody::cSoftBody(const sSoftBodyDef& def) 
	: cCollisionBody(eBodyType::soft)
	, mMinCorner(glm::vec3(FLT_MAX))
	, mMaxCorner(glm::vec3(-FLT_MAX)) {
		//Create all nodes
		size_t numNodes = def.Nodes.size();
		mNodes.resize(numNodes);
		for (size_t i = 0; i < numNodes; i++) {
			mNodes[i] = new cNode(def.Nodes[i]);
		}

		//Create all springs
		size_t numSprings = def.Springs.size();
		mSprings.resize(numSprings);
		for (size_t i = 0; i < numSprings; i++)	{
			mSprings[i] = new cSpring(mNodes[def.Springs[i].first],
								      mNodes[def.Springs[i].second],
									  def.SpringConstant);
			mSprings[i]->NodeA->Springs.push_back(mSprings[i]);
			mSprings[i]->NodeB->Springs.push_back(mSprings[i]);
		}

		//Find node radii
		for (size_t i = 0; i < numNodes; i++) {
			mNodes[i]->CalculateRadius();
		}
	}

	cSoftBody::~cSoftBody()	{
		for (cNode* node : mNodes) {
			delete node;
		}
		mNodes.clear();

		for (cSpring* spring : mSprings) {
			delete spring;
		}
		mSprings.clear();
	}

	size_t cSoftBody::NumNodes() {
		return this->mNodes.size();
	}

	bool cSoftBody::GetNodeRadius(size_t index, float& radiusOut) {
		if (index >= mNodes.size()) return false;
		radiusOut = mNodes[index]->Radius;
		return true;
	}

	bool cSoftBody::GetNodePosition(size_t index, glm::vec3& positionOut) {
		if (index >= mNodes.size()) return false;
		positionOut = mNodes[index]->Position;
		return true;
	}

	void cSoftBody::ClearAccelerations() {
		for (cNode* node : mNodes) {
			node->Acceleration = glm::vec3(0.f);
		}
	}

}

