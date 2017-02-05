#pragma once

#define VML_USE_VULKAN
#include <matrix4x4.h>

#include <vertex.h>
#include <vector>
#include <memory>
#include <shader.h>
#include <ubo.h>
#include <camera.h>
#include <mesh.h>


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

	std::vector<vkmesh_ptr> meshs;
	std::vector<shader_ptr> shaders;
	UBO ubo;
	camera_ptr camera;

	VkPipelineVertexInputStateCreateInfo vertexInputState = {};
	std::array<VkVertexInputAttributeDescription,4> vertexInputAttrib;
	VkVertexInputBindingDescription vertexInputBinding;

	void buildVertexBuffer();
	void buildIndiceBuffer();
	void initUniformBuffer();
	void buildInputState();

	void updateUnifomrBuffers();

	void releaseBuffers();

	void addElement(vkmesh_ptr mesh);
	void addElement(shader_ptr shader);
	void addElement(camera_ptr cam);


	//built in
	void destroyBuffer(VkBuffer buffer, VkDeviceMemory memory);
	void destroyBuffer(Buffer &buffer);

};

inline void Scene::addElement(vkmesh_ptr mesh)
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

