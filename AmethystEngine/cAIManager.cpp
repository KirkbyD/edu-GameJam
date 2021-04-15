#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include <iostream>
#include "cAIManager.hpp"
#include "nAI.hpp"
#include "nAIEnums.hpp"

#include "cThreadSys.hpp"

// HACKY REMOVE LATER
extern std::map<std::string, cModelObject*> mpModel;

#pragma region SINGLETON
cAIManager cAIManager::stonAIMngr;
cAIManager* cAIManager::GetAIManager() { return &(cAIManager::stonAIMngr); }
cAIManager::cAIManager()
	: ai_behaviours(), pMediator(nullptr)
{ std::cout << "AI Manager Created" << std::endl; }
#pragma endregion



#pragma region MEDIATOR SETUP
void cAIManager::setMediatorPointer(iMediatorInterface* pMediator) { this->pMediator = pMediator; }
#pragma endregion



#pragma region MEDIATOR COMMUNICATIONS
sData cAIManager::RecieveMessage(sData& data) {
	data.setResult(OK);

	switch (data.getCMD()) {
	case INVERT_PATH:
		wp.isInvert = !wp.isInvert;
		if (wp.isInvert == true) {
			--wp.waypointPos;
			mpModel["0 Test"]->positionXYZ = wp.vec_waypointPositions[wp.waypointPos];
		}
		else {
			++wp.waypointPos;
			mpModel["0 Test"]->positionXYZ = wp.vec_waypointPositions[wp.waypointPos];
		}
		break;

	case SET_SEPERATION_WEIGHT:
		if (data.getFlockWeight() < 0) {
			seperationWeight += data.getFlockWeight();
			alignmentWeight += 0.05f;
			cohesionWeight += 0.05f;
		}
		else {
			seperationWeight += data.getFlockWeight();
			alignmentWeight -= 0.05f;
			cohesionWeight -= 0.05f;
		}
		break;

	case SET_ALIGNMENT_WEIGHT:
		if (data.getFlockWeight() < 0) {
			seperationWeight += 0.05f;
			alignmentWeight += data.getFlockWeight();
			cohesionWeight += 0.05f;
		}
		else {
			seperationWeight -= 0.05f;
			alignmentWeight += data.getFlockWeight();
			cohesionWeight -= 0.05f;
		}
		break;

	case SET_COHESION_WEIGHT:
		if (data.getFlockWeight() < 0) {
			seperationWeight += 0.05f;
			alignmentWeight += 0.05f;
			cohesionWeight += data.getFlockWeight();
		}
		else {
			seperationWeight -= 0.05f;
			alignmentWeight -= 0.05f;
			cohesionWeight += data.getFlockWeight();
		}
		break;

	case UNKN_CMD:
		data.setResult(UNKNOWN_COMMAND);
		break;

	default:
		data.setResult(INVALID_COMMAND);
		break;
	}

	return data;
}
#pragma endregion



#pragma region AI BEHAVIOURS UPDATE
void cAIManager::s_update(sData& data) {
	/*update(data);*/
}

