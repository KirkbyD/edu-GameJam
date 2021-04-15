#pragma once


namespace nPhysics {
	class cWrapperDebugRenderer : public phys::iDebugRenderer {
	private:
		iDebugRenderer* mDebugRenderer;

	public:
		//Constructor
		//iDebugRenderer is stored and used, but never deleted.
		cWrapperDebugRenderer(iDebugRenderer* debugRenderer);

		//Destructor
		virtual ~cWrapperDebugRenderer();

		virtual void PhysDrawSphere(const glm::mat4& transform, float radius);
		virtual void PhysDrawPlane(const glm::vec3& normal, float constant);
	};
}