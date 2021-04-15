#pragma once

/*CLEAN*/

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <glad/glad.h>
#include "cTexture.hpp"
#include "cCubemapTexture.hpp"

class cTextureManager {
public:
	// Singleton
	static cTextureManager* GetTextureManager();

	// Destuctor
	~cTextureManager();

	// Changes the base path to textures (Can be used to switch between different texture file extension)
	void SetBasePath(std::string path);

	// Creates texture on CPU and pushes it to CPU textures map
	bool CreateTexture(std::string textureName, std::string fileName, bool bGenerateMIPMap = true);

	// Creates cubemap texture on CPU and pushes it to CPU textures map
	bool CreateCubemapTexture(std::string textureName,
							std::string posX_fileName, std::string negX_fileName,
							std::string posY_fileName, std::string negY_fileName,
							std::string posZ_fileName, std::string negZ_fileName,
							bool bIsSeamless = true);

	// Loads created texture to GPU and pushes it to GPU textures map
	void LoadTexture(cTexture* pTexture);

	// Loads created cubemap texture to GPU and pushes it to GPU textures map
	void LoadCubemapTexture(cCubemapTexture* pTexture);

	// Loads all created textures to GPU, pushes them to GPU textures map and clears CPU textures maps
	void LoadAllCreatedTexturesToGPU();

	void LoadDefaultTexturesToGPU();

	// returns 0 on error
	GLuint FindTextureByName(std::string textureName);

	// Picks a random texture from the textures loaded
	std::string PickRandomTexture();

private:
	// Singleton
	static cTextureManager* stonTextureMngr;
	cTextureManager();

	// Base Path and Texture Maps
	std::string basePath;
	std::map<std::string, cTexture*> mTexturesOnCPU;
	std::map<std::string, cCubemapTexture*> mCubemapTexturesOnCPU;
	std::map<std::string, GLuint> mAllTexturesOnGPU;

	// Mutex
	std::mutex m_mutex;
};