void cAIManager::update(sData& data) {
	std::cout << "UPDATE CALLED... IN AI MANAGER... EXECUTING AI UPDATE..." << std::endl;
	// change this such that it passes through only what is needed to the AI Behaviours
	// before this switch sends data to the AI Behaviours it needs to determine if the behaviour in question can be enacted...

	// update any active formations after the steering behaviours
	switch (data.getSourceGameObj()->GetActor()->getActiveFormation()) {
	case (size_t)nAIEnums::AIEnums::eFormationBehaviours::CIRCLE:
	{
		fa.nodes = 0;

		// get the center position of the selected units
		for (auto a : g_vec_pActorObjects) {
			if (a->isSelected())
				a->_formation_pos = ++fa.nodes;
		}

		for (auto a : data.getEntities()) {
			if (a->HasActor()) {
				if (a->GetActor()->_formation_pos == 1) {
					fa.position = a->getPosition();
					break;
				}
			}
		}

		if (data.getSourceGameObj()->GetActor()->_formation_pos == 1) {
			fa.centerPos.x = fa.position.x - fa.radius;
			fa.centerPos.z = fa.position.z - fa.radius;
		}

		fa.circleoffset = (360 / fa.nodes) * (data.getSourceGameObj()->GetActor()->_formation_pos - 1);
		glm::vec3 front = data.getSourceGameObj()->getFrontVector();
		front.y = 0.f;
		front = glm::normalize(front);

		glm::vec3 centerOffset = glm::vec3(front.x * -fa.radius, 0.f, front.z * -fa.radius);
		fa.centerPos = fa.position + centerOffset;

		// need to determine the point at which to set the formation nodes...

		if (data.getSourceGameObj()->HasActor()) {
			// if its not the anchor unit then override its movement for the formation
			if (data.getSourceGameObj()->GetActor()->_formation_pos > 1) {
				ai_behaviours.formCircle(data, fa);				// set the target position based off of the desired unit position
				data.getSourceGameObj()->GetActor()->setActiveBehaviour((size_t)nAIEnums::AIEnums::eAIBehviours::SEEK);
			}
		}
		break;
	}

	case (size_t)nAIEnums::AIEnums::eFormationBehaviours::COLUMN:
	{
		fa.nodes = 0;

		// get the center position of the selected units
		for (auto a : g_vec_pActorObjects) {
			if (a->isSelected())
				a->_formation_pos = ++fa.nodes;
		}

		for (auto a : data.getEntities()) {
			if (a->HasActor()) {
				if (a->GetActor()->_formation_pos == 1) {
					fa.position = a->getPosition();
					break;
				}
			}
		}

		// need to determine the point at which to set the formation nodes...

		if (data.getSourceGameObj()->HasActor()) {
			// if its not the anchor unit then override its movement for the formation
			if (data.getSourceGameObj()->GetActor()->_formation_pos > 1) {
				ai_behaviours.formColumn(data, fa);				// set the target position based off of the desired unit position
				data.getSourceGameObj()->GetActor()->setActiveBehaviour((size_t)nAIEnums::AIEnums::eAIBehviours::SEEK);
			}
		}
		break;
	}

	case (size_t)nAIEnums::AIEnums::eFormationBehaviours::LINE:
	{
		fa.nodes = 0;

		// get the center position of the selected units
		for (auto a : g_vec_pActorObjects) {
			if (a->isSelected())
				a->_formation_pos = ++fa.nodes;
		}

		for (auto a : data.getEntities()) {
			if (a->HasActor()) {
				if (a->GetActor()->_formation_pos == 1) {
					fa.position = a->getPosition();
					break;
				}
			}
		}

		// need to determine the point at which to set the formation nodes...

		if (data.getSourceGameObj()->HasActor()) {
			// if its not the anchor unit then override its movement for the formation
			if (data.getSourceGameObj()->GetActor()->_formation_pos > 1) {
				ai_behaviours.formLine(data, fa);				// set the target position based off of the desired unit position
				data.getSourceGameObj()->GetActor()->setActiveBehaviour((size_t)nAIEnums::AIEnums::eAIBehviours::SEEK);
			}
		}
		break;
	}

	case (size_t)nAIEnums::AIEnums::eFormationBehaviours::SQUARE:
	{
		fa.nodes = 0;

		// get the center position of the selected units
		for (auto a : g_vec_pActorObjects) {
			if (a->isSelected())
				a->_formation_pos = ++fa.nodes;
		}

		for (auto a : data.getEntities()) {
			if (a->HasActor()) {
				if (a->GetActor()->_formation_pos == 1) {
					fa.position = a->getPosition();
					break;
				}
			}
		}

		// need to determine the point at which to set the formation nodes...

		if (data.getSourceGameObj()->HasActor()) {
			// if its not the anchor unit then override its movement for the formation
			if (data.getSourceGameObj()->GetActor()->_formation_pos > 1) {
				ai_behaviours.formBox(data, fa);				// set the target position based off of the desired unit position
				data.getSourceGameObj()->GetActor()->setActiveBehaviour((size_t)nAIEnums::AIEnums::eAIBehviours::SEEK);
			}
		}
		break;
	}

	case (size_t)nAIEnums::AIEnums::eFormationBehaviours::WEDGE:
	{
		fa.nodes = 0;

		// get the center position of the selected units
		for (auto a : g_vec_pActorObjects) {
			if (a->isSelected())
				a->_formation_pos = ++fa.nodes;
		}

		for (auto a : data.getEntities()) {
			if (a->HasActor()) {
				if (a->GetActor()->_formation_pos == 1) {
					fa.position = a->getPosition();
					break;
				}
			}
		}

		// need to determine the point at which to set the formation nodes...

		if (data.getSourceGameObj()->HasActor()) {
			// if its not the anchor unit then override its movement for the formation
			if (data.getSourceGameObj()->GetActor()->_formation_pos > 1) {
				ai_behaviours.formWedge(data, fa);				// set the target position based off of the desired unit position
				data.getSourceGameObj()->GetActor()->setActiveBehaviour((size_t)nAIEnums::AIEnums::eAIBehviours::SEEK);
			}
		}
		break;
	}

	default:
		break;
	}

	switch (data.getSourceGameObj()->GetActor()->getActiveBehaviour()) {
	case (size_t)nAIEnums::AIEnums::eAIBehviours::IDLE:
	{
		// check physics to make sure the AI can be enacted...
		ai_behaviours.idle(data);
		break;
	}

	case (size_t)nAIEnums::AIEnums::eAIBehviours::SEEK:
	{
		ai_behaviours.seek(data);
		break;
	}

	case (size_t)nAIEnums::AIEnums::eAIBehviours::APPROACH:
	{
		ai_behaviours.approach(data);
		break;
	}

	case (size_t)nAIEnums::AIEnums::eAIBehviours::PURSURE:
	{
		ai_behaviours.pursure(data);
		break;
	}

	case (size_t)nAIEnums::AIEnums::eAIBehviours::FLEE:
	{
		ai_behaviours.flee(data);
		break;
	}

	case (size_t)nAIEnums::AIEnums::eAIBehviours::EVADE:
	{
		ai_behaviours.evade(data);
		break;
	}

	case (size_t)nAIEnums::AIEnums::eAIBehviours::WANDER:
	{
		ai_behaviours.wander(data);
		break;
	}

	case (size_t)nAIEnums::AIEnums::eAIBehviours::FOLLOW_PATH:
	{
		if (ai_behaviours.approach(data, wp.vec_waypointPositions[wp.waypointPos], wp.waypointRadius)) {
			if (!wp.isInvert) {
				if (wp.waypointPos != (wp.vec_waypointPositions.size() - 1))
					++wp.waypointPos;
				else
					wp.waypointPos = 0;
			}
			else {
				if (wp.waypointPos != 0)
					--wp.waypointPos;
				else
					wp.waypointPos = wp.vec_waypointPositions.size()-1;
			}
			mpModel["0 Test"]->positionXYZ = wp.vec_waypointPositions[wp.waypointPos];
		}
		break;
	}

	case (size_t)nAIEnums::AIEnums::eAIBehviours::SEPERATION:
	{
		ai_behaviours.seperation(data);
		glm::vec3 seperationForce = data.GetSteeringForce();
		data.getSourceGameObj()->ApplyImpulse(seperationForce);
		break;
	}

	case (size_t)nAIEnums::AIEnums::eAIBehviours::ALIGNMENT:
	{
		ai_behaviours.alignment(data);
		glm::vec3 seperationForce = data.GetSteeringForce();
		data.getSourceGameObj()->ApplyImpulse(seperationForce);
		break;
	}

	case (size_t)nAIEnums::AIEnums::eAIBehviours::COHESION:
	{
		ai_behaviours.cohesion(data);
		glm::vec3 seperationForce = data.GetSteeringForce();
		data.getSourceGameObj()->ApplyImpulse(seperationForce);
		break;
	}

	case (size_t)nAIEnums::AIEnums::eAIBehviours::FLOCK:
	{
		if (data.getSourceGameObj()->GetActor()->_formation_pos > 1) {
			ai_behaviours.seperation(data);
			glm::vec3 seperationForce = data.GetSteeringForce();

			ai_behaviours.alignment(data);
			glm::vec3 alignmentForce = data.GetSteeringForce();

			ai_behaviours.cohesion(data);
			glm::vec3 cohesionForce = data.GetSteeringForce();

			// APPLY FORCE WEIGHTS HERE...
			seperationForce *= seperationWeight;
			alignmentForce *= alignmentWeight;
			cohesionForce *= cohesionWeight;
			// TODO: Male the above configurable in an external file...

			glm::vec3 finalSteeringForce = seperationForce + alignmentForce + cohesionForce;
			//data.getSourceGameObj()->ApplyImpulse(finalSteeringForce);
			data.getSourceGameObj()->MutatePosition(data.getSourceGameObj()->getVelocity() + finalSteeringForce);
		}
		else {
			ai_behaviours.wander(data);
		}
		break;
	}

	case (size_t)nAIEnums::AIEnums::eAIBehviours::FLOCK_PATH:
	{
		if (data.getSourceGameObj()->GetActor()->_formation_pos > 1) {
			ai_behaviours.seperation(data);
			seperationForce = data.GetSteeringForce();

			ai_behaviours.alignment(data);
			alignmentForce = data.GetSteeringForce();

			ai_behaviours.cohesion(data);
			cohesionForce = data.GetSteeringForce();

			// APPLY FORCE WEIGHTS HERE...
			seperationForce *= seperationWeight;
			alignmentForce *= alignmentWeight;
			cohesionForce *= cohesionWeight;

			glm::vec3 finalSteeringForce = seperationForce + alignmentForce + cohesionForce;
			//data.getSourceGameObj()->ApplyImpulse(finalSteeringForce);
			data.getSourceGameObj()->MutatePosition(data.getSourceGameObj()->getVelocity() + finalSteeringForce);
		}

		if (ai_behaviours.approach(data, wp.vec_waypointPositions[wp.waypointPos], wp.waypointRadius)) {
			if (!wp.isInvert) {
				if (wp.waypointPos != (wp.vec_waypointPositions.size() - 1))
					++wp.waypointPos;
				else
					wp.waypointPos = 0;
			}
			else {
				if (wp.waypointPos != 0)
					--wp.waypointPos;
				else
					wp.waypointPos = wp.vec_waypointPositions.size() - 1;
			}
			mpModel["0 Test"]->positionXYZ = wp.vec_waypointPositions[wp.waypointPos];
		}


		break;
	}

	default:
		break;	
	}
}
#pragma endregion

