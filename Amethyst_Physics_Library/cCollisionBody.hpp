#pragma once
#include "eBodyType.hpp"

namespace phys {
	//good place to add common items like bounding box and extremes
	// TODO - TURN ME INTO A CLASS
	class cCollisionBody {
	private:
		// The shape type identifier.
		eBodyType mBodyType;

		// Constructors not to be used.
		cCollisionBody() = delete;
		cCollisionBody(const cCollisionBody& other) = delete;
		cCollisionBody& operator=(const cCollisionBody& other) = delete;

	protected:
		// Expected to be called only by the constructors of subclasses.
		// The calling subclass is expected to pass the correct eShapeType.
		cCollisionBody(eBodyType bodyType)
			: mBodyType(bodyType)
			, mUserPointer(0)
		{ }

	public:
		//TODO FOR COLLISION LISTENER
		void* mUserPointer;
		inline void* GetUserPointer() { return mUserPointer; }
		inline void SetUserPointer(void* userPointer) { mUserPointer = userPointer; }

		virtual ~cCollisionBody() {}

		// used by cWorld post integration
		virtual void ClearAccelerations() = 0;

		// Returns the eShapeType indicating the specific shape.
		inline const eBodyType& GetBodyType() { return mBodyType; }
	};
};