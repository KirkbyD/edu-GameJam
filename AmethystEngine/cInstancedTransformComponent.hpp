#pragma once

#include "GLCommon.hpp"
#include "cTransformComponent.hpp"
#include "vector"

class cInstancedTransformComponent {
private:
	std::vector<cTransformComponent*> transformComponents;
	glm::mat4* worldMatrixBufferOnCPU;
	GLuint worldMatrixBufferOnGPU;

	void initializeOnCPU();
	void updateOnCPU();

public:
	cInstancedTransformComponent();

	void addTransformInstance(cTransformComponent* instance);
	void moveInstance(size_t idx, glm::vec3 position);
	const std::vector<cTransformComponent*> getTransforms();
	int getInstancesCount();

	void bindBuffer();
	void unbindBuffer();

	void initializeOnGPU();
	void updateOnGPU();
};