#pragma once
#include <glm/vec3.hpp>
#include "iPhysicsComponent.h"

namespace nPhysics
{
	// sBallDef
	//
	// Holds all relevant information for an iBallComponent, 
	// Necessary to build a concrete cBallComponent in a wrapper.
	struct sBallDef	{
		float Mass;
		float Radius;
		glm::vec3 Velocity;
		glm::vec3 Position;
		//add restitution value
	};

	class iBallComponent : public iPhysicsComponent
	{
	public:
		virtual ~iBallComponent() {}

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
		iBallComponent(const unsigned& id) : iPhysicsComponent(eComponentType::ball, id) {}

	private:
		// Delete constructors so that they cannot be used
		//iBallComponent() = delete;
		iBallComponent(const iBallComponent& other) = delete;
		iBallComponent& operator=(const iBallComponent& other) = delete;
	};
}