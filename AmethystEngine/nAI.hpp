#pragma once

#include <type_traits>
#include <iostream>
#include <vector>
#include <list>
#include <glm/glm.hpp>
#include <vector>

extern double deltaTime;

namespace nAI {
	struct waypoints {
		std::vector<glm::vec3> vec_waypointPositions;
		size_t waypointPos;
		float waypointRadius;
		bool isInvert;
	};

	struct formationAnchor {
		glm::vec3 position;
		glm::vec3 centerPos;
		float offset;
		float circleoffset;
		float nodes;
		float radius;
		size_t rowdef;
		size_t columndef;
	};
	

	enum class eNodeType { INVALID = -1, EMPTY, SPAWN, BASE, RESOURCE, DIFFTERRAIN };

	struct sGraphNode {
		unsigned id;
		bool visited;
		int type;
		float gCost;
		float hDist;
		glm::vec3 pos;
		sGraphNode* prevNode;
		std::vector<std::pair<sGraphNode*, float>> linkedNodes;
	};

	class cGraph {
		unsigned next_id = 0;
	public:
		cGraph() { }
		~cGraph() { }
		void CreateNode(glm::vec3 position, int type) {
			sGraphNode* node = new sGraphNode();
			node->id = next_id;
			node->pos = position;
			node->type = type;
			node->gCost = FLT_MAX;
			node->hDist = 0;
			node->prevNode = nullptr;
			node->visited = false;

			this->nodes.push_back(node);
			++next_id;
		}

		void AddEdge(sGraphNode* prevNode, sGraphNode* linkedNode, float weight) {
			std::pair<sGraphNode*, float> edge;
			edge.first = linkedNode;
			edge.second = weight;
			prevNode->linkedNodes.push_back(edge);

			std::pair<sGraphNode*, float> reverseEdge;
			reverseEdge.first = prevNode;
			reverseEdge.second = weight;
			linkedNode->linkedNodes.push_back(reverseEdge);
		}

		void ResetGraph() {
			for (sGraphNode*& currNode : this->nodes) {
				currNode->visited = false;
				currNode->prevNode = NULL;
				currNode->gCost = FLT_MAX;
			}
		}

		void PrintGraph() {
			for (sGraphNode*& currNode : this->nodes) {
				std::cout << "Node: " << currNode->id << " -> ";

				for (std::pair <sGraphNode*, float>& child : currNode->linkedNodes)
					std::cout << "( " << child.first->id << ", " << child.second << ") ";

				std::cout << std::endl;
			}
		}

		void PrintParents(bool includeCost) {
			for (sGraphNode*& currNode : this->nodes) {
				std::cout << "Node: " << currNode->id << " -> ";
				if (currNode->prevNode != nullptr)
					std::cout << currNode->prevNode->id;
				else
					std::cout << "NULL";

				if (includeCost)
					std::cout << " cost so far: " << currNode->gCost << " hDist= " << currNode->hDist << " f= " << currNode->gCost + currNode->hDist;
				std::cout << std::endl;
			}
		}

		std::vector<sGraphNode*> nodes;
	};

	// Algorithms
	class cAIAlgorithms {
	public:
		sGraphNode* DijkstraSearch(sGraphNode* rootNode, cGraph* graph, eNodeType goalType) {
			graph->ResetGraph();

			rootNode->visited = true;
			rootNode->gCost = 0;
			std::vector<sGraphNode*> closedList;
			std::vector<sGraphNode*> openList;
			openList.push_back(rootNode);

			while (!openList.empty()) {
				float minCost = FLT_MAX;
				int minIndex = 0;
				sGraphNode* currNode;
				//find node with the lowest cost from root node
				for (size_t i = 0; i < openList.size(); ++i) {
					if (openList[i]->gCost < minCost) {
						minCost = openList[i]->gCost;
						minIndex = i;
					}
				}

				//remove current node from open list and add to closed list
				currNode = openList[minIndex];
				for (auto iter = openList.begin(); iter != openList.end(); ++iter) {
					if (*iter == currNode) {
						openList.erase(iter);
						break;
					}
				}
				closedList.push_back(currNode);

				currNode->visited = true;
				if (currNode->type == (int)goalType) return currNode;

				//Go through every linkednode
				for (std::pair <sGraphNode*, float> linkedNode : currNode->linkedNodes) {
					if (linkedNode.first->visited == false) {
						float weightSoFar = currNode->gCost + linkedNode.second;
						if (weightSoFar < linkedNode.first->gCost) {
							//update node when new better path is found
							linkedNode.first->gCost = weightSoFar;
							linkedNode.first->prevNode = currNode;
							if (!IsNodeInOpenList(openList, linkedNode.first))
								openList.push_back(linkedNode.first); //add newly discovered node to open list
						}
					}
				}
			}
			return nullptr;
		}

