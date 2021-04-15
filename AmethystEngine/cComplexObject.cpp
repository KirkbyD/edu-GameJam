#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cComplexObject.hpp"
#include "Globals.hpp"

cComplexObject::cComplexObject(std::vector<cComplexObject*> &vObjects)
	: _actor(nullptr)
{
	// TODO: check to see if this can be default initialized within the interface class ctor...
	this->friendlyIDNumber = 0;
	this->friendlyName = "";
	this->parentObject = nullptr;
	this->isVisible = true;
	this->isControl = false;
	this->m_uniqueID = next_uniqueID;

	vObjects.push_back(this);
}

cComplexObject::~cComplexObject() {
	// null the actor pointer, then delete it to prevent it deleting stuff that it should not
	// TODO: investigate to see if this should also delete the thing that it is pointing too as well
	physicsComponents.clear();
	modelPositions.clear();
	modelVector.clear();
	lightPositions.clear();
	lightVector.clear();
	delete _actor;
	delete TransformComponent;
	animaComponent = nullptr;
}

void cComplexObject::DeconstructEntities() {
	_actor = nullptr;
	for (cLightObject* light : lightVector)
		light->parentObject = nullptr;

	for (cModelObject* model : modelVector)
		model->parentObject = nullptr;

	delete TransformComponent;
	TransformComponent = nullptr;
}

void cComplexObject::AddLight(std::string lightName) {
	if (mpLight.find(lightName) != mpLight.end()) {
		this->lightVector.push_back(mpLight[lightName]);
		this->lightVector.back()->parentObject = this;
		this->lightVector.back()->isControl = false;
		this->lightPositions.push_back(this->lightVector.back()->getPosition());
		//this->lightVector.back()->direction = glm::vec4(mainCamera.GetFront(), 1.0f);
	}
}

void cComplexObject::AddLight(cLightObject* theLight) {
	this->lightVector.push_back(theLight);
	this->lightVector.back()->parentObject = this;
	this->lightVector.back()->isControl = false;
	this->lightPositions.push_back(theLight->getPosition());
	//this->lightVector.back()->direction = glm::vec4(mainCamera.GetFront(), 1.0f);
}

void cComplexObject::AddModel(std::string friendlyName) {
	cModelObject* theModel = pFindObjectByFriendlyName(friendlyName);
	this->modelVector.push_back(theModel);
	this->modelVector.back()->parentObject = this;
	this->modelVector.back()->isControl = false;
}

void cComplexObject::AddModel(cModelObject* theModel) {
	this->modelVector.push_back(theModel);
	this->modelVector.back()->parentObject = this;
	this->modelVector.back()->isControl = false;
}

void cComplexObject::AddPhysicsComponent(nPhysics::iPhysicsComponent* component) {
	this->physicsComponents.push_back(component); 
	component->SetParentID(this->friendlyIDNumber);
	component->SetParentPointer(this);
}

void cComplexObject::AddActorComponent(cActorObject* component) { this->_actor = component; }

void cComplexObject::AddAnimation(cAnimationComponent* component) { 
	component->parentObject = this;
	component->parentIDNumber = this->friendlyIDNumber;
	this->animaComponent = component;
}

void cComplexObject::AddTransformComponent(cTransformComponent* component) {
	this->TransformComponent = component;
	component->parentObject = this;
}

bool cComplexObject::QueueAnimation(std::string AnimaName) {
	if (HasAnimations()) {
		return animaComponent->QueueAnimation(AnimaName);
	}
}

void cComplexObject::QueueBlendAnimation(std::string AnimaName) {
	if (HasAnimations()) {
		animaComponent->QueueBlendAnimation(AnimaName);
	}
}

bool cComplexObject::RemoveLight(cLightObject* theLight){
	for (size_t i = 0; i < lightVector.size(); i++)	{
		if (lightVector[i] = theLight) {
			lightPositions.erase(lightPositions.begin() + (i));
			lightVector.erase(lightVector.begin() + (i));
			return true;
		}
	}
	return false;
}

