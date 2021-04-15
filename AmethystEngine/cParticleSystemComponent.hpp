#pragma once

#include "GLCommon.hpp"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>
#include <algorithm>

#include "cHelpingFunctions.hpp"

class cParticle
{
public:
	cParticle() : location(glm::vec3(0.0f)), scale(1.0f), velocity(glm::vec3(0.0f)), lifeTime(0.0f), cameraDistance(-1.0f) {}

	glm::vec3 location;
	// glm::quat orientation;
	float scale;

	glm::vec3 velocity;
	float lifeTime;

	float cameraDistance;

	bool operator<(const cParticle& that) const {
		// Sort in reverse order (far particles drawn first)
		return this->cameraDistance > that.cameraDistance;
	}
};

class cParticleSystemComponent
{
private:
	std::vector<cParticle*> m_vec_pParticles;
	glm::mat4* worldMatrixBufferOnCPU;
	GLuint worldMatrixBufferOnGPU;

	// Static consts can be set here, static can't
	static const int MaxParticles = 1000;
	int ParticlesCount;

	float minSize;
	float maxSize;
	glm::vec3 minVelocity;
	glm::vec3 maxVelocity;
	glm::vec3 minDeltaPosition;
	glm::vec3 maxDeltaPosition;
	float minLifeSeconds;
	float maxLifeSeconds;
	int minParticlesPerFrame;
	int maxParticlesPerFrame;

	// Creates a single particle in place of the "dead" one
	void createNewParticle();

	void SortParticles();

	void initializeOnCPU(float minSize, float maxSize,
		glm::vec3 minVelocity, glm::vec3 maxVelocity,
		glm::vec3 minDeltaPosition, glm::vec3 maxDeltaPosition,
		float minLifeSeconds, float maxLifeSeconds,
		int minParticlesPerFrame, int maxParticlesPerFrame);

	void updateOnCPU(float deltaTime, glm::vec3 CameraPosition);

public:
	cParticleSystemComponent();
	cParticleSystemComponent(glm::vec3 location, glm::vec3 acceleration);
	~cParticleSystemComponent();

	glm::vec3 location;
	glm::vec3 acceleration;

	int getMaxParticles() { return MaxParticles; }
	int getParticlesCount() { return ParticlesCount; }
	glm::mat4* getWorldMatrixBuffer() { return worldMatrixBufferOnCPU; }

	void bindBuffer();
	void unbindBuffer();

	void initializeOnGPU(float minSize, float maxSize,
		glm::vec3 minVelocity, glm::vec3 maxVelocity,
		glm::vec3 minDeltaPosition, glm::vec3 maxDeltaPosition,
		float minLifeSeconds, float maxLifeSeconds,
		int minParticlesPerFrame, int maxParticlesPerFrame);

	void updateOnGPU(float deltaTime, glm::vec3 CameraPosition);
};