#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

#include "cGLSLShaderProgram.hpp"

cGLSLShaderProgram::cGLSLShaderProgram()
{
	_program = glCreateProgram();
	_shaderProgramName = "";
	_totalShaders = 0;
	_shaders[VERTEX_SHADER] = 0;
	_shaders[FRAGMENT_SHADER] = 0;
	_shaders[GEOMETRY_SHADER] = 0;
	_attributeList.clear();
	_uniformLocationList.clear();
}

cGLSLShaderProgram::~cGLSLShaderProgram()
{
	DeleteShaderProgram();
	_attributeList.clear();
	_uniformLocationList.clear();
}

void cGLSLShaderProgram::LoadShaderFromString(GLenum shaderType, const std::string& source) {
	GLuint shader = glCreateShader(shaderType);

	const char* ptmp = source.c_str();
	glShaderSource(shader, 1, &ptmp, NULL);

	//check whether the shader loads fine
	GLint status;
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* infoLog = new GLchar[infoLogLength];
		glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
		std::cerr << "Compile log: " << infoLog << std::endl;
		delete[] infoLog;
	}
	_shaders[_totalShaders++] = shader;
}

void cGLSLShaderProgram::LoadShaderFromFile(GLenum shaderType, const std::string& filename) {
	std::ifstream fp;
	fp.open(filename.c_str(), std::ios_base::in);
	if (fp) {
		std::string line, buffer;
		while (getline(fp, line)) {
			buffer.append(line);
			buffer.append("\r\n");
		}
		//copy to source
		LoadShaderFromString(shaderType, buffer);
	}
	else {
		std::cerr << "Error loading shader: " << filename << std::endl;
	}
}

void cGLSLShaderProgram::LinkShadersToShaderProgram() {
	if (_shaders[VERTEX_SHADER] != 0) {
		glAttachShader(_program, _shaders[VERTEX_SHADER]);
	}
	if (_shaders[FRAGMENT_SHADER] != 0) {
		glAttachShader(_program, _shaders[FRAGMENT_SHADER]);
	}
	if (_shaders[GEOMETRY_SHADER] != 0) {
		glAttachShader(_program, _shaders[GEOMETRY_SHADER]);
	}

	//link and check whether the program links fine
	GLint status;
	glLinkProgram(_program);
	glGetProgramiv(_program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;

		glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* infoLog = new GLchar[infoLogLength];
		glGetProgramInfoLog(_program, infoLogLength, NULL, infoLog);
		std::cerr << "Link log: " << infoLog << std::endl;
		delete[] infoLog;
	}

	glDeleteShader(_shaders[VERTEX_SHADER]);
	glDeleteShader(_shaders[FRAGMENT_SHADER]);
	glDeleteShader(_shaders[GEOMETRY_SHADER]);
}

void cGLSLShaderProgram::Use() {
	glUseProgram(_program);
}

void cGLSLShaderProgram::UnUse() {
	glUseProgram(0);
}

void cGLSLShaderProgram::AddAttribute(const std::string& attribute) {
	_attributeList[attribute] = glGetAttribLocation(_program, attribute.c_str());
}

void cGLSLShaderProgram::AddUniform(const std::string& uniform) {
	_uniformLocationList[uniform] = glGetUniformLocation(_program, uniform.c_str());
}

GLuint cGLSLShaderProgram::operator [](const std::string& attribute) {
	return _attributeList[attribute];
}

GLuint cGLSLShaderProgram::operator()(const std::string& uniform) {
	return _uniformLocationList[uniform];
}

void cGLSLShaderProgram::DeleteShaderProgram() {
	glDeleteProgram(_program);
}

void cGLSLShaderProgram::AddAllActiveUniforms(bool print)
{
	// Find out the max size of the buffer we need. 
	int maxNameSize = 0;
	glGetProgramiv(_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameSize);

	if (print)
	{
		std::cout << "Max uniform name size is : " << maxNameSize << std::endl;
	}

	// Create a buffer of this max size
	char* uniformName = new char[maxNameSize];

	// Clear it
	memset(uniformName, 0, maxNameSize);

	// How many uniforms
	int numberOfActiveUniforms = 0;
	glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &numberOfActiveUniforms);

	if (print)
	{
		std::cout << "There are " << numberOfActiveUniforms << " active uniforms" << std::endl;
	}

	for (int index = 0; index != numberOfActiveUniforms; index++)
	{
		const int BUFFERSIZE = 1000;
		char myBuffer[BUFFERSIZE] = { 0 };

		GLsizei numCharWritten = 0;
		GLint sizeOfVariable = 0;
		GLenum uniformType = 0;
		memset(uniformName, 0, maxNameSize);

		glGetActiveUniform(_program,
			index,				// Uniform index (0 to ...)
			BUFFERSIZE,			// Max number of chars
			&numCharWritten,	// How many it REALLY wrote
			&sizeOfVariable,	// Size in bytes
			&uniformType,
			uniformName);

		AddUniform(uniformName);

		if (print)
		{
			std::cout << "Uniform # " << index << std::endl;
			std::cout << "\t" << uniformName << std::endl;
			std::cout << "\t" << GetNameStringFromType(uniformType) << std::endl;
			std::cout << "\t" << sizeOfVariable << std::endl;
		}
	}

	return;
}

