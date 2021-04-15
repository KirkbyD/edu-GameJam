#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "cCollisionBody.hpp"
#include "iShape.hpp"

namespace phys {
	// sRigidBodyDef
	// 
	// Contains all non-shape related information
	// necessary to create a cRigidBody instance.
	struct sRigidBodyDef {
		// Default Constructor
		// Sets all value's to 0
		sRigidBodyDef() {
			Mass = 0.f;
			Position = glm::vec3(0.f, 0.f, 0.f);
			Velocity = glm::vec3(0.f, 0.f, 0.f);
			MaxVelocity = glm::vec3(0.f);
		}

		// Constructor
		// Takes in all values by reference.
		sRigidBodyDef(const float& Mas, const glm::vec3& Pos, const glm::vec3& Vel) {
			Mass = Mas;
			Position = Pos;
			Velocity = Vel;
			MaxVelocity = glm::vec3(0.f);
		}

		// Constructor
		// Takes in all values by reference.
		// Also allows a cap on velocity.
		sRigidBodyDef(const float& Mas, const glm::vec3& Pos, const glm::vec3& Vel, glm::vec3& MaxVel) {
			Mass = Mas;
			Position = Pos;
			Velocity = Vel;
			MaxVelocity = MaxVel;
		}

		// Mass
		// Must be set to a non-negative value.
		// 0.f will result in a static rigid body.
		// A positive value will result in a dynamic rigid body.
		float Mass;
		// Position
		// The initial position of the rigid body.
		glm::vec3 Position;
		// Velocity
		// The initial linear velocity of the rigid body.
		glm::vec3 Velocity;
		// Maximum Velocity
		// Expected to be non-negative.
		// If 0.f, velocity is uncapped
		glm::vec3 MaxVelocity;
	};

	// cRigidBody
	//
	// Contains all values required for a rigid body simulation.
	// 
	// Shapes may be shared between rigid bodies.
	// Does not own the iShape* used to create it.
	// Will not delete the iShape* it contains when it is deleted.
	class cRigidBody : public cCollisionBody {
		// cWorld will be operating on cRigidBody values quite a bit
		// We will trust it to do everything correctly.
		friend class cWorld;
	private:
		// My shape, expected to be valid.
		// cRigidBody will not delete mShape in its destructor.
		iShape* mShape;
		glm::vec3 mPosition;
		glm::vec3 mPreviousPosition;
		glm::vec3 mVelocity;
		glm::vec3 mAcceleration;
		// Mass
		// Expected to be non-negative.
		// A value of 0 infers a static rigid body.
		float mMass;
		// Inverse mass
		// Stored for convenience.
		// If mMass is 0, mInvMass will also be 0.
		float mInvMass;
		// Maximum Velocity
		// Expected to be non-negative.
		// If 0.f, velocity is uncapped
		glm::vec3 mMaxVelocity;

		// Constructors not to be used.
		cRigidBody() = delete;
		cRigidBody(const cRigidBody& other) = delete;
		cRigidBody& operator=(const cRigidBody& other) = delete;

	public:
		// All internal values are initialized according to the sRigidBodyDef
		// A valid iShape* is expected.
		cRigidBody(const sRigidBodyDef& def, iShape* shape);
	
		// Destructor
		// Will not delete the contained iShape.
		virtual ~cRigidBody();

		// GetVelocity
		// Retrieve the objects current velocity
		void GetVelocity(glm::vec3& velocityOut);

		// GetTransform
		// Retrieve a rendering-friendly form of position + rotation
		void GetTransform(glm::mat4& transformOut);
	
		// GetTransform
		// Retrieve a rendering-friendly form of position + rotation
		void GetMass(float& mass);
	
		// ApplyForce
		// Apply an external force to this rigid body.
		// The force is applied center-mass, affecting the acceleration.
		void ApplyForce(const glm::vec3& force);
		//void ApplyForceAtPoint(const glm::vec3& force, const glm::vec3& relativePoint);

		// ApplyImpulse
		// Apply an impulse to the center-mass of this rigid body.
		void ApplyImpulse(const glm::vec3& impulse);

		// GetShape
		// Get this rigid body's shape.
		// Each rigid body must be created with a shape.
		inline iShape* GetShape() { return mShape; }

		// GetShapeType
		// Get the type of shape of this rigid body's shape.
		// Equivalent to GetShape()->GetShapeType()
		inline const eShapeType& GetBodyType() { return mShape->GetBodyType(); }

		virtual void ClearAccelerations() override;

		// IsStatic
		// A static rigid body was created with a mass of 0.f.
		// A static rigid body does react to other rigid bodies in the world.
		inline bool IsStatic() { return mMass == 0.f; }
	};
}