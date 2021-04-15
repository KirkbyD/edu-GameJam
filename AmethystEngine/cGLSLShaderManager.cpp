#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__, __func__)
#else
#define DBG_NEW
#endif

#include "cGLSLShaderManager.hpp"

#pragma region SINGLETON
cGLSLShaderManager* cGLSLShaderManager::stonShaderMngr = 0;
cGLSLShaderManager* cGLSLShaderManager::GetShaderManager() {
	if (stonShaderMngr == 0)
	{
		cGLSLShaderManager::stonShaderMngr = new cGLSLShaderManager();
	}
	return stonShaderMngr;
}
cGLSLShaderManager::cGLSLShaderManager() { printf("Shader Manager Created\n"); }
#pragma endregion

cGLSLShaderManager::~cGLSLShaderManager()
{
	if (!mShaderPrograms.empty())
	{
		for (auto it = mShaderPrograms.cbegin(); it != mShaderPrograms.cend() /* not hoisted */; /* no increment */)
		{
			delete it->second;
			it = mShaderPrograms.erase(it);
		}
	}
}

void cGLSLShaderManager::CreateShaderProgram(std::string shaderProgramName)
{
	if (mShaderPrograms.find(shaderProgramName) != mShaderPrograms.end())
	{
		delete mShaderPrograms[shaderProgramName];
		mShaderPrograms[shaderProgramName] = new cGLSLShaderProgram();
		mShaderPrograms[shaderProgramName]->_shaderProgramName = shaderProgramName;
	}
	else
	{
		mShaderPrograms[shaderProgramName] = new cGLSLShaderProgram();
		mShaderPrograms[shaderProgramName]->_shaderProgramName = shaderProgramName;
	}
}

void cGLSLShaderManager::CreateShader(std::string shaderProgramName, GLenum shaderType, const std::string& filename)
{
	if (mShaderPrograms.find(shaderProgramName) != mShaderPrograms.end())
	{
		mShaderPrograms[shaderProgramName]->LoadShaderFromFile(shaderType, filename);
	}
	else
	{
		printf("Shader program '%s' doesn't exist\n", shaderProgramName.c_str());
	}
}

void cGLSLShaderManager::LinkShaders(std::string shaderProgramName)
{
	if (mShaderPrograms.find(shaderProgramName) != mShaderPrograms.end())
	{
		mShaderPrograms[shaderProgramName]->LinkShadersToShaderProgram();
	}
	else
	{
		printf("Shader program '%s' doesn't exist\n", shaderProgramName.c_str());
	}
}

void cGLSLShaderManager::LinkAllShaders()
{
	if (!mShaderPrograms.empty())
	{
		for (auto const& it : mShaderPrograms)
		{
			it.second->LinkShadersToShaderProgram();
		}
	}
}

void cGLSLShaderManager::AddAllActiveUniforms(std::string shaderProgramName, bool print)
{
	if (mShaderPrograms.find(shaderProgramName) != mShaderPrograms.end())
	{
		mShaderPrograms[shaderProgramName]->AddAllActiveUniforms(print);
	}
	else
	{
		printf("Shader program '%s' doesn't exist\n", shaderProgramName.c_str());
	}
}

void cGLSLShaderManager::AddAllActiveAttributes(std::string shaderProgramName, bool print)
{
	if (mShaderPrograms.find(shaderProgramName) != mShaderPrograms.end())
	{
		mShaderPrograms[shaderProgramName]->AddAllActiveAttributes(print);
	}
	else
	{
		printf("Shader program '%s' doesn't exist\n", shaderProgramName.c_str());
	}
}

GLuint cGLSLShaderManager::FindShaderByName(std::string shaderProgramName)
{
	if (mShaderPrograms.find(shaderProgramName) != mShaderPrograms.end())
	{
		return mShaderPrograms[shaderProgramName]->GetShaderProgramID();
	}
	else
	{
		printf("Shader program '%s' doesn't exist\n", shaderProgramName.c_str());
		return 0;
	}
}

void cGLSLShaderManager::UseShader(std::string shaderProgramName)
{
	if (mShaderPrograms.find(shaderProgramName) != mShaderPrograms.end())
	{
		mShaderPrograms[shaderProgramName]->Use();
	}
	else
	{
		printf("Shader program '%s' doesn't exist\n", shaderProgramName.c_str());
	}
}

void cGLSLShaderManager::UnUseShader()
{
	glUseProgram(0);
}
