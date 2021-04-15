#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include <iostream>`
#include "cAnimationManager.hpp"
#include "nConvert.hpp"
#include <glm\gtc\type_ptr.hpp>


#pragma region SINGLETON
cAnimationManager cAnimationManager::stonAnimaMngr;
cAnimationManager* cAnimationManager::GetAnimationManager() { return &(cAnimationManager::stonAnimaMngr); }
cAnimationManager::cAnimationManager() {
	std::cout << "Animation Manager Created" << std::endl;
	pMediator = nullptr;
}
#pragma endregion

extern long PlayerHealth;	//Todo: move to specialized entity object!

void cAnimationManager::DeconstructAnimationComponents() {
	for (std::pair<std::string, cAnimationComponent*> it : mpAnima) {
		delete it.second;
	}
	mpAnima.clear();
}

cAnimationComponent* cAnimationManager::LoadMeshFromFile(const std::string& friendlyName, const std::string& filename) {
	cAnimationComponent* AnimaObj = dynamic_cast<cAnimationComponent*>(this->_fact_game_obj.CreateGameObject("[ANIMATION]"));

	if (AnimaObj->LoadMeshFromFile(friendlyName, filename)) {
		AnimaObj->friendlyIDNumber = Module_Hex_Value | System_Hex_Value | next_UniqueComponentID++;
		mpAnima[friendlyName] = AnimaObj;
		return AnimaObj;
	}
	return nullptr;
}

void cAnimationManager::IterateComponent(cAnimationComponent* component, float dt) {
	int* AnimaTime = component->GetTime();

	float TicksPerSecond = static_cast<float>(component->GetScene()->mAnimations[0]->mTicksPerSecond != 0 ?
											  component->GetScene()->mAnimations[0]->mTicksPerSecond : 25.0);
	float TimeInTicks = dt * TicksPerSecond;
	//Keep us on the same animation we're blending fromc
	if (AnimaTime[0] > (float)component->GetScene()->mAnimations[0]->mDuration&& component->isBlending())
		AnimaTime[0] = (float)component->GetScene()->mAnimations[0]->mDuration;
	else
		AnimaTime[0] += TimeInTicks;

	if (component->isBlending()) {
		//TicksPerSecond = static_cast<float>(component->GetScene()->mAnimations[1]->mTicksPerSecond != 0 ?
		//									component->GetScene()->mAnimations[1]->mTicksPerSecond : 25.0);
		//TimeInTicks = dt * TicksPerSecond;
		AnimaTime[1] += TimeInTicks;
	}
	
	//check prev anim
	std::string currAnim = component->GetPrevAnimation();
	bool wasIdle = (currAnim == "Idle" || currAnim == "FightIdle") ? true : false;

	cComplexObject* pParent = dynamic_cast<cComplexObject*>(component->GetParent());

	// if a blended animation has been going long enough, swap to it.
	// 30 ticks at 60 fps animation for 1/2 second blends
	if (component->isBlending() 
		&& ((wasIdle && AnimaTime[1] >= 10) || AnimaTime[1] >= 20)) {
		component->EndBlend();

		if (!wasIdle) {
			ResolvePreviousAnimation(currAnim, component, pParent);
		}
		currAnim = pParent->getCurrentAnimationName()[0];
		DispatchNewAnimation(currAnim, component, pParent);
		component->SetPrevAnimation(currAnim);
	}

	//otherwise check if main animation has ended.
	else if (AnimaTime[0] > (float)component->GetScene()->mAnimations[0]->mDuration /*|| wasIdle*/) {
		AnimaTime[0] = 0;
		AnimaTime[1] = 0;
		component->SetTime(AnimaTime);
		
		if (!wasIdle) {
			ResolvePreviousAnimation(currAnim, component, pParent);
		}

		component->ClearAnimation();

		if (component->QueueEmpty()) {
			//Check for alternate Idle Transitions
			if (currAnim == "IdleToFight" || currAnim == "FightIdle" || currAnim == "Punch") {
				component->QueueAnimation("FightIdle");
			}
			else
			component->QueueAnimation("Idle");
		}

		component->NextAnimation();		
	}

	currAnim = pParent->getCurrentAnimationName()[0];
	DispatchNewAnimation(currAnim, component, pParent);
	component->SetPrevAnimation(currAnim);
	return;
}

