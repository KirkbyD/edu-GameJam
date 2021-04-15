#include "cWrapperDebugRenderer.hpp"
#include "nConvert.hpp"

//DBG_NoDebug = 0,
//DBG_DrawWireframe = 1,
//DBG_DrawAabb = 2,
//DBG_DrawFeaturesText = 4,
//DBG_DrawContactPoints = 8,
//DBG_NoDeactivation = 16,
//DBG_NoHelpText = 32,
//DBG_DrawText = 64,
//DBG_ProfileTimings = 128,
//DBG_EnableSatComparison = 256,
//DBG_DisableBulletLCP = 512,
//DBG_EnableCCD = 1024,
//DBG_DrawConstraints = (1 << 11),
//DBG_DrawConstraintLimits = (1 << 12),
//DBG_FastWireframe = (1 << 13),
//DBG_DrawNormals = (1 << 14),
//DBG_DrawFrames = (1 << 15),
//DBG_MAX_DEBUG_DRAW_MODE

namespace nPhysics {
	cWrapperDebugRenderer::cWrapperDebugRenderer(iDebugRenderer* debugRenderer)
		: mDebugRenderer(debugRenderer)
		, m_debugMode(btIDebugDraw::DBG_DrawWireframe | DBG_DrawAabb | DBG_DrawConstraints | DBG_DrawConstraintLimits | DBG_FastWireframe)
	{ }

	cWrapperDebugRenderer::~cWrapperDebugRenderer() {
		mDebugRenderer = 0;
	}

	void cWrapperDebugRenderer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
		sDebugLine line;
		line.start = nConvert::ToSimple(from);
		line.end = nConvert::ToSimple(to);
		line.colour = nConvert::ToSimple(color);

		mDebugRenderer->DrawLine(line);
	}

	void cWrapperDebugRenderer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
	{
	}

	void cWrapperDebugRenderer::reportErrorWarning(const char* warningString)
	{
	}

	void cWrapperDebugRenderer::draw3dText(const btVector3& location, const char* textString)
	{
	}

	void cWrapperDebugRenderer::setDebugMode(int m_debugMode)
	{
	}

	int cWrapperDebugRenderer::getDebugMode() const
	{
		return m_debugMode;
	}
}