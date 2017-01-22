#pragma once

#include <vertex.h>
#include <vector>
#include <memory>
#include <matrix4x4.h>

struct MeshObject
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	VkBuffer vbo = VK_NULL_HANDLE;
	VkDeviceMemory vboMemory = VK_NULL_HANDLE;

	VkBuffer ibo = VK_NULL_HANDLE;
	VkDeviceMemory iboMemory = VK_NULL_HANDLE;

	bool isEmpty() { 
		return (
			(vertices.size() == NULL) || (indices.size() == NULL) ||
			(iboMemory == NULL) || (ibo == NULL) ||
			(vboMemory == NULL)) || (vbo == NULL);
	}

	uint64_t indiceBufferSize() const
	{
		return uint64_t(sizeof(indices[0]) * indices.size());
	}
};


typedef std::shared_ptr<MeshObject> mesh_ptr;

typedef struct UBO_T
{
	Matrix4x4 proj;
	Matrix4x4 view;
	Matrix4x4 model;
}UBO;

typedef struct UniformObject_T
{
	UBO ubo;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	VkBuffer buffer;
	VkDeviceMemory bufferMemory;

}UniformObject;

class VulkanDevice;
class Scene
{
public:
	Scene(VulkanDevice* device);
	~Scene();

	VulkanDevice* vulkanDevice;
	VkDevice m_device;
	bool isBuilt = false;

	std::vector<mesh_ptr> objects;


	/*VERTEX BUFFER*/
	void buildVertexBuffer();
	void buildIndiceBuffer();

	/*UNIFORM BUFFER*/
	UniformObject uniObj;
	void initUniformBuffer();

	void releaseBuffers();

	void addObject(mesh_ptr obj);

	//built in
	VkBuffer& unifromBuffer() {
		return uniObj.buffer;
	}

};

inline void Scene::addObject(mesh_ptr obj)
{
	//if (obj->isEmpty()) return;
	objects.push_back(obj);
}

