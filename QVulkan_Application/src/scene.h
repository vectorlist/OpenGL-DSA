#pragma once

#include <vertex.h>
#include <vector>
#include <memory>
#include <matrix4x4.h>

struct Buffer
{
	VkBuffer buffer;
	VkDeviceMemory memory;
};

struct MeshObject
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	Buffer vbo;
	Buffer ibo;

	VkPipeline pipeline;

	void render(VkCommandBuffer cmd)
	{
		VkDeviceSize offset[1] = { 0 };

		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		vkCmdBindVertexBuffers(cmd, 0, 1, &vbo.buffer, offset);
		vkCmdBindIndexBuffer(cmd, ibo.buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(cmd, indices.size(), 1, 0, 0, 0);
	}


	uint64_t indiceBufferSize() const
	{
		return uint64_t(sizeof(indices[0]) * indices.size());
	}
};


typedef std::shared_ptr<MeshObject> mesh_ptr;

typedef struct UBOData
{
	Matrix4x4 model;
	Matrix4x4 view;
	Matrix4x4 proj;
}UBODataType;

typedef struct UniformBufferObject_T
{
	UBOData data;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;

	VkBuffer buffer;
	VkDeviceMemory memory;

}UBO;

class VkRenderer;
class VulkanDevice;
class Scene
{
public:
	Scene(VkRenderer *renderer);
	~Scene();

	VkRenderer* m_renderer;
	VulkanDevice* vulkanDevice;
	VkDevice m_device;
	bool isBuilt = false;

	std::vector<mesh_ptr> meshs;

	VkPipelineVertexInputStateCreateInfo vertexInputState = {};
	std::array<VkVertexInputAttributeDescription,3> vertexInputAttrib;
	VkVertexInputBindingDescription vertexInputBinding;


	/*VERTEX BUFFER*/
	void buildVertexBuffer();
	void buildIndiceBuffer();

	/*UNIFORM BUFFER*/
	UBO ubo;
	void initUniformBuffer();

	/*BUILD DESCRTIPTION AND STATE*/
	void buildInputState();

	void updateUnifomrBuffers();

	void releaseBuffers();

	void addObject(mesh_ptr obj);

	//built in
	void destroyBuffer(VkBuffer buffer, VkDeviceMemory memory);
	void destroyBuffer(Buffer &buffer);

};

inline void Scene::addObject(mesh_ptr obj)
{
	meshs.push_back(obj);
}

inline void Scene::destroyBuffer(VkBuffer buffer, VkDeviceMemory memory)
{
	vkDestroyBuffer(m_device, buffer, nullptr);
	vkFreeMemory(m_device, memory, nullptr);
}

inline void Scene::destroyBuffer(Buffer &buffer)
{
	vkDestroyBuffer(m_device, buffer.buffer, nullptr);
	vkFreeMemory(m_device, buffer.memory, nullptr);
}