		sGraphNode* AStarSearch(sGraphNode* rootNode, cGraph* graph, sGraphNode* target) {
			graph->ResetGraph();

			rootNode->gCost = 0;
			rootNode->hDist = CalculateHeuristics(rootNode, target);

			std::vector<sGraphNode*> closedList;
			std::vector<sGraphNode*> openList;
			openList.push_back(rootNode);

			while (!openList.empty()) {
				float minCost = FLT_MAX;
				int minIndex = 0;
				sGraphNode* currNode;
				//find node with the lowest cost from root node and heuristic distance from the goal node
				for (size_t i = 0; i < openList.size(); ++i) {
					if (openList[i]->gCost + openList[i]->hDist < minCost) {
						minCost = openList[i]->gCost + openList[i]->hDist;
						minIndex = i;
					}
				}

				//remove current node from open list and add to closed list
				currNode = openList[minIndex];
				for (auto iter = openList.begin(); iter != openList.end(); ++iter) {
					if (*iter == currNode) {
						openList.erase(iter);
						break;
					}
				}
				closedList.push_back(currNode);

				currNode->visited = true;
				if (currNode->id == target->id)
					return currNode;

				//Go through every child node node 
				for (std::pair <sGraphNode*, float> linkedNode : currNode->linkedNodes) {
					if (linkedNode.first->visited == false) {
						float fCost = currNode->gCost + linkedNode.second;
						if (fCost < linkedNode.first->gCost) {
							linkedNode.first->gCost = fCost;
							linkedNode.first->prevNode = currNode;
							if (!IsNodeInOpenList(openList, linkedNode.first)) {
								linkedNode.first->hDist = CalculateHeuristics(linkedNode.first, target);
								openList.push_back(linkedNode.first);
							}
						}
					}
				}
			}
			return nullptr;
		}
		
		bool IsNodeInOpenList(std::vector<sGraphNode*> openList, sGraphNode* linkedNode) {
			for (int i = 0; i < openList.size(); ++i) {
				if (openList[i] == linkedNode)
					return true;
			}
			return false;
		}

		float CalculateHeuristics(sGraphNode* current_node, sGraphNode* target) {
			float D = 1;
			float dx = abs(current_node->pos.x - target->pos.x);
			float dy = abs(current_node->pos.y - target->pos.y);
			return D * (dx + dy);
		}
		
		void SeekState(glm::vec3& sourcePos, glm::vec3 targetPos) {
			glm::vec3 maxSpeed = glm::vec3(0.25f, 0.f, 0.25f);
			glm::vec3 vel = glm::vec3(0.125f, 0.f, 0.125f);
			glm::vec3 desiredVelocity = glm::vec3(0.f);
			glm::vec3 seekForce = glm::vec3(0.f);
			targetPos.y += 1.f;

			if (sourcePos.x >= targetPos.x)
				vel.x = 0.f;
			if (sourcePos.z >= targetPos.z)
				vel.z = 0.f;

			desiredVelocity = glm::normalize(targetPos - sourcePos) * maxSpeed;
			
			seekForce = desiredVelocity - vel;

			sourcePos += vel + (seekForce * (float)deltaTime);
		}

		void ReturnState(glm::vec3& sourcePos, glm::vec3 targetPos) {
			glm::vec3 maxSpeed = glm::vec3(0.25f, 0.f, 0.25f);
			glm::vec3 vel = glm::vec3(0.125f, 0.f, 0.125f);
			glm::vec3 desiredVelocity = glm::vec3(0.f);
			glm::vec3 seekForce = glm::vec3(0.f);
			targetPos.y += 1.f;

			if (sourcePos.x <= targetPos.x)
				vel.x = 0.f;
			if (sourcePos.z <= targetPos.z)
				vel.z = 0.f;

			desiredVelocity = glm::normalize(targetPos - sourcePos) * maxSpeed;

			seekForce = desiredVelocity - vel;

			sourcePos -= vel + (seekForce * (float)deltaTime);
		}

	};
}
