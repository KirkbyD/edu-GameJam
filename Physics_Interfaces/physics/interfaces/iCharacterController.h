#pragma once
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include "iPhysicsComponent.h"

namespace nPhysics
{
	// sCharacterDef
	//
	// Holds all relevant information for an iCharacterController, 
	// Necessary to build a concrete cCharacterController in a wrapper.
	struct sCharacterDef {
		float Radius;
		float Height;
		float StepHeight;
		glm::vec3 Position;
		glm::quat Rotation;
	};

	class iCharacterController : public iPhysicsComponent {
	public:
		virtual ~iCharacterController() {}

		virtual void SetOrientation(glm::quat orientationIn) = 0;

		// Apply an impulse to this ball component.
		// Adds directly to velocity.
		virtual void SetLinearVelocity(glm::vec3 velIn) = 0;

		// Set velocity to 0;
		virtual void Stop() = 0;

		//TODO: figure out how it actually works in bullet first though!
		virtual void Jump(glm::vec3 jumpVel) = 0;

	protected:
		// Automatically fulfills interface requirement when called
		iCharacterController(const unsigned& id) : iPhysicsComponent(eComponentType::characterController, id) {}

	private:
		// Delete constructors so that they cannot be used
		iCharacterController() = delete;
		iCharacterController(const iCharacterController& other) = delete;
		iCharacterController& operator=(const iCharacterController& other) = delete;
	};
}