bool cComplexObject::RemovePhysicsComponent(nPhysics::iPhysicsComponent* component) {
	for (auto it = this->physicsComponents.begin();
		it != physicsComponents.end();
		it++) {
		if (component == *it) {
			physicsComponents.erase(it);
			return true;
		}
	}
	return false;
}

void cComplexObject::ApplyImpulse(glm::vec3 impulse) {
	for (auto component : this->physicsComponents) {
		switch (component->GetComponentType()) {
		case nPhysics::eComponentType::ball:
			((nPhysics::iBallComponent*)component)->ApplyImpulse(impulse);
			break;
		case nPhysics::eComponentType::characterController:
			((nPhysics::iCharacterController*)component)->SetLinearVelocity(impulse);
			break;
		}
	}
}

void cComplexObject::VelocityZero() {
	for (auto component : this->physicsComponents) {
		switch (component->GetComponentType()) {
		case nPhysics::eComponentType::ball:
			((nPhysics::iBallComponent*)component)->Stop();
			break;
		case nPhysics::eComponentType::characterController:
			((nPhysics::iCharacterController*)component)->Stop();
			break;
		}
	}
}

void cComplexObject::Jump(glm::vec3 jumpVel) {
	for (auto component : this->physicsComponents) {
		switch (component->GetComponentType()) {
		case nPhysics::eComponentType::characterController:
			((nPhysics::iCharacterController*)component)->Jump(jumpVel);
			break;
		}
	}
}

void cComplexObject::ClearPhysicsComponents() {
	this->physicsComponents.clear();
}

void cComplexObject::SetOrientation(glm::quat orientationIn) {
	this->TransformComponent->setOrientation(orientationIn);
	for (auto component : this->physicsComponents) {
		switch (component->GetComponentType()) {
		case nPhysics::eComponentType::characterController:
			((nPhysics::iCharacterController*)component)->SetOrientation(orientationIn);
			break;
		}
	}
}

void cComplexObject::MutateOrientation(glm::vec3 eulerAngle) {
	this->TransformComponent->mutateOrientation(eulerAngle);
	for (auto component : this->physicsComponents) {
		switch (component->GetComponentType()) {
		case nPhysics::eComponentType::characterController:
			((nPhysics::iCharacterController*)component)->SetOrientation(TransformComponent->getQuaternion());
			break;
		}
	}
}

void cComplexObject::SetPosition(glm::vec3 positionIn) {
	this->TransformComponent->setPosition(positionIn);
	for (auto component : this->physicsComponents) {
		switch (component->GetComponentType()) {
		case nPhysics::eComponentType::characterController:
			((nPhysics::iCharacterController*)component)->SetPosition(positionIn);
			break;
		}
	}
}

unsigned cComplexObject::getUniqueID() { return this->m_uniqueID; }

glm::vec3 cComplexObject::getPosition() {
	return this->TransformComponent->getPosition();
}

glm::vec3 cComplexObject::getVelocity() {
	glm::vec3 vel(0.f);
	if (this->physicsComponents.size() != 0) {
		this->physicsComponents[0]->GetVelocity(vel);
	}
	return vel;
}

glm::vec3 cComplexObject::getEulerAngle() {
	return this->TransformComponent->getEulerAngle();
}

glm::quat cComplexObject::getQuaternion() {
	return this->TransformComponent->getQuaternion();
}

glm::vec3 cComplexObject::getFrontVector() {
	return this->TransformComponent->getFrontVector();
}

glm::vec3 cComplexObject::getBonePositionByBoneName(std::string Name) {
	return glm::vec4(glm::rotate(getQuaternion(), animaComponent->getBonePositionByBoneNameModelSpace(Name)), 1.f) + getWorldMatrix()[3];
}

std::vector<cModelObject*> cComplexObject::GetModels() { return this->modelVector; }
std::vector<cLightObject*> cComplexObject::GetLights() { return this->lightVector; }

std::vector<nPhysics::iPhysicsComponent*> cComplexObject::GetPhysicsComponents() { return this->physicsComponents; }
cActorObject* cComplexObject::GetActor() { return this->_actor; }
