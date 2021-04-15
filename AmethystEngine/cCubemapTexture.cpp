#include "cCubemapTexture.hpp"
#include "stb/stb_image.h"

cCubemapTexture::cCubemapTexture(std::string textureName,
								std::string posX_fileToLoadFullPath, std::string negX_fileToLoadFullPath,
								std::string posY_fileToLoadFullPath, std::string negY_fileToLoadFullPath,
								std::string posZ_fileToLoadFullPath, std::string negZ_fileToLoadFullPath,
								bool bIsSeamless)
{
	// Copy the needed data
	this->textureName = textureName;
	this->bIsSeamless = bIsSeamless;

	// y-axis shenanigans
	stbi_set_flip_vertically_on_load(true);

	// Load images into the image buffers
	this->imageBuffers.push_back(stbi_load(posX_fileToLoadFullPath.c_str(), &(this->width), &(this->height), &(this->nrChannels), 4));
	this->imageBuffers.push_back(stbi_load(negX_fileToLoadFullPath.c_str(), &(this->width), &(this->height), &(this->nrChannels), 4));
	this->imageBuffers.push_back(stbi_load(posY_fileToLoadFullPath.c_str(), &(this->width), &(this->height), &(this->nrChannels), 4));
	this->imageBuffers.push_back(stbi_load(negY_fileToLoadFullPath.c_str(), &(this->width), &(this->height), &(this->nrChannels), 4));
	this->imageBuffers.push_back(stbi_load(posZ_fileToLoadFullPath.c_str(), &(this->width), &(this->height), &(this->nrChannels), 4));
	this->imageBuffers.push_back(stbi_load(negZ_fileToLoadFullPath.c_str(), &(this->width), &(this->height), &(this->nrChannels), 4));
}

cCubemapTexture::~cCubemapTexture()
{
	for (size_t i = 0; i < this->imageBuffers.size(); i++)
	{
		stbi_image_free(imageBuffers[i]);
	}

	imageBuffers.clear();
}