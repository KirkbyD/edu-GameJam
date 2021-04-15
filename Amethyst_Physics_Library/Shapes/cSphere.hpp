#pragma once
#include "../iShape.hpp"
#include "glm/vec3.hpp"

namespace phys{
	class cSphere : public iShape {
	public:
		virtual ~cSphere() {}

		// A sphere is defined only by a radius.
		cSphere(float radius)
			: iShape(eShapeType::sphere)
			, mRadius(radius) {	}

		inline const float& GetRadius() { return mRadius; }

	private:
		float mRadius;

		// Constructors not to be used.
		cSphere() = delete;
		cSphere(const cSphere& other) = delete;
		cSphere& operator=(const cSphere& other) = delete;
	};
}