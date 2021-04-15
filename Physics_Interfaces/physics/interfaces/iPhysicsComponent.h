#pragma once
#include <glm/mat4x4.hpp>
#include "eComponentType.h"
#include <string>

class cComplexObject;


namespace nPhysics {
	class iPhysicsComponent	{
	public:
		virtual ~iPhysicsComponent() {}
		//Returns what sort of shape this component is.
		inline const eComponentType& GetComponentType() { return mComponentType; }

		//Returns a pointer to the component's parent
		inline const unsigned& GetFriendlyID() { return mFriendlyID; }
		//Sets the Component's Parent
		inline const void SeFriendlyID(unsigned id) { mFriendlyID = id; }

		//Returns a pointer to the component's parent
		inline const unsigned& GetParentID() { return mParentID; }
		//Sets the Component's Parent
		inline const void SetParentID(unsigned id) { mParentID = id; }

		//Returns a pointer to the component's parent
		inline cComplexObject* GetParentPointer() { return mParentPointer; }
		//Sets the Component's Parent
		inline const void SetParentPointer(cComplexObject* parent) { mParentPointer = parent; }
		
		inline std::string GetBoneName() { return mBoneName; }
		inline const void SetBoneName(std::string name) { mBoneName = name; }

		// Get the current physics transformation for rendering purposes.
		virtual void GetTransform(glm::mat4& transformOut) = 0;
		virtual void SetTransform(glm::mat4 TransformIn) = 0;

		// Get the current velocity of this component
		// returns false if component is static.
		virtual bool GetVelocity(glm::vec3& velocityOut) = 0;

		virtual void SetPosition(glm::vec3 positionIn) = 0;

	protected:
		iPhysicsComponent(eComponentType componentType, const unsigned& id)
			: mComponentType(componentType)
			, mParentID(NULL)
			, mFriendlyID(id)
			, mParentPointer(NULL) { }
		
	private:
		// What type of shape this component is
		eComponentType mComponentType;

		// ID values for engine use
		unsigned mFriendlyID;
		unsigned mParentID;

		// Proof of concept
		// TODO - GET THIS OUT OF HERE
		cComplexObject* mParentPointer;

		std::string mBoneName; //for animation stuff

		// Delete constructors so that they cannot be used
		iPhysicsComponent() = delete;
		iPhysicsComponent(const iPhysicsComponent& other) = delete;
		iPhysicsComponent& operator=(const iPhysicsComponent& other) = delete;
	};
}