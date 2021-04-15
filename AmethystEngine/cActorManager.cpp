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
#include "cActorManager.hpp"
#include "cModelObject.hpp"
#include "cGameObjectFactory.hpp"
#include "nAI.hpp"
#include "nAIEnums.hpp"

extern std::vector<cModelObject*> g_vec_pGameObjects;
cGameObjectFactory* ActorMngrFactory;

#pragma region SINGLETON
cActorManager cActorManager::stonActMngr;
cActorManager* cActorManager::GetActorManager() { return &(cActorManager::stonActMngr); }
cActorManager::cActorManager()
	: pMediator(nullptr)
{ std::cout << "Actor Manager Created" << std::endl; }
#pragma endregion



void cActorManager::update(sData& data) {
	// check the actors decision trees...
	// stop being stupid and add the actors to the damned complex objects...
	float dot = 0.0f;
	float dist = 0.0f;
	bool isWander = true;
	glm::vec3 targetOrientation;
	for (nDecisionTree::cDecision* dt : data.getSourceGameObj()->GetActor()->GetDecisionTree()) {
		// do the checks for the appropriate decision triggers...
		
		switch (*data.getSourceGameObj()->GetActor()->getEnemyType()) {
		case nDecisionTree::eEnemyType::TYPE_A:
		{
			// Step 1: Check the type of trigger...
			// WAY THIS SHOULD BE DONE ONCE OBJECT FACING IS NO LONGER BOUND TO CAMERA
			//targetOrientation = data.getTargetGameObj()->getEulerAngle();
			//targetOrientation.x = cos(targetOrientation.x) * cos(targetOrientation.y);
			//targetOrientation.y = sin(targetOrientation.y);
			//targetOrientation.z = sin(targetOrientation.x) * cos(targetOrientation.y);
			//targetOrientation = glm::normalize(targetOrientation) * -1.0f;

			data.setSubsystem(CAMERA);
			data.setCMD(GET_ACTIVE_FRONT);
			pMediator->RecieveMessage(data);
			targetOrientation = data.getVec4Data().front();

			dot = glm::dot(targetOrientation, glm::normalize(data.getSourceGameObj()->getPosition() - data.getTargetGameObj()->getPosition()));

			if (dot > 0.95f) {
				for (nDecisionTree::cDecision* aib : data.getSourceGameObj()->GetActor()->GetDecisionTree()) {
					if (aib->behaviour == (size_t)nAIEnums::AIEnums::eAIBehviours::FLEE)
						data.getSourceGameObj()->GetActor()->setActiveBehaviour((size_t)aib->behaviour);
				}
			}
			else {
				for (nDecisionTree::cDecision* aib : data.getSourceGameObj()->GetActor()->GetDecisionTree()) {
					if (aib->behaviour == (size_t)nAIEnums::AIEnums::eAIBehviours::SEEK)
						data.getSourceGameObj()->GetActor()->setActiveBehaviour((size_t)aib->behaviour);
				}
			}

			/*
				for line of sight use this type of idea (from unity)
				float dot = Vector3.Dot(transform.forward, (other.position - transform.position).normalized);
				if(dot > 0.7f) { Debug.Log("Quite facing");}


				this->front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
				this->front.y = sin(glm::radians(this->pitch));
				this->front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
				this->front = glm::normalize(this->front);

			*/
			break;
		}

		case nDecisionTree::eEnemyType::TYPE_B:
		{
			// pursues the player and evades closest bullet fired by the player...
			for (cModelObject* pco : g_vec_pGameObjects) {
				dist = 0.0f;
				if (pco->actorOwned == "theFighter")
					dist = glm::distance(pco->positionXYZ, data.getSourceGameObj()->getPosition());

				if (dist < data.GetCircleRadius()) {
					// is the bullet close enough
					data.setTargetPos(pco->positionXYZ);
					data.setTargetVel(pco->velocity);
					for (nDecisionTree::cDecision* aib : data.getSourceGameObj()->GetActor()->GetDecisionTree()) {
						if (aib->behaviour == (size_t)nAIEnums::AIEnums::eAIBehviours::EVADE)
							data.getSourceGameObj()->GetActor()->setActiveBehaviour((size_t)aib->behaviour);
					}
				}
			}

			// otherwise pursure
			for (nDecisionTree::cDecision* aib : data.getSourceGameObj()->GetActor()->GetDecisionTree()) {
				if (aib->behaviour == (size_t)nAIEnums::AIEnums::eAIBehviours::PURSURE)
					data.getSourceGameObj()->GetActor()->setActiveBehaviour((size_t)aib->behaviour);
			}

			break;
		}

		case nDecisionTree::eEnemyType::TYPE_C:
		{
			// AI TYPE C
			// approaches the player and maintains a radius, if in that radius faces the player and fires at them...
			// check for firing the bullet if inside the radius...
			dist = glm::distance(data.getTargetGameObj()->getPosition(), data.getSourceGameObj()->getPosition());
			if (dist < data.GetCircleRadius()) {
				// fire bullet its inside the radius...
				iGameObject* created = ActorMngrFactory->CreateMediatedGameObject("model", "Sphere", "enemyBullet");
				cModelObject* theBullet = ((cModelObject*)created);
				theBullet->positionXYZ = data.getSourceGameObj()->getPosition();
				glm::vec3 vel;

				theBullet->velocity = data.getSourceGameObj()->getVelocity() + (100.0f * (glm::normalize(data.getSourceGameObj()->getPosition() - data.getTargetGameObj()->getPosition())));
				theBullet->scale = 0.5f;
				theBullet->v_textureNames = { "Fire" };
				theBullet->v_texureBlendRatio = { 1.0f };
				theBullet->inverseMass = 1.0f;
				theBullet->actorOwned = "enemy";
				theBullet->physicsShapeType = eShapeTypes::SPHERE;
			}

			for (nDecisionTree::cDecision* aib : data.getSourceGameObj()->GetActor()->GetDecisionTree()) {
				if (aib->behaviour == (size_t)nAIEnums::AIEnums::eAIBehviours::APPROACH)
					data.getSourceGameObj()->GetActor()->setActiveBehaviour((size_t)aib->behaviour);
			}

			break;
		}

		case nDecisionTree::eEnemyType::TYPE_D:
		{
			// AI TYPE D
			// wanders around for 6 seconds then switches to idle for 3 seconds before resuming wandering, action repeats at 6 and 3 second intervals...
			if (data.GetElapsedTime() < 6.0f) {
				data.IncrementElapsedTime();
				for (nDecisionTree::cDecision* aib : data.getSourceGameObj()->GetActor()->GetDecisionTree()) {
					if (aib->behaviour == (size_t)nAIEnums::AIEnums::eAIBehviours::WANDER)
						data.getSourceGameObj()->GetActor()->setActiveBehaviour((size_t)aib->behaviour);
				}
			}
			else if (data.GetElapsedTime() < 9.0f) {
				data.IncrementElapsedTime();
				for (nDecisionTree::cDecision* aib : data.getSourceGameObj()->GetActor()->GetDecisionTree()) {
					if (aib->behaviour == (size_t)nAIEnums::AIEnums::eAIBehviours::IDLE)
						data.getSourceGameObj()->GetActor()->setActiveBehaviour((size_t)aib->behaviour);
				}
			}
			else
				data.ResetElaspedTime();

			break;
		}

		default:
			break;		// HAS NO AI ASSIGNED TO IT
		}
	}
}

