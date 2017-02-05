#pragma once

#include <vulkan/vulkan.h>
#include <matrix4x4.h>
#include <vec3f.h>
#include <glad/glad.h>

//UBO DATA is shared btw gl and vulkan
typedef struct UBOData
{
	Matrix4x4 proj;
	Matrix4x4 view;
	Matrix4x4 model;
	vec3f lightPos;
}UBODataType;

typedef struct UBO
{
	UBOData data;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;

	VkBuffer buffer;
	VkDeviceMemory memory;

}UBO_T;


typedef struct GLUBO
{
	GLUBO();
	~GLUBO();
	UBOData data;
	GLuint buffer = NULL;

	void buildBuffers();

}GLUBO_T;

inline GLUBO::GLUBO()
{
	//buildBuffers();
}

inline GLUBO::~GLUBO()
{
	if (buffer != NULL)
		glDeleteBuffers(1, &buffer);
}

inline void GLUBO::buildBuffers()
{
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(data), &data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, NULL);
}
