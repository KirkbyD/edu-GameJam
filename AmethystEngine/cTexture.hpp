#pragma once

/*CLEAN*/

#include <string>

class cTexture {
public:

	cTexture(std::string textureName, std::string fileName, bool bGenerateMIPMap = true);
	~cTexture();

	std::string textureName;
	unsigned char* imageBuffer;
	int width, height, nrChannels;
	bool bGenerateMIPMap;
};