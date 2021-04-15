#include "cSearchState.hpp"
#include "cModelObject.hpp"
#include "nAI.hpp"


extern nAI::cGraph* graph;
extern cModelObject* gatherer;
extern std::vector<nAI::sGraphNode*> resources;
extern nAI::sGraphNode* currentNode;
extern bool isSimulationRunning;

SearchState::SearchState(void)
	: cFSMState("Search State") {
}

SearchState::~SearchState(void) {
}

void SearchState::Update(void) {
	if (gNumResources == 0) {
		printf("SearchState: No resources found; Terminating Simulation!\n");
		isSimulationRunning = false;
		mCurrentCondition = 2;
	}
	else if (gatherer->positionXYZ.x == resources[0]->pos.x && gatherer->positionXYZ.z == resources[0]->pos.z) {
		printf("SearchState: Arrived at resources!\n");
		currentNode = resources[0];
		hasSearched = false;
		mCurrentCondition = 1;
	}
	else {
		if (hasSearched == false) {
			printf("SearchState: Searching for resources...\n");
			// this would be a loop to determine which resource is the best choice if multiple resources are present...
			nAI::sGraphNode* pathNode;
			pathNode = aiAlgorithms.DijkstraSearch(currentNode, graph, nAI::eNodeType::RESOURCE);

			while (pathNode->prevNode != nullptr) {
				posList.push_front(pathNode->pos);
				pathNode = pathNode->prevNode;
			}

			hasSearched = true;
		}
		else {
			printf("SearchState: Moving to resources...\n");
			if (!posList.empty()) {
				aiAlgorithms.SeekState(gatherer->positionXYZ, posList.front());
				if (gatherer->positionXYZ.x >= posList.front().x && gatherer->positionXYZ.z >= posList.front().z) {
					if (posList.size() == 1) {
						gatherer->positionXYZ.x = posList.front().x;
						gatherer->positionXYZ.z = posList.front().z;
					}
					posList.pop_front();
				}
			}
		}
	}
}

void SearchState::OnEnterState(void) {
	printf("SearchState: Entered\n");
	mCurrentCondition = 0;
}

void SearchState::OnExitState(void) {
	printf("SearchState: Exited\n");
}
