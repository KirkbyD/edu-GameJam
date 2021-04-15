#pragma once
#include "../iShape.hpp"
#include "glm/vec3.hpp"

namespace phys {
	class cPlane : public iShape {
	public:
		virtual ~cPlane() {}
	
		// A plane must defined by a normal vector and a constant.
		// The normal vector is expected to already be normalized.
		cPlane(const glm::vec3 normal, float constant)
			: iShape(eShapeType::plane)
			, mNormal(normal)
			, mConstant(constant) { }

		inline const glm::vec3& GetNormal() { return mNormal; }
		inline float GetConstant() { return mConstant; }

	private:
		// Normal vector.
		// The <A,B,C> in  Ax + By + Cz = D
		glm::vec3 mNormal;

		// Constant value of the plane.
		// The "D" in  Ax + By + Cz = D
		float mConstant;

		// Constructors not to be used.
		cPlane() = delete;
		cPlane(const cPlane& other) = delete;
		cPlane& operator=(const cPlane& other) = delete;
	};
}