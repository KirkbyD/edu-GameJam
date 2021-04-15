#pragma once
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include "iPhysicsComponent.h"

namespace nPhysics
{
	// sCapsuleDef
	//
	// Holds all relevant information for an iCapsuleComponent, 
	// Necessary to build a concrete cBallComponent in a wrapper.
	struct sCapsuleDef {
		float Mass;
		float Height;
		float Radius;
		glm::vec3 Velocity;
		glm::vec3 Position;
		glm::quat Rotation;
	};

	class iCapsuleComponent : public iPhysicsComponent
	{
	public:
		virtual ~iCapsuleComponent() {}

		// Apply an impulse to this ball component.
		// Adds directly to velocity.
		virtual void ApplyImpulse(glm::vec3 impulseIn) = 0;

		// Set velocity to 0;
		virtual void Stop() = 0;

		// Returns relevant information for recreating the ball.
		// Used for hotswapping physics and potentially for saving.
		virtual bool GetBuildInfo(glm::vec3& position, glm::vec3& velocity, float& radius, float& mass) = 0;

	protected:
		// Automatically fulfills interface requirement when called
		iCapsuleComponent(const unsigned& id) : iPhysicsComponent(eComponentType::capsule, id) {}

	private:
		// Delete constructors so that they cannot be used
		iCapsuleComponent() = delete;
		iCapsuleComponent(const iCapsuleComponent& other) = delete;
		iCapsuleComponent& operator=(const iCapsuleComponent& other) = delete;
	};
}