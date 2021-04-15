#include "cReturnState.hpp"
#include "cModelObject.hpp"
#include "nAI.hpp"

extern nAI::cGraph* graph;
extern cModelObject* gatherer;
extern std::vector<nAI::sGraphNode*> bases;
extern nAI::sGraphNode* currentNode;

ReturnState::ReturnState(void)
	: cFSMState("Return State") {
}

ReturnState::~ReturnState(void) {
}

void ReturnState::Update(void) {
	if (hasBasePath == false) {
		printf("ReturnState: Plotting Course back to base...\n");
		nAI::sGraphNode* pathNode;
		pathNode = aiAlgorithms.AStarSearch(currentNode, graph, bases[0]);

		while (pathNode->prevNode != nullptr) {
			posList.push_front(pathNode->pos);
			pathNode = pathNode->prevNode;
		}
		hasBasePath = true;
	}
	else {
		printf("ReturnState: Returning to Base with resources...\n");
		if (!posList.empty()) {
			aiAlgorithms.ReturnState(gatherer->positionXYZ, posList.front());
			if (gatherer->positionXYZ.x <= posList.front().x && gatherer->positionXYZ.z <= posList.front().z) {
				if (posList.size() == 1) {
					gatherer->positionXYZ.x = posList.front().x;
					gatherer->positionXYZ.z = posList.front().z;
				}
				posList.pop_front();
			}
		}
	}

	if (gatherer->positionXYZ.x == bases[0]->pos.x && gatherer->positionXYZ.z == bases[0]->pos.z) {
		printf("ReturnState: Dropping off resource at base!\n");
		currentNode = bases[0];
		hasBasePath = false;
		mCurrentCondition = 1;
	}
}


void ReturnState::OnEnterState(void) {
	printf("ReturnState: Entered\n");
	mCurrentCondition = 0;
}

void ReturnState::OnExitState(void) {
	printf("ReturnState: Exited\n");
}