void cAIManager::init_waypoints() {
	wp.waypointPos = 0;
	wp.waypointRadius = 10.f;
	wp.isInvert = false;
}

void cAIManager::add_waypoints() {
	wp.vec_waypointPositions.push_back(glm::vec3(0.f));
	wp.vec_waypointPositions.push_back(glm::vec3(20.f, 0.f, 30.f));
	wp.vec_waypointPositions.push_back(glm::vec3(60.f, 0.f, 50.f));
	wp.vec_waypointPositions.push_back(glm::vec3(100.f, 0.f, -50.f));
	wp.vec_waypointPositions.push_back(glm::vec3(10.f, 0.f, -75.f));
	wp.vec_waypointPositions.push_back(glm::vec3(-20.f, 0.f, 30.f));
	wp.vec_waypointPositions.push_back(glm::vec3(-25.f, 0.f, 50.f));
	wp.vec_waypointPositions.push_back(glm::vec3(-60.f, 0.f, 60.f));
	wp.vec_waypointPositions.push_back(glm::vec3(-75.f, 0.f, 100.f));
	wp.vec_waypointPositions.push_back(glm::vec3(-100.f, 0.f, -100.f));
	// have it add something to the model vector when these are created to see what it is hitting...
}

void cAIManager::add_waypoints(glm::vec3 waypoint) {
	wp.vec_waypointPositions.push_back(glm::vec3(waypoint));
	// have it add something to the model vector when these are created to see what it is hitting...
}

void cAIManager::add_waypoints(std::vector<glm::vec3> waypoints) {
	for (glm::vec3 waypoint : waypoints)
		wp.vec_waypointPositions.push_back(waypoint);
	// have it add something to the model vector when these are created to see what it is hitting...
}

void cAIManager::init_formationAnchor() {
	fa.position = glm::vec3(0.f);
	fa.centerPos = glm::vec3(0.f);
	fa.nodes = 0;
	fa.offset = 10.f;
	fa.circleoffset = 10.f;
	fa.radius = 20.f;
	fa.rowdef = 4;
	fa.columndef = 4;
}
