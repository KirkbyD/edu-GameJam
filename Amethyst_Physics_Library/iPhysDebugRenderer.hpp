#pragma once

namespace phys {
	class iPhysDebugRenderer {
	public:
		virtual void PhysDrawSphere(glm::mat4 transform, float radius) = 0;
		virtual void PhysDrawPlane(glm::vec3 normal, float constant) = 0;
	};
}