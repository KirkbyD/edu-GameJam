#pragma once

namespace nPhysics {
	// Library's existing shape types.
	// 'Unkown' shapes will be disregarded
	enum class eComponentType {
		unknown,
		ball,
		plane,
		cloth,
		box,
		capsule,
		triangleMesh,
		characterController
	};
}