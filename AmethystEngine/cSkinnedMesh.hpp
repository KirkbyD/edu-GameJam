#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include "cMesh.hpp"

class cSkinnedMesh {
public:
	std::vector<cMesh> skinnedMeshes;
};
