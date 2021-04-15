#pragma once
#pragma once
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include "iPhysicsComponent.h"

class cMesh;

namespace nPhysics
{
	// sTriangleMeshDef
	//
	// Holds all relevant information for an iTriangleMeshComponent, 
	// Necessary to build a concrete cTriangleMeshComponent in a wrapper.
	struct sTriangleMeshDef
	{
		cMesh* Mesh;
		glm::vec3 Scale;
		glm::vec3 Position;
		glm::quat Rotation;
	};

	class iTriangleMeshComponent : public iPhysicsComponent
	{
	public:
		virtual ~iTriangleMeshComponent() {}

		// Returns relevant information for recreating the TriangleMesh.
		// Used for hotswapping physics and potentially for saving.
		virtual bool GetBuildInfo(glm::vec3& normal, float& constant) = 0;

	protected:
		iTriangleMeshComponent(const unsigned& id) : iPhysicsComponent(eComponentType::triangleMesh, id) {}

	private:
		// Delete constructors so that they cannot be used
		iTriangleMeshComponent() = delete;
		iTriangleMeshComponent(const iTriangleMeshComponent& other) = delete;
		iTriangleMeshComponent& operator=(const iTriangleMeshComponent& other) = delete;
	};
}