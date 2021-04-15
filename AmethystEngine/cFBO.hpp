#pragma once
#include <string>
#include "GLCommon.hpp"

class cFBO
{
public:
	cFBO() :
		ID(0),
		colourTexture_0_ID(0),
		depthTexture_ID(0),
		width(-1), height(-1),
		isCubeMap(false){};

	GLuint ID;						// = 0;
	GLuint colourTexture_0_ID;		// = 0;
	GLuint depthTexture_ID;			// = 0;

	bool isCubeMap;

	GLint width;		// = 512 the WIDTH of the framebuffer, in pixels;
	GLint height;

	// Inits the FBP
	bool init(int width, int height, std::string& error);
	bool initCubeMap(int cubeMapSize, std::string& error);

	bool shutdown(void);

	// Calls shutdown(), then init()
	bool reset(int width, int height, std::string& error);
	bool resetCubeMap(int cubeMapSize, std::string& error);

	void clearBuffers(bool bClearColour = true, bool bClearDepth = true);

	void clearColourBuffer(int bufferindex);
	void clearAllColourBuffers(void);
	void clearDepthBuffer(void);
	void clearStencilBuffer(int clearColour, int mask = 0xFF);

	int getMaxColourAttachments(void);
	int getMaxDrawBuffers(void);
};