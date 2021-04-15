#include "cTexture.hpp"
#include "stb/stb_image.h"

cTexture::cTexture(std::string textureName, std::string fileToLoadFullPath, bool bGenerateMIPMap)
{
	// Copy the needed data
	this->textureName = textureName;
	this->bGenerateMIPMap = bGenerateMIPMap;

	// y-axis shenanigans
	stbi_set_flip_vertically_on_load(true);

	// Load image into the image buffer
	this->imageBuffer = stbi_load(fileToLoadFullPath.c_str(), &(this->width), &(this->height), &(this->nrChannels), 4);
}

cTexture::~cTexture()
{
	stbi_image_free(imageBuffer);
}