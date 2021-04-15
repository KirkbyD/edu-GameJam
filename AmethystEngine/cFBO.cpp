#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cFBO.hpp"


// Calls shutdown(), then init()
bool cFBO::reset(int width, int height, std::string& error)
{
	if (!this->shutdown())
	{
		error = "Could not shutdown";
		return false;
	}

	return this->init(width, height, error);
}

// Calls shutdown(), then init()
bool cFBO::resetCubeMap(int cubeMapSize, std::string& error)
{
	if (!this->shutdown())
	{
		error = "Could not shutdown";
		return false;
	}

	return this->initCubeMap(cubeMapSize, error);
}

bool cFBO::shutdown(void)
{
	glDeleteTextures(1, &(this->colourTexture_0_ID));
	glDeleteTextures(1, &(this->depthTexture_ID));

	glDeleteFramebuffers(1, &(this->ID));

	return true;
}


bool cFBO::init(int width, int height, std::string& error)
{
	this->width = width;
	this->height = height;
	this->isCubeMap = false;

	//glCreateFramebuffers(1, &( this->ID ) );	// GL 4.5		//g_FBO
	glGenFramebuffers(1, &(this->ID));		// GL 3.0
	glBindFramebuffer(GL_FRAMEBUFFER, this->ID);

	//************************************************************
	// Create the colour buffer (texture)
	glGenTextures(1, &(this->colourTexture_0_ID));		//g_FBO_colourTexture
	glBindTexture(GL_TEXTURE_2D, this->colourTexture_0_ID);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8,		// 8 bits per colour
	//glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F,	// 8 bits per colour
						this->width,				// g_FBO_SizeInPixes
						this->height);				// g_FBO_SizeInPixes

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//***************************************************************

	// Create the depth buffer (texture)
	glGenTextures(1, &(this->depthTexture_ID));			//g_FBO_depthTexture
	glBindTexture(GL_TEXTURE_2D, this->depthTexture_ID);

	//glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, ]

	// Note that, unless you specifically ask for it, the stencil buffer
	// is NOT present... i.e. GL_DEPTH_COMPONENT32F DOESN'T have stencil

	// These are:
	// - GL_DEPTH32F_STENCIL8, which is 32 bit float depth + 8 bit stencil
	// - GL_DEPTH24_STENCIL8,  which is 24 bit float depth + 8 bit stencil (more common?)
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8,	//GL_DEPTH32F_STENCIL8,
		this->width,		//g_FBO_SizeInPixes
		this->height);
	//	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT );
	//	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_STENCIL_COMPONENT );
	//	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, this->width, this->height, 0, GL_EXT_packe

	//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH24_STENCIL8, GL_TEXTURE_2D, this->depthTexture_ID, 0);

	// ***************************************************************

	glFramebufferTexture(GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,			// Colour goes to #0
		this->colourTexture_0_ID, 0);


	//	glFramebufferTexture(GL_FRAMEBUFFER,
	//						 GL_DEPTH_ATTACHMENT,
	//						 this->depthTexture_ID, 0);
	glFramebufferTexture(GL_FRAMEBUFFER,
		GL_DEPTH_STENCIL_ATTACHMENT,
		this->depthTexture_ID, 0);

	static const GLenum draw_bufers[] =
	{
		GL_COLOR_ATTACHMENT0
	};
	glDrawBuffers(1, draw_bufers);		// There are 4 outputs now

	// ***************************************************************




	// ADD ONE MORE THING...
	bool bFrameBufferIsGoodToGo = true;

	switch (glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
	case GL_FRAMEBUFFER_COMPLETE:
		bFrameBufferIsGoodToGo = true;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
		bFrameBufferIsGoodToGo = false;
		break;
		//	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
	case GL_FRAMEBUFFER_UNSUPPORTED:
	default:
		bFrameBufferIsGoodToGo = false;
		break;
	}//switch ( glCheckFramebufferStatus(GL_FRAMEBUFFER) )

	// Point back to default frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return bFrameBufferIsGoodToGo;
}

bool cFBO::initCubeMap(int cubeMapSize, std::string& error)
{
	this->width = cubeMapSize;
	this->height = cubeMapSize;
	this->isCubeMap = true;

	//************************************************************

	// Create framebuffer
	glGenFramebuffers(1, &(this->ID));
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, (this->ID));

	//************************************************************

	// Create empty cubemap
	glGenTextures(1, &(this->colourTexture_0_ID));
	//glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, (this->colourTexture_0_ID));
	//set texture parameters
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//for all 6 cubemap faces
	for (int face = 0; face < 6; face++) {
		//allocate a different texture for each face and assign to the cubemap texture target
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	}

	//***************************************************************

	//setup render buffer object (RBO)
	glGenRenderbuffers(1, &(this->depthTexture_ID));
	glBindRenderbuffer(GL_RENDERBUFFER, (this->depthTexture_ID));

	//set the renderbuffer storage to have the same dimensions as the cubemap texture
	//also set the renderbuffer as the depth attachment of the FBO
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, (this->ID));

	//set the dynamic cubemap texture as the colour attachment of FBO
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, (this->colourTexture_0_ID), 0);

	//***************************************************************

	// ADD ONE MORE THING...
	bool bFrameBufferIsGoodToGo = true;

	switch (glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
	case GL_FRAMEBUFFER_COMPLETE:
		bFrameBufferIsGoodToGo = true;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
		bFrameBufferIsGoodToGo = false;
		break;
		//	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
	case GL_FRAMEBUFFER_UNSUPPORTED:
	default:
		bFrameBufferIsGoodToGo = false;
		break;
	}

	//***************************************************************

	// Unbind FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	// Unbind renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	return bFrameBufferIsGoodToGo;
}

void cFBO::clearColourBuffer(int bufferindex)
{
	glViewport(0, 0, this->width, this->height);
	GLfloat	zero = 0.0f;
	glClearBufferfv(GL_COLOR, bufferindex, &zero);

	return;
}


void cFBO::clearBuffers(bool bClearColour, bool bClearDepth)
{
	glViewport(0, 0, this->width, this->height);
	GLfloat	zero = 0.0f;
	GLfloat one = 1.0f;
	if (bClearColour)
	{
		glClearBufferfv(GL_COLOR, 0, &zero);		// Colour
	}
	if (bClearDepth)
	{
		glClearBufferfv(GL_DEPTH, 0, &one);		// Depth is normalized 0.0 to 1.0f
	}
	// If buffer is GL_STENCIL, drawbuffer must be zero, and value points to a 
	//  single value to clear the stencil buffer to. Masking is performed in the 
	//  same fashion as for glClearStencil. Only the *iv forms of these commands 
	//  should be used to clear stencil buffers; be used to clear stencil buffers; 
	//  other forms do not accept a buffer of GL_STENCIL.

	// 
	glStencilMask(0xFF);

	{	// Clear stencil
		//GLint intZero = 0;
		//glClearBufferiv(GL_STENCIL, 0, &intZero );
		glClearBufferfi(GL_DEPTH_STENCIL,
			0,		// Must be zero
			1.0f,	// Clear value for depth
			0);	// Clear value for stencil
	}

	return;
}


int cFBO::getMaxColourAttachments(void)
{
	int maxColourAttach = 0;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColourAttach);

	return maxColourAttach;
}

int cFBO::getMaxDrawBuffers(void)
{
	int maxDrawBuffers = 0;
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers);

	return maxDrawBuffers;
}
