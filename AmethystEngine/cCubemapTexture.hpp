#pragma once

/*CLEAN*/

#include <string>
#include <vector>

class cCubemapTexture {
public:

	cCubemapTexture(std::string textureName,
					std::string posX_fileToLoadFullPath, std::string negX_fileToLoadFullPath,
					std::string posY_fileToLoadFullPath, std::string negY_fileToLoadFullPath,
					std::string posZ_fileToLoadFullPath, std::string negZ_fileToLoadFullPath,
					bool bIsSeamless = true);
	~cCubemapTexture();

	std::string textureName;
	std::vector<unsigned char*> imageBuffers;
	int width, height, nrChannels;
	bool bIsSeamless;
};