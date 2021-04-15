#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cTextureManager.hpp"

#pragma region SINGLETON
cTextureManager* cTextureManager::stonTextureMngr = 0;
cTextureManager* cTextureManager::GetTextureManager() {
	if (stonTextureMngr == 0)
	{
		cTextureManager::stonTextureMngr = new cTextureManager();
	}
	return stonTextureMngr;
}
cTextureManager::cTextureManager() { printf("Texture Manager Created\n"); }
#pragma endregion

cTextureManager::~cTextureManager()
{
	if (!mTexturesOnCPU.empty())
	{
		for (auto it = mTexturesOnCPU.cbegin(); it != mTexturesOnCPU.cend() /* not hoisted */; /* no increment */)
		{
			delete it->second;
			it = mTexturesOnCPU.erase(it);
		}
	}

	if (!mCubemapTexturesOnCPU.empty())
	{
		for (auto it = mCubemapTexturesOnCPU.cbegin(); it != mCubemapTexturesOnCPU.cend() /* not hoisted */; /* no increment */)
		{
			delete it->second;
			it = mCubemapTexturesOnCPU.erase(it);
		}
	}

	mTexturesOnCPU.clear();
	mCubemapTexturesOnCPU.clear();
	mAllTexturesOnGPU.clear();
}

void cTextureManager::SetBasePath(std::string path)
{
	this->basePath = path;
	return;
}

bool cTextureManager::CreateTexture(std::string textureName, std::string fileName, bool bGenerateMIPMap)
{
	// Full path
	std::string fileToLoadFullPath = this->basePath + "/" + fileName;

	// Create texture
	cTexture* texture = new cTexture(textureName, fileToLoadFullPath, bGenerateMIPMap);

	// Check if anything was written to the buffer
	if (texture->imageBuffer == NULL)
	{
		printf("Texture failed to load at path: '%s'\n", fileToLoadFullPath.c_str());
		
		return false;
	}

	// Add texture to the map
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		mTexturesOnCPU[textureName] = texture;
	}

	return true;
}

bool cTextureManager::CreateCubemapTexture(std::string textureName,
	std::string posX_fileName, std::string negX_fileName,
	std::string posY_fileName, std::string negY_fileName,
	std::string posZ_fileName, std::string negZ_fileName,
	bool bIsSeamless)
{
	// Full path
	std::vector<std::string> facesPaths;
	facesPaths.push_back(this->basePath + "/" + posX_fileName);
	facesPaths.push_back(this->basePath + "/" + negX_fileName);
	facesPaths.push_back(this->basePath + "/" + posY_fileName);
	facesPaths.push_back(this->basePath + "/" + negY_fileName);
	facesPaths.push_back(this->basePath + "/" + posZ_fileName);
	facesPaths.push_back(this->basePath + "/" + negZ_fileName);

	// Create cubemap texture
	cCubemapTexture* texture = new cCubemapTexture(textureName,
		facesPaths[0], facesPaths[1],
		facesPaths[2], facesPaths[3],
		facesPaths[4], facesPaths[5],
		bIsSeamless);

	for (int i = 0; i < facesPaths.size(); i++)
	{
		// Check if anything was written to the buffer
		if (texture->imageBuffers[i] == NULL)
		{
			printf("Cubemap texture failed to load at path: '%s'\n", facesPaths[i].c_str());
			return false;
		}
	}

	// Add Texture to the map
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		mCubemapTexturesOnCPU[textureName] = texture;
	}

	return true;
}

void cTextureManager::LoadTexture(cTexture* pTexture)
{
	// Create empty texture on GPU
	GLuint tex_2d;
	glGenTextures(1, &tex_2d);
	glBindTexture(GL_TEXTURE_2D, tex_2d);

	// Allocate space for image buffer contents and assign it to the texture
	glTexImage2D(GL_TEXTURE_2D,			// target (2D, 3D, etc.)		// OpenGL 2.0 and up
				0,						// MIP map level 
				GL_RGBA,				// internal format
				pTexture->width,		// width (pixels)	
				pTexture->height,		// height (pixels)	
				0,						// border (0 or 1)
				GL_RGBA,				// format of pixel data			// Change if other types of images used
				GL_UNSIGNED_BYTE,		// type of pixel data
				pTexture->imageBuffer);	// pointer to data in memory

	// We can use glTexSubImage2D in case if we want to map a portion of texture (but it's easier to do it in shader)

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Generate mipmaps for texture (usually we almost always want that)
	if (pTexture->bGenerateMIPMap)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	// Bind everything back to 0 after we're done
	glBindTexture(GL_TEXTURE_2D, 0);

	// Add texture to map
	this->mAllTexturesOnGPU[pTexture->textureName] = tex_2d;
}

