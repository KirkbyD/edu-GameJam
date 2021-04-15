#pragma once
#pragma once
#include <glm/vec3.hpp>
#include "iPhysicsComponent.h"
#include <vector>

namespace nPhysics
{
	// sTriangle
	//
	// Holds the index number for nodes that make a triangle for rendering purposes. 
	struct sTriangle {
		size_t a, b, c;
	};


	// sClothDef
	//
	// Holds all relevant information for an iClothComponent, 
	// Necessary to build a concrete cClothComponent in a wrapper.
	struct sClothDef {
		glm::vec3 cornerA, cornerB, downDir;
		size_t nodesAcross, nodesDown;
		float nodeMass, springConstant;
	};

	class iClothComponent : public iPhysicsComponent {
	public:
		virtual ~iClothComponent() {}

		//Returns number of nodes in coth.
		virtual size_t NumNodes() = 0;

		// Retrieve radius of node at index.
		// returns false if index does not exist.
		virtual bool GetNodeRadius(size_t index, float& radiusOut) = 0;

		// Retrieve position of node at index.
		// returns false if index does not exist.
		virtual bool GetNodePosition(size_t index, glm::vec3& positionOut) = 0;

		// Retrieve triangles drawable for soft body
		// returns false if there are none ie, this is a rope
		virtual bool GetTriangles(std::vector<sTriangle>& trianglesOut) = 0;

		// Retrieve number of nodesm across and down
		// TODO never returns false so I should make it a void!
		virtual bool GetDimensions(size_t& across, size_t& down) = 0;
		
	protected:
		// Automatically fulfills interface requirement when called
		iClothComponent(const unsigned& id) : iPhysicsComponent(eComponentType::cloth, id) {}

	private:
		// Delete constructors so that they cannot be used
		//iClothComponent() = delete;
		iClothComponent(const iClothComponent& other) = delete;
		iClothComponent& operator=(const iClothComponent& other) = delete;
	};
}