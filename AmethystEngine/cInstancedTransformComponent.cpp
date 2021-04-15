#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cInstancedTransformComponent.hpp"

cInstancedTransformComponent::cInstancedTransformComponent()
{
	// Create buffer
	glGenBuffers(1, &(this->worldMatrixBufferOnGPU));
}

void cInstancedTransformComponent::addTransformInstance(cTransformComponent* instance)
{
	this->transformComponents.push_back(instance);
}

void cInstancedTransformComponent::moveInstance(size_t idx, glm::vec3 position) {
	transformComponents[idx]->setPosition(position);
}

const std::vector<cTransformComponent*> cInstancedTransformComponent::getTransforms()
{
	return this->transformComponents;
}

int cInstancedTransformComponent::getInstancesCount()
{
	return this->transformComponents.size();
}

void cInstancedTransformComponent::bindBuffer()
{
	// Bind buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->worldMatrixBufferOnGPU);
}

void cInstancedTransformComponent::unbindBuffer()
{
	// Unbind buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void cInstancedTransformComponent::initializeOnCPU()
{
	// Creates world matrix on CPU
	this->worldMatrixBufferOnCPU = new glm::mat4[this->getInstancesCount()];
}

void cInstancedTransformComponent::updateOnCPU()
{
	// Updates world matrix on CPU
	for (size_t i = 0; i < this->getInstancesCount(); i++)
	{
		this->worldMatrixBufferOnCPU[i] = this->transformComponents[i]->getWorldMatrix();
	}
}

void cInstancedTransformComponent::initializeOnGPU()
{
	// If we're not using threaded system - we're doing it here
	this->initializeOnCPU();

	// Bind buffer
	this->bindBuffer();

	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, this->getInstancesCount() * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);

	// Unbind buffer
	this->unbindBuffer();
}

void cInstancedTransformComponent::updateOnGPU()
{
	// If we're not using threaded system - we're doing it here
	this->updateOnCPU();

	// Bind buffer
	this->bindBuffer();

	// Empty buffer
	glBufferData(GL_ARRAY_BUFFER, this->getInstancesCount() * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);

	// Fill buffer with existing World Matrices
	glBufferSubData(GL_ARRAY_BUFFER, 0, this->getInstancesCount() * sizeof(glm::mat4), this->worldMatrixBufferOnCPU);

	// Unbind buffer
	this->unbindBuffer();
}