void cTextureManager::LoadCubemapTexture(cCubemapTexture* pTexture)
{
	// Create empty cubemap texture on GPU
	GLuint tex_cube;
	glGenTextures(1, &tex_cube);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex_cube);

	// Make cubemap seamless (usually we almost always want that)
	if (pTexture->bIsSeamless)
	{
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	// Allocate space for each image buffer of cubemap texture on GPU and assign it to proper side
	for (int i = 0; i < pTexture->imageBuffers.size(); i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, pTexture->width, pTexture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pTexture->imageBuffers[i]);
	}

	// We can generate mipmaps for cubemap texture, but since resolution of cubemaps rarely changes (especially skyboxes), it'll just waste memory (around 8mb, but still)

	// Set texture parameters
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Bind everything back to 0
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	// Add texture to map
	this->mAllTexturesOnGPU[pTexture->textureName] = tex_cube;
}

void cTextureManager::LoadAllCreatedTexturesToGPU()
{
	std::lock_guard<std::mutex> locker(m_mutex);

	if (!mTexturesOnCPU.empty())
	{
		for (auto it = mTexturesOnCPU.cbegin(); it != mTexturesOnCPU.cend() /* not hoisted */; /* no increment */)
		{
			LoadTexture(it->second);
			delete it->second;
			it = mTexturesOnCPU.erase(it);
		}
	}

	if (!mCubemapTexturesOnCPU.empty())
	{
		for (auto it = mCubemapTexturesOnCPU.cbegin(); it != mCubemapTexturesOnCPU.cend() /* not hoisted */; /* no increment */)
		{
			LoadCubemapTexture(it->second);
			delete it->second;
			it = mCubemapTexturesOnCPU.erase(it);
		}
	}
}

void cTextureManager::LoadDefaultTexturesToGPU()
{
	std::lock_guard<std::mutex> locker(m_mutex);

	if (!mTexturesOnCPU.empty())
	{
		for (auto it = mTexturesOnCPU.cbegin(); it != mTexturesOnCPU.cend() /* not hoisted */; /* no increment */)
		{
			if (it->first == "Default")
			{
				LoadTexture(it->second);
				delete it->second;
				it = mTexturesOnCPU.erase(it);
			}
		}
	}

	if (!mCubemapTexturesOnCPU.empty())
	{
		for (auto it = mCubemapTexturesOnCPU.cbegin(); it != mCubemapTexturesOnCPU.cend() /* not hoisted */; /* no increment */)
		{
			if (it->first == "DefaultSkybox")
			{
				LoadCubemapTexture(it->second);
				delete it->second;
				it = mCubemapTexturesOnCPU.erase(it);
			}
		}
	}
}

GLuint cTextureManager::FindTextureByName(std::string textureName)
{
	if (this->mAllTexturesOnGPU.find(textureName) != this->mAllTexturesOnGPU.end())
	{
		return this->mAllTexturesOnGPU[textureName];
	}
	else
	{
		return 0;
	}
}

std::string cTextureManager::PickRandomTexture()
{
	// There are no textures loaded, yet
	if (this->mAllTexturesOnGPU.empty())
	{
		return "";
	}

	int textureIndexRand = rand() % (this->mAllTexturesOnGPU.size() + 1);
	if (textureIndexRand >= this->mAllTexturesOnGPU.size())
	{
		textureIndexRand = 0;
	}

	std::map< std::string, GLuint>::iterator itTex = this->mAllTexturesOnGPU.begin();
	for (unsigned int count = 0; count != textureIndexRand; count++, itTex++) {}

	return itTex->first;
}