#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include "cCollisionBody.hpp"

namespace phys {

	struct sBodyNodeDef {
		glm::vec3 Position;
		float Mass;
	};

	// sSoftBodyDef
	// 
	// Contains all non-shape related information
	// necessary to create a cSoftBody instance.
	struct sSoftBodyDef {
		std::vector<sBodyNodeDef> Nodes;
		std::vector<std::pair<size_t, size_t>> Springs;
		float SpringConstant;
	};

	class cSoftBody : public cCollisionBody {
		// cWorld will be operating on cRigidBody values quite a bit
		// We will trust it to do everything correctly.
		friend class cWorld;
	private:
		class cSpring; //forward declared for cNode

		class cNode { // could make it extend rigid body instead?
		public:
			cNode(const sBodyNodeDef& nodeDef);
			void CalculateRadius();
			bool isNeighbor(cNode* other);
			inline bool isFixed() { return Mass == 0 ? true : false; }

			std::vector<cSpring*> Springs;
			float Mass;
			float Radius;
			glm::vec3 Position, previousPosition, Velocity, Acceleration;
		};

		class cSpring {
		public:
			cSpring(cNode* nodeA, cNode* nodeB, float springConstant);
			
			void UpdateSpringForce();
			void ApplyForceToNodes();
			cNode* GetOther(cNode* node);

			cNode* NodeA;
			cNode* NodeB;
			float SpringConstant;
			float RestingLength;
			glm::vec3 CurrentForceAtoB;
		};

		// Constructors not to be used.
		cSoftBody() = delete;
		cSoftBody(const cSoftBody& other) = delete;
		cSoftBody& operator=(const cSoftBody& other) = delete;

	private:
		//actual member variables
		std::vector<cNode*> mNodes;
		std::vector<cSpring*> mSprings;

		//Used for broad phase collision checking - represents cloth bounding box
		glm::vec3 mMaxCorner, mMinCorner;

	protected:
		void IntegrateNode(cNode* node, float dt);
		void Integrate(float dt, glm::vec3 accelerationIn);

	public:
		cSoftBody(const sSoftBodyDef& def);
		virtual ~cSoftBody();

		size_t NumNodes();
		bool GetNodeRadius(size_t index, float& radiusOut);
		bool GetNodePosition(size_t index, glm::vec3& positionOut);

		// Inherited via cCollisionBody
		virtual void ClearAccelerations() override;
		//TODO
	};
}