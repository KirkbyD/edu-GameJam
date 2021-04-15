#pragma once

#include <string>
#include <map>
#include "GLCommon.hpp"

#include "cGLSLShaderProgram.hpp"

class cGLSLShaderManager {
public:

	// Singleton
	static cGLSLShaderManager* GetShaderManager();

	// Destuctor
	~cGLSLShaderManager();

	//void UseShaderProgram(std::string shaderProgramName);

	// Recreates existing shader if there is one
	void CreateShaderProgram(std::string shaderProgramName);

	// Creates shader for existing program
	void CreateShader(std::string shaderProgramName, GLenum shaderType, const std::string& filename);

	// Links shader program's shaders
	void LinkShaders(std::string shaderProgramName);

	// Links all shader programs' shaders
	void LinkAllShaders();

	// Links all shader program's active uniforms/attributes
	void AddAllActiveUniforms(std::string shaderProgramName, bool print = false);
	void AddAllActiveAttributes(std::string shaderProgramName, bool print = false);

	// returns 0 on error
	GLuint FindShaderByName(std::string shaderProgramName);

	void UseShader(std::string shaderProgramName);
	void UnUseShader();


private:
	// Singleton
	static cGLSLShaderManager* stonShaderMngr;
	cGLSLShaderManager();

	// Map of shader programs
	std::map<std::string, cGLSLShaderProgram*> mShaderPrograms;
};
