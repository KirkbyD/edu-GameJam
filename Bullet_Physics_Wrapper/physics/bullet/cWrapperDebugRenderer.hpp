#pragma once
#include "LinearMath/btIDebugDraw.h"
#include <physics/interfaces/iDebugRenderer.h>

namespace nPhysics {
	class cWrapperDebugRenderer : public btIDebugDraw {
	private:
		iDebugRenderer* mDebugRenderer;
        int m_debugMode;

	public:
		//Constructor
		//iDebugRenderer is stored and used, but never deleted.
		cWrapperDebugRenderer(iDebugRenderer* debugRenderer);

		//Destructor
		virtual ~cWrapperDebugRenderer();

		//virtual void PhysDrawSphere(const glm::mat4& transform, float radius);
		//virtual void PhysDrawPlane(const glm::vec3& normal, float constant);

        virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

        virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);

        virtual void reportErrorWarning(const char* warningString);

        virtual void draw3dText(const btVector3& location, const char* textString);

        virtual void setDebugMode(int m_debugMode);

        virtual int getDebugMode() const;

        // void doDrawing();
		// void cleanDrawing();
	};
}