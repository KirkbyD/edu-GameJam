#pragma once

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include "GLCommon.hpp"


class cGLSLShaderProgram {
public:
	std::string _shaderProgramName;

	cGLSLShaderProgram();
	~cGLSLShaderProgram();

	void LoadShaderFromString(GLenum shaderType, const std::string& source);
	void LoadShaderFromFile(GLenum shaderType, const std::string& filename);
	void LinkShadersToShaderProgram();

	void Use();
	void UnUse();

	void AddAttribute(const std::string& attribute);
	void AddUniform(const std::string& uniform);

	// An indexer that returns the location of the attribute/uniform
	GLuint operator[](const std::string& attribute);
	GLuint operator()(const std::string& uniform);

	void DeleteShaderProgram();

	// Prints out list of all active uniforms/attributes
	void AddAllActiveUniforms(bool print = false);
	void AddAllActiveAttributes(bool print = false);
	std::string GetNameStringFromType(int glVariableType);

	//TODO: Remove later
	GLuint GetShaderProgramID();

private:
	enum ShaderType { VERTEX_SHADER, FRAGMENT_SHADER, GEOMETRY_SHADER };
	GLuint _program;
	int _totalShaders;
	GLuint _shaders[3]; // 0->vertexshader, 1->fragmentshader, 2->geometryshader
	std::map<std::string, GLuint> _attributeList;
	std::map<std::string, GLuint> _uniformLocationList;
};