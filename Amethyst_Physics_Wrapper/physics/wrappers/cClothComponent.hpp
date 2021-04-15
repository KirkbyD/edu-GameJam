#pragma once
#include <physics/interfaces/iClothComponent.h>
#include <cSoftBody.hpp>
#include "cPhysicsWorld.hpp"

namespace nPhysics {
	// cClothComponent
	//
	// Represents a soft body and spere shape(s)
	class cClothComponent : public iClothComponent {
	private:
		//Friends so the world can access these components freely, it needs them a lot.
		friend nPhysics::cPhysicsWorld;
		phys::cSoftBody* mBody;

		std::vector<sTriangle> mRenderTriangles;
		size_t mAcross, mDown;
		
	public:
		// Only contructor, builds the library's RigidBody and shape.
		cClothComponent(const sClothDef& def, const unsigned& id);
		virtual ~cClothComponent();

		// Inherited via iPhysicsComponent
		virtual void GetTransform(glm::mat4& transformOut) override;
		virtual void SetTransform(glm::mat4 TransformIn) override;

		virtual bool GetVelocity(glm::vec3& velocityOut) override;

		virtual void SetPosition(glm::vec3 positionIn) override;

		// The number of nodes in this cloth
		virtual size_t NumNodes() override;

		// Access to the radius of a given node
		virtual bool GetNodeRadius(size_t index, float& radiusOut) override;

		// Access to the position of a given node
		virtual bool GetNodePosition(size_t index, glm::vec3& positionOut) override;

		// Access to the mRenderTriangles, for creating a dynamic mesh based on node positions.
		virtual bool GetTriangles(std::vector<sTriangle>& trianglesOut) override;

		// Access to mAcross and mDown, used to try texture mapping!
		virtual bool GetDimensions(size_t& across, size_t& down);
	};
}