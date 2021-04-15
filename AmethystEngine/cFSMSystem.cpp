#include "cFSMSystem.hpp"
#include "cIdleState.hpp"
#include "cGatherState.hpp"
#include "cReturnState.hpp"
#include "cSearchState.hpp"

cFSMSystem::cFSMSystem(): mIsRunning(false), mCurrentState(nullptr) { }

cFSMSystem::~cFSMSystem() {
	mCurrentState = nullptr;
	delete mCurrentState;
}

//States our FSM and begins the first state entered in our vector
void cFSMSystem::Start(void) {
	if (mStateVec.size() == 0) {
		printf("Finite State Machine is empty! Cannot start it!");
		return;
	}

	mIsRunning = true;
	TransitionToState(mStateVec[0]);
}

//adds state to our FSM that our AI agent can switch to
void cFSMSystem::AddState(cFSMState* state) {
	cFSMState* key = state;
	if (find(mStateVec.begin(), mStateVec.end(), key) == mStateVec.end()) {
		mStateVec.push_back(state);
	}
}

//calls the Update function of our currently active state.
//checks every frame if a condition is met that triggers changing to a new state
//if triggered, than transition to the new state
void cFSMSystem::Update(void)
{
	if (!mIsRunning) return;

	if (mCurrentState == 0) {
		printf("Error: The current state is null!");
		mIsRunning = false;
		return;
	}

	mCurrentState->Update();

	if (mCurrentState->IsDone()) {
		cFSMState* newState = mCurrentState->GetTransition();
		TransitionToState(newState);

	}

}

//calls OnExitState() before switching to our new state
//we then have our new state call onEnterState()
void cFSMSystem::TransitionToState(cFSMState* state) {
	if (state == 0) {
		printf("Error: [FSMSystem::TransitionToState: The state is null");
		mIsRunning = false;
		return;
	}

	if (mCurrentState != 0) {
		mCurrentState->OnExitState();
	}

	mCurrentState = state;
	mCurrentState->OnEnterState();
}

//Resets our FSM
//we call OnExitState of any current state we have active before resetting
void cFSMSystem::Reset(void) {
	if (mCurrentState != 0) {
		mCurrentState->OnExitState();
	}
	mCurrentState = 0;
	mIsRunning = false;
}

void cFSMSystem::Init() {
	cFSMState* stateIdle = new IdleState();     //initial state
	cFSMState* stateSearch = new SearchState(); //checks if there are any resources left
	cFSMState* stateGather = new GatherState(); //reduces the number of resources on the map 
	cFSMState* stateReturn = new ReturnState(); //returns resource to player

	//set the transitions a state can make to sitch to a new state
	stateIdle->AddTransition(1, stateSearch);

	//1 = resource found, switch to gather state
	//2 = no resources found, exit our FSM
	stateSearch->AddTransition(1, stateGather);
	stateSearch->AddTransition(2, 0);

	stateGather->AddTransition(1, stateReturn);

	stateReturn->AddTransition(1, stateSearch);

	//Add our states to the FSM
	AddState(stateIdle);
	AddState(stateSearch);
	AddState(stateGather);
	AddState(stateReturn);
}


