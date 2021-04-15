#pragma once

#include "eShapetype.hpp"

namespace phys {
	// iShape
	// A base interface for all specific shapes.
	// Contains an eShapeType to identify the specific shape type for casting.
	class iShape {
	private:
		// The shape type identifier.
		eShapeType mShapeType;

		// Constructors not to be used.
		iShape() : mShapeType(eShapeType::sphere) {}
		iShape(const iShape& other) : mShapeType(other.mShapeType) {}
		iShape& operator=(const iShape& other) { return *this;  }

	protected:
		// Expected to be called only by the constructors of subclasses.
		// The calling subclass is expected to pass the correct eShapeType.
		iShape(eShapeType shapeType) : mShapeType (shapeType) {}

	public:
		virtual ~iShape() {}

		// Returns the eShapeType indicating the specific shape.
		inline const eShapeType& GetBodyType() { return mShapeType; }
	};
}