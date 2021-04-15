#include "cPhysicsDebugRenderer.hpp"

void cPhysicsDebugRenderer::DrawLine(nPhysics::sDebugLine line) {
	mSystemRenderer->addLine(line.start, line.end, line.colour);
}