void cGLSLShaderProgram::AddAllActiveAttributes(bool print)
{
	// Find out the max size of the buffer we need. 
	int maxNameSize = 0;
	glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameSize);

	if (print)
	{
		std::cout << "Max attribute name size is : " << maxNameSize << std::endl;
	}

	// Create a buffer of this max size
	char* attributeName = new char[maxNameSize];

	// Clear it
	memset(attributeName, 0, maxNameSize);

	// How many attributes
	int numberOfActiveAttributes = 0;
	glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &numberOfActiveAttributes);

	if (print)
	{
		std::cout << "There are " << numberOfActiveAttributes << " active attributes" << std::endl;
	}

	for (int index = 0; index != numberOfActiveAttributes; index++)
	{
		const int BUFFERSIZE = 1000;
		char myBuffer[BUFFERSIZE] = { 0 };

		GLsizei numCharWritten = 0;
		GLint sizeOfVariable = 0;
		GLenum attributeType = 0;
		memset(attributeName, 0, maxNameSize);

		glGetActiveAttrib(_program,
			index,				// Attribute index (0 to ...)
			BUFFERSIZE,			// Max number of chars
			&numCharWritten,	// How many it REALLY wrote
			&sizeOfVariable,	// Size in bytes
			&attributeType,
			attributeName);

		AddAttribute(attributeName);

		if (print)
		{
			std::cout << "Attribute # " << index << std::endl;
			std::cout << "\t" << attributeName << std::endl;
			std::cout << "\t" << GetNameStringFromType(attributeType) << std::endl;
			std::cout << "\t" << sizeOfVariable << std::endl;
		}
	}

	return;
}

