#pragma once

#include <vulkan/vulkan.h>
#include <matrix4x4.h>
#include <vec3f.h>

typedef struct UBOData
{
	Matrix4x4 model;
	Matrix4x4 view;
	Matrix4x4 proj;
	vec3f lightPos;
}UBODataType;

typedef struct UniformBufferObject_T
{
	UBOData data;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;

	VkBuffer buffer;
	VkDeviceMemory memory;

}UBO;

//test

typedef struct testData
{
	vec3f pos;
}testData;

template <typename T>
struct UniformSet
{
	T data;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;

	VkBuffer buffer;
	VkDeviceMemory memory;

	uint64_t typeSize()
	{
		return sizeof(T);
	}
};