void cAnimationManager::ResolvePreviousAnimation(std::string currAnim, cAnimationComponent* component, cComplexObject* pParent) {
	if (currAnim == "Fall") {
		component->QueueAnimation("Fall");
	}
	else if (currAnim == "Land" && component->GetNextAnimation() == "Fall") {
		component->ClearNextAnimation();
	}
	//Check if we turned
	else if (currAnim == "TurnL") {
		glm::vec3 right = glm::cross(glm::vec3(0.f, 1.f, 0.f), pParent->getFrontVector());
		pParent->SetOrientation(glm::quatLookAt(-right, glm::vec3(0.f, 1.f, 0.f)));
	}
	else if (currAnim == "TurnR") {
		glm::vec3 right = glm::cross(glm::vec3(0.f, 1.f, 0.f), pParent->getFrontVector());
		pParent->SetOrientation(glm::quatLookAt(right, glm::vec3(0.f, 1.f, 0.f)));
	}
	//Check if it was our 'Dodge' (need to move further for the dodge specifically)
	else if (currAnim == "DodgeL") {
		pParent->SetPosition(pParent->getBonePositionByBoneName("mixamorig:LeftFoot"));
	}
	else if (currAnim == "DodgeR") {
		pParent->SetPosition(pParent->getBonePositionByBoneName("mixamorig:RightFoot"));
	}
	//Check if we died!
	else if (currAnim == "Death") {
		pParent->SetPosition(glm::vec3(100.f));
		PlayerHealth = 20000;
	}
}

void cAnimationManager::DispatchNewAnimation(std::string currAnim, cAnimationComponent* component, cComplexObject* pParent) {
	//Dispatch physics Impulses
	float accelSpeed = 1.5f;
	if (pParent->friendlyName == "Elf")
		accelSpeed = 1.5f;
	else if (pParent->friendlyName == "Zombie")
		accelSpeed = 0.8;

	if (currAnim == "Idle"
		|| currAnim == "FightIdle"
		|| currAnim == "IdleToFight"
		|| currAnim == "FightToIdle"
		|| currAnim == "Punch"
		|| currAnim == "Death"
		|| currAnim == "Land") {
		pParent->VelocityZero();
	}
	else if (currAnim == "JumpF" || currAnim == "JumpB") {
		//Check if should jump backwards
		if (currAnim == "JumpF") {
			glm::vec3 front = pParent->getFrontVector();
			glm::vec3 vel = pParent->getVelocity();
			if (glm::dot(front, vel) < 0.f) {
				component->ClearAnimation();
				component->QueueAnimation("JumpB");
				//pParent->Jump(glm::vec3(0.f, 1.f, 0.f));
			}
			/*else
				pParent->Jump(glm::vec3(0.f, 2.f, 0.f));*/
		}
	}
	else {
		pParent->VelocityZero();
		if (currAnim == "WalkF") {
			glm::vec3 front = pParent->getFrontVector();
			pParent->ApplyImpulse(glm::vec3(front.x * accelSpeed, 0.f, front.z * accelSpeed) * .75f);
		}
		else if (currAnim == "WalkB") {
			glm::vec3 front = pParent->getFrontVector();
			pParent->ApplyImpulse(glm::vec3(front.x * -accelSpeed, 0.f, front.z * -accelSpeed) * .5f);
		}
		else if (currAnim == "WalkL") {
			glm::vec3 right = glm::cross(glm::vec3(0.f, 1.f, 0.f), pParent->getFrontVector());
			pParent->ApplyImpulse(glm::vec3(right.x * accelSpeed, 0.f, right.z * accelSpeed) * .5f);
		}
		else if (currAnim == "WalkR") {
			glm::vec3 right = glm::cross(glm::vec3(0.f, 1.f, 0.f), pParent->getFrontVector());
			pParent->ApplyImpulse(glm::vec3(right.x * -accelSpeed, 0.f, right.z * -accelSpeed) * .5f);
		}
		else if (currAnim == "RunF") {
			glm::vec3 front = pParent->getFrontVector();
			pParent->ApplyImpulse(glm::vec3(front.x * accelSpeed, 0.f, front.z * accelSpeed) * 1.5f);
		}
		else if (currAnim == "RunB") {
			glm::vec3 front = pParent->getFrontVector();
			pParent->ApplyImpulse(glm::vec3(front.x * -accelSpeed, 0.f, front.z * -accelSpeed));
		}
		else if (currAnim == "RunL") {
			glm::vec3 right = glm::cross(glm::vec3(0.f, 1.f, 0.f), pParent->getFrontVector());
			pParent->ApplyImpulse(glm::vec3(right.x * accelSpeed, 0.f, right.z * accelSpeed));
		}
		else if (currAnim == "RunR") {
			glm::vec3 right = glm::cross(glm::vec3(0.f, 1.f, 0.f), pParent->getFrontVector());
			pParent->ApplyImpulse(glm::vec3(right.x * -accelSpeed, 0.f, right.z * -accelSpeed));
		}
	}
}

