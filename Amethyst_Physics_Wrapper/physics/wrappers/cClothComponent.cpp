#include "cClothComponent.hpp"
#include <glm\ext\matrix_transform.hpp>

namespace nPhysics {
	cClothComponent::cClothComponent(const sClothDef& def, const unsigned& id)
	: iClothComponent(id)
	, mBody(0)
	{
		mAcross = def.nodesAcross;
		mDown = def.nodesDown;

		size_t numNodes = def.nodesAcross * def.nodesDown;
		phys::sSoftBodyDef physDef;
		physDef.Nodes.resize(numNodes);
		size_t idxNode = 0;

		//TODO - make sure we aren't getting in 0 or 1 for nodes across
		glm::vec3 sepAcross = (def.cornerB - def.cornerA);
		sepAcross /= def.nodesAcross - 1;

		glm::vec3 sepDown = glm::normalize(def.downDir) * glm::length(sepAcross);

		//Traverse to set up nodes
		for (size_t idxAcross = 0; idxAcross < def.nodesAcross; idxAcross++) {
			for (size_t idxDown = 0; idxDown < def.nodesDown; idxDown++) {
				physDef.Nodes[idxNode].Position = def.cornerA 
												+ sepAcross * (float)idxAcross 
												+ sepDown * (float)idxDown;
				physDef.Nodes[idxNode].Mass = def.nodeMass;
				idxNode++;
			}
		}
		//set top corners to static
		physDef.Nodes[0].Mass = 0.f;
		physDef.Nodes[numNodes - def.nodesDown].Mass = 0.f;
		//trying static top row instead
		for (size_t nodeIdx = 0; nodeIdx < numNodes; nodeIdx+=def.nodesDown)
		{
			physDef.Nodes[nodeIdx].Mass = 0.f;
		}

		physDef.SpringConstant = def.springConstant;

		//Set up adjacent springs for the nodes
		//define the spring going right and down for each node, stop one early.
		for (size_t idxAcross = 0; idxAcross < def.nodesAcross; idxAcross++) {
			for (size_t idxDown = 0; idxDown < def.nodesDown; idxDown++) {
				//My positon
				size_t idxNode = (idxAcross * def.nodesDown) + idxDown;
				//set across spring going right
				if(idxAcross < def.nodesAcross - 1) //not on rightmost row
					physDef.Springs.push_back(std::make_pair(idxNode, idxNode + def.nodesDown));

				//set down spring goin down
				if(idxDown < def.nodesDown - 1) //not on bottom row
					physDef.Springs.push_back(std::make_pair(idxNode, idxNode + 1));

				//set spring diagonal right.
				if (idxAcross < def.nodesAcross - 1 && idxDown < def.nodesDown - 1) {
					physDef.Springs.push_back(std::make_pair(idxNode, idxNode + 1 + def.nodesDown));

					//set up two draw triangles for the 'top right' nodes only
					sTriangle TriRightThenDown;
					TriRightThenDown.a = idxNode;
					TriRightThenDown.b = idxNode + def.nodesDown;
					TriRightThenDown.c = idxNode + 1 + def.nodesDown;
					mRenderTriangles.push_back(TriRightThenDown);

					sTriangle TriDownThenRight;
					TriDownThenRight.a = idxNode;
					TriDownThenRight.b = idxNode + 1;
					TriDownThenRight.c = idxNode + 1 + def.nodesDown;
					mRenderTriangles.push_back(TriDownThenRight);
				}
				
				//set spring diagonal left.
				if(idxAcross != 0 && idxDown < def.nodesDown - 1)
					physDef.Springs.push_back(std::make_pair(idxNode, idxNode + 1 - def.nodesDown));

			}
		}
		
		mBody = new phys::cSoftBody(physDef);
		mBody->SetUserPointer(this);
	}

	cClothComponent::~cClothComponent()	{
		if (mBody) {
			delete mBody;
			mBody = 0;
		}
	}

	void cClothComponent::GetTransform(glm::mat4& transformOut)	{
		transformOut = glm::mat4(1.f);
		glm::vec3 rootNodePos;
		GetNodePosition(0, rootNodePos);
		transformOut = glm::translate(transformOut, rootNodePos);
	}

	void cClothComponent::SetTransform(glm::mat4 TransformIn) {
		//todo
	}

	bool cClothComponent::GetVelocity(glm::vec3& velocityOut) {
		return false;
		//todo remove?
	}

	void cClothComponent::SetPosition(glm::vec3 positionIn)	{
		//todo
	}

	size_t cClothComponent::NumNodes() {
		return mBody->NumNodes();
	}

	bool cClothComponent::GetNodeRadius(size_t index, float& radiusOut)	{
		return mBody->GetNodeRadius(index, radiusOut);
	}

	bool cClothComponent::GetNodePosition(size_t index, glm::vec3& positionOut)	{
		return mBody->GetNodePosition(index, positionOut);
	}

	bool cClothComponent::GetTriangles(std::vector<sTriangle>& trianglesOut) {
		if (mRenderTriangles.size() < 0)
			return false;
		trianglesOut = mRenderTriangles;
		return true;
	}

	bool cClothComponent::GetDimensions(size_t& across, size_t& down)	{
		across = mAcross;
		down = mDown;
		return true;
	}
}