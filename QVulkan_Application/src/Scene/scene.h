#pragma once

#define VML_USE_VULKAN
#include <matrix4x4.h>

#include <vertex.h>
#include <vector>
#include <memory>
#include <shader.h>
#include <ubo.h>
#include <camera.h>

typedef struct Buffer
{
	VkBuffer buffer;
	VkDeviceMemory memory;
}Buffer;

struct MeshObject
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	Buffer vbo;
	Buffer ibo;

	VkPipeline pipeline;

	void render(VkCommandBuffer cmd, VkPipeline inPipeline, bool stage)
	{
		VkDeviceSize offset[1] = { 0 };

		if (!stage)
			vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		else
			vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, inPipeline);
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
	std::vector<shader_ptr> shaders;
	UBO ubo;
	camera_ptr camera;
	VkPipeline wireframePipeline;

	VkPipelineVertexInputStateCreateInfo vertexInputState = {};
	std::array<VkVertexInputAttributeDescription,4> vertexInputAttrib;
	VkVertexInputBindingDescription vertexInputBinding;

	void buildVertexBuffer();
	void buildIndiceBuffer();
	void initUniformBuffer();
	void buildInputState();

	void updateUnifomrBuffers();

	void releaseBuffers();

	void addElement(mesh_ptr mesh);
	void addElement(shader_ptr shader);
	void addElement(camera_ptr cam);


	//built in
	void destroyBuffer(VkBuffer buffer, VkDeviceMemory memory);
	void destroyBuffer(Buffer &buffer);

};

inline void Scene::addElement(mesh_ptr mesh)
{
	meshs.push_back(mesh);
}

inline void Scene::addElement(shader_ptr shader)
{
	shaders.push_back(shader);
}

inline void Scene::addElement(camera_ptr cam)
{
	if (camera != nullptr) return;
	camera = cam;
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