std::string cGLSLShaderProgram::GetNameStringFromType(int glVariableType)
{
	switch (glVariableType)
	{
	case GL_FLOAT: return "float"; break;
	case GL_FLOAT_VEC2: return "vec2"; break;
	case GL_FLOAT_VEC3: return "vec3"; break;
	case GL_FLOAT_VEC4: return "vec4"; break;
	case GL_DOUBLE: return "double"; break;
	case GL_DOUBLE_VEC2: return "dvec2"; break;
	case GL_DOUBLE_VEC3: return "dvec3"; break;
	case GL_DOUBLE_VEC4: return "dvec4"; break;
	case GL_INT: return "int"; break;
	case GL_INT_VEC2: return "ivec2"; break;
	case GL_INT_VEC3: return "ivec3"; break;
	case GL_INT_VEC4: return "ivec4"; break;
	case GL_UNSIGNED_INT: return "unsigned int"; break;
	case GL_UNSIGNED_INT_VEC2: return "uvec2"; break;
	case GL_UNSIGNED_INT_VEC3: return "uvec3"; break;
	case GL_UNSIGNED_INT_VEC4: return "uvec4"; break;
	case GL_BOOL: return "bool"; break;
	case GL_BOOL_VEC2: return "bvec2"; break;
	case GL_BOOL_VEC3: return "bvec3"; break;
	case GL_BOOL_VEC4: return "bvec4"; break;
	case GL_FLOAT_MAT2: return "mat2"; break;
	case GL_FLOAT_MAT3: return "mat3"; break;
	case GL_FLOAT_MAT4: return "mat4"; break;
	case GL_FLOAT_MAT2x3: return "mat2x3"; break;
	case GL_FLOAT_MAT2x4: return "mat2x4"; break;
	case GL_FLOAT_MAT3x2: return "mat3x2"; break;
	case GL_FLOAT_MAT3x4: return "mat3x4"; break;
	case GL_FLOAT_MAT4x2: return "mat4x2"; break;
	case GL_FLOAT_MAT4x3: return "mat4x3"; break;
	case GL_DOUBLE_MAT2: return "dmat2"; break;
	case GL_DOUBLE_MAT3: return "dmat3"; break;
	case GL_DOUBLE_MAT4: return "dmat4"; break;
	case GL_DOUBLE_MAT2x3: return "dmat2x3"; break;
	case GL_DOUBLE_MAT2x4: return "dmat2x4"; break;
	case GL_DOUBLE_MAT3x2: return "dmat3x2"; break;
	case GL_DOUBLE_MAT3x4: return "dmat3x4"; break;
	case GL_DOUBLE_MAT4x2: return "dmat4x2"; break;
	case GL_DOUBLE_MAT4x3: return "dmat4x3"; break;
	case GL_SAMPLER_1D: return "sampler1D"; break;
	case GL_SAMPLER_2D: return "sampler2D"; break;
	case GL_SAMPLER_3D: return "sampler3D"; break;
	case GL_SAMPLER_CUBE: return "samplerCube"; break;
	case GL_SAMPLER_1D_SHADOW: return "sampler1DShadow"; break;
	case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow"; break;
	case GL_SAMPLER_1D_ARRAY: return "sampler1DArray"; break;
	case GL_SAMPLER_2D_ARRAY: return "sampler2DArray"; break;
	case GL_SAMPLER_1D_ARRAY_SHADOW: return "sampler1DArrayShadow"; break;
	case GL_SAMPLER_2D_ARRAY_SHADOW: return "sampler2DArrayShadow"; break;
	case GL_SAMPLER_2D_MULTISAMPLE: return "sampler2DMS"; break;
	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY: return "sampler2DMSArray"; break;
	case GL_SAMPLER_CUBE_SHADOW: return "samplerCubeShadow"; break;
	case GL_SAMPLER_BUFFER: return "samplerBuffer"; break;
	case GL_SAMPLER_2D_RECT: return "sampler2DRect"; break;
	case GL_SAMPLER_2D_RECT_SHADOW: return "sampler2DRectShadow"; break;
	case GL_INT_SAMPLER_1D: return "isampler1D"; break;
	case GL_INT_SAMPLER_2D: return "isampler2D"; break;
	case GL_INT_SAMPLER_3D: return "isampler3D"; break;
	case GL_INT_SAMPLER_CUBE: return "isamplerCube"; break;
	case GL_INT_SAMPLER_1D_ARRAY: return "isampler1DArray"; break;
	case GL_INT_SAMPLER_2D_ARRAY: return "isampler2DArray"; break;
	case GL_INT_SAMPLER_2D_MULTISAMPLE: return "isampler2DMS"; break;
	case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "isampler2DMSArray"; break;
	case GL_INT_SAMPLER_BUFFER: return "isamplerBuffer"; break;
	case GL_INT_SAMPLER_2D_RECT: return "isampler2DRect"; break;
	case GL_UNSIGNED_INT_SAMPLER_1D: return "usampler1D"; break;
	case GL_UNSIGNED_INT_SAMPLER_2D: return "usampler2D"; break;
	case GL_UNSIGNED_INT_SAMPLER_3D: return "usampler3D"; break;
	case GL_UNSIGNED_INT_SAMPLER_CUBE: return "usamplerCube"; break;
	case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY: return "usampler2DArray"; break;
	case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: return "usampler2DArray"; break;
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE: return "usampler2DMS"; break;
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "usampler2DMSArray"; break;
	case GL_UNSIGNED_INT_SAMPLER_BUFFER: return "usamplerBuffer"; break;
	case GL_UNSIGNED_INT_SAMPLER_2D_RECT: return "usampler2DRect"; break;
	case GL_IMAGE_1D: return "image1D"; break;
	case GL_IMAGE_2D: return "image2D"; break;
	case GL_IMAGE_3D: return "image3D"; break;
	case GL_IMAGE_2D_RECT: return "image2DRect"; break;
	case GL_IMAGE_CUBE: return "imageCube"; break;
	case GL_IMAGE_BUFFER: return "imageBuffer"; break;
	case GL_IMAGE_1D_ARRAY: return "image1DArray"; break;
	case GL_IMAGE_2D_ARRAY: return "image2DArray"; break;
	case GL_IMAGE_2D_MULTISAMPLE: return "image2DMS"; break;
	case GL_IMAGE_2D_MULTISAMPLE_ARRAY: return "image2DMSArray"; break;
	case GL_INT_IMAGE_1D: return "iimage1D"; break;
	case GL_INT_IMAGE_2D: return "iimage2D"; break;
	case GL_INT_IMAGE_3D: return "iimage3D"; break;
	case GL_INT_IMAGE_2D_RECT: return "iimage2DRect"; break;
	case GL_INT_IMAGE_CUBE: return "iimageCube"; break;
	case GL_INT_IMAGE_BUFFER: return "iimageBuffer"; break;
	case GL_INT_IMAGE_1D_ARRAY: return "iimage1DArray"; break;
	case GL_INT_IMAGE_2D_ARRAY: return "iimage2DArray"; break;
	case GL_INT_IMAGE_2D_MULTISAMPLE: return "iimage2DMS"; break;
	case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY: return "iimage2DMSArray"; break;
	case GL_UNSIGNED_INT_IMAGE_1D: return "uimage1D"; break;
	case GL_UNSIGNED_INT_IMAGE_2D: return "uimage2D"; break;
	case GL_UNSIGNED_INT_IMAGE_3D: return "uimage3D"; break;
	case GL_UNSIGNED_INT_IMAGE_2D_RECT: return "uimage2DRect"; break;
	case GL_UNSIGNED_INT_IMAGE_CUBE: return "uimageCube"; break;
	case GL_UNSIGNED_INT_IMAGE_BUFFER: return "uimageBuffer"; break;
	case GL_UNSIGNED_INT_IMAGE_1D_ARRAY: return "uimage1DArray"; break;
	case GL_UNSIGNED_INT_IMAGE_2D_ARRAY: return "uimage2DArray"; break;
	case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE: return "uimage2DMS"; break;
	case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY: return "uimage2DMSArray"; break;
	case GL_UNSIGNED_INT_ATOMIC_COUNTER: return "atomic_uint"; break;

	default:
		return "Unknown";
	}

	return "Unknown";
}

GLuint cGLSLShaderProgram::GetShaderProgramID()
{
	return _program;
}