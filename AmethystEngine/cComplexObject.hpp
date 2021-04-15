#pragma once
#include "cModelObject.hpp"
#include "cLightObject.hpp"
#include "iGameObject.hpp"
#include <vector>
#include <string>
#include <glm/gtx/quaternion.hpp>

#include <physics/interfaces/PhysicsInterfaces.h>

#include "cActorObject.hpp"
#include "cAnimationComponent.hpp"
#include "cTransformComponent.hpp"
extern class cLightObject;

struct collisionPoint {
	glm::vec3 RelativePos;
	glm::vec3 Position;
	float Collided;
};

class cComplexObject : public iGameObject {
private:
	//Transform
	cTransformComponent* TransformComponent;

	// Physics
	std::vector<nPhysics::iPhysicsComponent*> physicsComponents;

	// Actors
	cActorObject* _actor;

	//Animations
	cAnimationComponent* animaComponent;

	// Members
	std::vector<glm::vec3> modelPositions;
	std::vector<cModelObject*> modelVector;
	std::vector<glm::vec3> lightPositions;
	std::vector<cLightObject*> lightVector;

	void RotateChildren();

	const static size_t System_Hex_Value = (0x04 << 16);
	const static size_t Module_Hex_Value = ((uint64_t)0x000 << 32);


public:
	
	cComplexObject(std::vector<cComplexObject*> &vObjects);
	~cComplexObject();

	void DeconstructEntities();


	std::vector<cModelObject*> GetModels();
	std::vector<cLightObject*> GetLights();
	std::vector<nPhysics::iPhysicsComponent*> GetPhysicsComponents();
	cActorObject* GetActor();

	bool HasAnimations() { return (animaComponent != nullptr) ? true : false; }
	bool HasActor() { return (_actor != nullptr) ? true : false; }

	void AddLight(std::string);
	void AddLight(cLightObject* theLight);
	void AddModel(std::string);	
	void AddModel(cModelObject* theModel);
	void AddPhysicsComponent(nPhysics::iPhysicsComponent* component);
	void AddActorComponent(cActorObject* component);
	void AddAnimation(cAnimationComponent* component);
	void AddTransformComponent(cTransformComponent* component);

	bool QueueAnimation(std::string AnimaName);
	void QueueBlendAnimation(std::string AnimaName);

	bool RemoveLight(cLightObject* theLight);
	bool RemovePhysicsComponent(nPhysics::iPhysicsComponent* component);

	//Apply an impulse to all member components
	void ApplyImpulse(glm::vec3 impulse);
	void VelocityZero();
	void Jump(glm::vec3 jumpVel);

	//Clear all existing physics components from entity (used in physics terminate)
	void ClearPhysicsComponents();

	glm::vec3 getVelocity();

	glm::vec3 getEulerAngle();
	glm::quat getQuaternion();
	glm::vec3 getFrontVector();
	glm::mat4 getWorldMatrix() { return TransformComponent->RecalculateWorldMatrix(); }
	glm::vec3 getBonePositionByBoneName(std::string Name);
	std::string* getCurrentAnimationName() { if (animaComponent != nullptr) return animaComponent->GetCurrentAnimation(); }

	float GetScale() { return this->TransformComponent->getScale(); }
	void SetScale(float scaleIn) { this->TransformComponent->setScale(scaleIn); }
	void MutatePosition(glm::vec3 movement) { this->TransformComponent->mutatePosition(movement); }
	void SetTransform(glm::mat4 transformIn) { this->TransformComponent->setMatrix(transformIn); }
	void SetOrientation(glm::quat orientationIn);
	void MutateOrientation(glm::vec3 eulerAngle);
	void SetPosition(glm::vec3 positionIn);

	virtual unsigned getUniqueID();
	virtual glm::vec3 getPosition();
};
