#pragma once
#include <glm/vec3.hpp>
#include "iPhysicsComponent.h"
#include <glm/gtx/quaternion.hpp>

namespace nPhysics
{
	// sBoxDef
	//
	// Holds all relevant information for an iBoxComponent, 
	// Necessary to build a concrete cBallComponent in a wrapper.
	struct sBoxDef {
		float Mass;
		glm::vec3 HalfExtents;
		glm::vec3 Velocity;
		glm::vec3 Position;
		glm::quat Rotation;
	};

	class iBoxComponent : public iPhysicsComponent {
	public:
		virtual ~iBoxComponent() {}

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
		iBoxComponent(const unsigned& id) : iPhysicsComponent(eComponentType::box, id) {}

	private:
		// Delete constructors so that they cannot be used
		iBoxComponent() = delete;
		iBoxComponent(const iBoxComponent& other) = delete;
		iBoxComponent& operator=(const iBoxComponent& other) = delete;
	};
}