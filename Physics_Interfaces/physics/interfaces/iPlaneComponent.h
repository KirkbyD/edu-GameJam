#pragma once
#include <glm/vec3.hpp>
#include "iPhysicsComponent.h"

namespace nPhysics
{
	// sPlaneDef
	//
	// Holds all relevant information for an iPlaneComponent, 
	// Necessary to build a concrete cPlaneComponent in a wrapper.
	struct sPlaneDef
	{
		float Constant;
		glm::vec3 Normal;
	};

	class iPlaneComponent : public iPhysicsComponent
	{
	public:
		virtual ~iPlaneComponent() {}

		// Returns relevant information for recreating the plane.
		// Used for hotswapping physics and potentially for saving.
		virtual bool GetBuildInfo(glm::vec3& normal, float& constant) = 0;

	protected:
		iPlaneComponent(const unsigned& id) : iPhysicsComponent(eComponentType::plane, id) {}

	private:
		// Delete constructors so that they cannot be used
		//iPlaneComponent() = delete;
		iPlaneComponent(const iPlaneComponent& other) = delete;
		iPlaneComponent& operator=(const iPlaneComponent& other) = delete;
	};
}