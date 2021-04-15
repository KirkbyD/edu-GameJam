#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cParticleSystemComponent.hpp"

cParticleSystemComponent::cParticleSystemComponent()
{
	// Create buffer
	glGenBuffers(1, &(this->worldMatrixBufferOnGPU));

	this->location = glm::vec3(0.0f, 0.0f, 0.0f);
	this->acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
	return;
}

cParticleSystemComponent::cParticleSystemComponent(glm::vec3 location, glm::vec3 acceleration)
{
	// Create buffer
	glGenBuffers(1, &(this->worldMatrixBufferOnGPU));

	this->location = location;
	this->acceleration = acceleration;
	return;
}

cParticleSystemComponent::~cParticleSystemComponent() {
	for (auto a : this->m_vec_pParticles) {
		a->~cParticle();
		delete a;
	}

	this->m_vec_pParticles.clear();

	delete this->worldMatrixBufferOnCPU;
}

void cParticleSystemComponent::initializeOnCPU(float minSize, float maxSize,
	glm::vec3 minVelocity, glm::vec3 maxVelocity,
	glm::vec3 minDeltaPosition, glm::vec3 maxDeltaPosition,
	float minLifeSeconds, float maxLifeSeconds,
	int minParticlesPerFrame, int maxParticlesPerFrame)
{
	// In case of reinitialization
	for (auto a : this->m_vec_pParticles) {
		a->~cParticle();
		delete a;
	}

	this->m_vec_pParticles.clear();

	delete this->worldMatrixBufferOnCPU;


	// Reserve space for particles
	this->m_vec_pParticles.reserve(cParticleSystemComponent::MaxParticles);

	// Create "dead" particles
	for (int count = 0; count != cParticleSystemComponent::MaxParticles; count++)
	{
		this->m_vec_pParticles.push_back(new cParticle());
	}

	// Initialize data
	this->minSize = minSize;
	this->maxSize = maxSize;
	this->minVelocity = minVelocity;
	this->maxVelocity = maxVelocity;
	this->minDeltaPosition = minDeltaPosition;
	this->maxDeltaPosition = maxDeltaPosition;
	this->minLifeSeconds = minLifeSeconds;
	this->maxLifeSeconds = maxLifeSeconds;
	this->minParticlesPerFrame = minParticlesPerFrame;
	this->maxParticlesPerFrame = maxParticlesPerFrame;

	// Initialize World Matrix data for instancing
	this->worldMatrixBufferOnCPU = new glm::mat4[this->getMaxParticles()];

	return;
}

void cParticleSystemComponent::updateOnCPU(float deltaTime, glm::vec3 CameraPosition)
{
	// Decrease the life of each particle by "deltaTime" and if it's still "alive" move it

	ParticlesCount = 0;

	for (std::vector<cParticle*>::iterator itPart = this->m_vec_pParticles.begin();
		itPart != this->m_vec_pParticles.end();  itPart++)
	{
		cParticle* pCurParticle = *itPart;

		pCurParticle->lifeTime -= deltaTime;

		if (pCurParticle->lifeTime > 0.0f)
		{
			// Update velocity from acceleration
			pCurParticle->velocity += (this->acceleration * deltaTime);

			// Update position from velocity
			pCurParticle->location += (pCurParticle->velocity * deltaTime);

			// Update distance to camera
			pCurParticle->cameraDistance = glm::length2(pCurParticle->location - CameraPosition);


			glm::mat4 matWorld = glm::mat4(1.0f);
			matWorld = glm::translate(matWorld, glm::vec3(pCurParticle->location.x, pCurParticle->location.y, pCurParticle->location.z));
			matWorld = glm::scale(matWorld, glm::vec3(pCurParticle->scale));
			//matWorld = glm::rotate(matWorld, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
			this->worldMatrixBufferOnCPU[this->ParticlesCount] = matWorld;
		}
		else
		{
			pCurParticle->cameraDistance = -1.0f;
		}
		this->ParticlesCount++;
	}

	SortParticles();

	// Create new particle out of "dead" ones
	int numParticleToCreate = randInRange<int>(this->minParticlesPerFrame, this->maxParticlesPerFrame);

	for (int count = 0; count != numParticleToCreate; count++)
	{
		this->createNewParticle();
	}

	return;
}

void cParticleSystemComponent::bindBuffer()
{
	// Bind buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->worldMatrixBufferOnGPU);
}

void cParticleSystemComponent::unbindBuffer()
{
	// Unbind buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void cParticleSystemComponent::initializeOnGPU(float minSize, float maxSize,
	glm::vec3 minVelocity, glm::vec3 maxVelocity,
	glm::vec3 minDeltaPosition, glm::vec3 maxDeltaPosition,
	float minLifeSeconds, float maxLifeSeconds,
	int minParticlesPerFrame, int maxParticlesPerFrame)
{
	// If we're not using threaded system - we're doing it here
	this->initializeOnCPU(minSize, maxSize, minVelocity, maxVelocity, minDeltaPosition, maxDeltaPosition, minLifeSeconds, maxLifeSeconds, minParticlesPerFrame, maxParticlesPerFrame);

	// Bind buffer
	this->bindBuffer();

	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, this->getMaxParticles() * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);

	// Unbind buffer
	this->unbindBuffer();
}

void cParticleSystemComponent::updateOnGPU(float deltaTime, glm::vec3 CameraPosition)
{
	// If we're not using threaded system - we're doing it here
	this->updateOnCPU(deltaTime, CameraPosition);

	// Bind buffer
	this->bindBuffer();

	// Empty buffer
	glBufferData(GL_ARRAY_BUFFER, this->getMaxParticles() * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);

	// Fill buffer with existing World Matrices
	glBufferSubData(GL_ARRAY_BUFFER, 0, this->getParticlesCount() * sizeof(glm::mat4), this->worldMatrixBufferOnCPU);

	// Unbind buffer
	this->unbindBuffer();
}

void cParticleSystemComponent::createNewParticle()
{
	// Find the 1st particle that is "dead"
	for (std::vector<cParticle*>::iterator itPart = this->m_vec_pParticles.begin();
		itPart != this->m_vec_pParticles.end();  itPart++)
	{
		cParticle* pCurParticle = *itPart;

		// If particle is "dead" - make it "alive" and set to some values
		if (pCurParticle->lifeTime <= 0.0f)
		{
			pCurParticle->lifeTime = randInRange<float>(this->minLifeSeconds, this->maxLifeSeconds);

			pCurParticle->velocity.x = randInRange<float>(this->minVelocity.x, this->maxVelocity.x);
			pCurParticle->velocity.y = randInRange<float>(this->minVelocity.y, this->maxVelocity.y);
			pCurParticle->velocity.z = randInRange<float>(this->minVelocity.z, this->maxVelocity.z);

			pCurParticle->scale = randInRange<float>(this->minSize, this->maxSize);

			pCurParticle->location.x = this->location.x + randInRange<float>(this->minDeltaPosition.x, this->maxDeltaPosition.x);
			pCurParticle->location.y = this->location.y + randInRange<float>(this->minDeltaPosition.y, this->maxDeltaPosition.y);
			pCurParticle->location.z = this->location.z + randInRange<float>(this->minDeltaPosition.z, this->maxDeltaPosition.z);
		}
	}
	return;
}

void cParticleSystemComponent::SortParticles()
{
	std::sort(m_vec_pParticles.begin(), m_vec_pParticles.end());
}