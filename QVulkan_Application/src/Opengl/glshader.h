#pragma once

#include <glad/glad.h>
#include <string>
#include <map>
#include <vec2f.h>
#include <vec3f.h>
#include <matrix4x4.h>

#define NUM_SHADER 2
#define VERT 0
#define FRAG 1

class GLShader
{
public:
	GLShader();
	~GLShader();

	GLuint program;
	GLuint shader[NUM_SHADER];

	void buildShader(const std::string &vert, const std::string &frag);
	std::string codeFromFile(const std::string filename);

	GLuint createShader(const std::string &code, GLuint type);
	
	
	void debugCompile(GLuint shader, GLuint flag,
		const std::string &msg,bool isProgram = false);
	
	void setUniform1i(int data, const char* name);
	void setUuiform1f(float data, const char* name);
	void setUniform2i(const vec2i &data, const char* name);
	void setUniform2f(const vec2f &data, const char* name);
	void setUniform3f(const vec3f &data, const char* name);
	void setUniformMatrix4f(const Matrix4x4 &mat, const char* name, 
		bool transpose = false);

};