#pragma region MEDIATOR SETUP
void cActorManager::setMediatorPointer(iMediatorInterface* pMediator) { this->pMediator = pMediator; }
#pragma endregion



#pragma region MEDIATOR COMMUNICATIONS
sData cActorManager::RecieveMessage(sData& data) {
	switch (data.getCMD()) {
	case UPDATE:
	{
		this->update(data);
		break;
	}

	case SET_ACTIVE_AI:
	{
		for (auto a : g_vec_pActorObjects)
			a->setActiveBehaviour(data.GetSteeringBehaviour());
		break;
	}

	case SFORMATION:
	{
		if (!SetFormations(data))
			data.setResult(NOK);
		break;
	}

	case ACTIVATE_FORMATIONS:
	{
		if (!SelectActors())
			data.setResult(NOK);
		break;
	}

	case DEACTIVATE_FORMATIONS:
	{
		if (!UnselectActors())
			data.setResult(NOK);
		break;
	}

	case UNKN_CMD:
	{
		data.setResult(UNKNOWN_COMMAND);
		break;
	}

	default:
	{
		data.setResult(INVALID_COMMAND);
		break;
	}

	}

	return data;
}
#pragma endregion



#pragma region MEDIATOR COMMAND FUNCTIONS
bool cActorManager::UnselectActors() {
	for (cActorObject* cao : g_vec_pActorObjects) {
		if(cao->isSelected() == true)
			cao->ToggleSelected();
		
		if (cao->isSelected() == true)
			return false;
	}
	return true;
}

bool cActorManager::SelectActors() {
	for (cActorObject* cao : g_vec_pActorObjects) {
		if (cao->isSelected() == false)
			cao->ToggleSelected();

		if (cao->isSelected() == false)
			return false;
	}
	return true;
}

bool cActorManager::SetFormations(sData& data) {
	for (cActorObject* cao : g_vec_pActorObjects)
		cao->setActiveFormation(data.GetFormation());

	for (cActorObject* cao : g_vec_pActorObjects) {
		if (cao->getActiveFormation() != data.GetFormation()) {
			return false;
		}
	}
	return true;
}

// this should be used to set the position of each object
// the 0 position is considered the anchor and the formation will be
// formed based off of the anchor unit
void cActorManager::AssignFormationPositions() {
	size_t pos = 0;
	for (auto a : g_vec_pActorObjects) {
		if (a->isSelected()) {
			a->_formation_pos = pos;
			++pos;
		}
	}
}
#pragma endregion


#pragma region SYSTEM FUNCTION CALLS
size_t cActorManager::GetActorBehaviour(sData &data) { return data.getSourceGameObj()->GetActor()->getActiveBehaviour(); }
#pragma endregion