void cAnimationManager::BlendTransforms(std::pair<std::string, cAnimationComponent*> it) {
	std::vector<float> ratios = it.second->GetBlendRatios();

	std::vector<glm::mat4x4>* vecFinalTransform = mpFinalTransformation[it.first];

	for (unsigned int index = 0; index != vecFinalTransform[0].size(); index++)	{
		glm::mat4x4 matBlended;

		matBlended = vecFinalTransform[0][index] * ratios[index] +
					 vecFinalTransform[1][index] * (1.f - ratios[index]);

		glm::mat4 matAnotherTransform = glm::mat4(1.0f);
		vecFinalTransform[0][index] = matAnotherTransform * matBlended;
	}
}

void cAnimationManager::Update(float dt, GLint shaderProgID, cRenderer* pRenderer, cVAOManager* pVAOManager) {
	for (auto it : mpAnima) {
		auto pCurrentObject = it.second;

		IterateComponent(pCurrentObject, dt);

		// Set to all identity
		const int NUMBEROFBONES = pCurrentObject->GetNumBones();

		// Always get trnsforms for base animation
		// This loads the bone transforms from the animation model
		pCurrentObject->BoneTransform(0,
									  mpFinalTransformation[it.first][0],
									  mpOffsets[it.first][0],
									  mpObjectBoneTransforms[it.first][0]);

		//check if component is blending
		//only ever blend up to 2 animations.
		if (pCurrentObject->isBlending()) {
			pCurrentObject->BoneTransform(1,
										  mpFinalTransformation[it.first][1],
										  mpOffsets[it.first][1],
										  mpObjectBoneTransforms[it.first][1]);
			BlendTransforms(it);
		}

		// Wait until all threads are done updating.
	}
	
	return;
}

void cAnimationManager::Render(cRenderer* pRenderer, GLint shaderProgID, cVAOManager* pVAOManager) {
	for (auto it : mpAnima) {
		auto pCurrentObject = it.second;

		//Just getting the number of bones
		GLint numBonesUsed = (GLint)mpFinalTransformation[it.first][0].size();

		std::vector<sModelDrawInfo*> vecDrawInfo = it.second->GetMeshes();

		for (size_t i = 0; i < vecDrawInfo.size(); i++) {
			pRenderer->RenderAnimaObject(pCurrentObject, vecDrawInfo[i], pCurrentObject->GetTextures()[i], shaderProgID, pVAOManager, numBonesUsed, glm::value_ptr(mpFinalTransformation[it.first][0][0]));
		}
	}
	return;
}



#pragma region MEDIATOR_COMMUNICATION
void cAnimationManager::setMediatorPointer(iMediatorInterface* pMediator) {
	this->pMediator = pMediator;
	return;
}

sData cAnimationManager::RecieveMessage(sData& data) {
	data.setResult(OK);

	return data;
}
#pragma endregion
