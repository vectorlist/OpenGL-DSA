#include "scene.h"
#include <vkdevice.h>


Scene::Scene(VulkanDevice * device) 
	: vulkanDevice(device)
{
	//instace flash device for buffer objects
	m_device = device->m_device;
	if (m_device == nullptr) qFatal("fatal : could not get device");
}


Scene::~Scene()
{
	//ok far now
	releaseBuffers();
}

void Scene::buildVertexBuffer()
{
	LOG_SECTION("create vertex buffers");
	if (!objects.size()) return;
	
	for (auto &object : objects)
	{
		VkDeviceSize bufferSize = sizeof(object->vertices[0]) * object->vertices.size();
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		vulkanDevice->createBuffer(
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory,
			bufferSize);

		void* data;
		vkMapMemory(m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, object->vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(m_device, stagingBufferMemory);

		//create buffer for real vertex buffer object and memory
		vulkanDevice->createBuffer(
			VK_BUFFER_USAGE_TRANSFER_DST_BIT |
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT ,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			object->vbo,
			object->vboMemory,
			bufferSize
		);

		vulkanDevice->copyBuffer(stagingBuffer, object->vbo, bufferSize);


		LOG << "vertex buffer object : " <<object->vbo << ENDL;
		LOG << "buffer size : " << bufferSize << ENDL;
		//we dont need staging buffer anymore
		vulkanDevice->destroyBuffer(stagingBuffer, stagingBufferMemory);
	}
}


void Scene::buildIndiceBuffer()
{
	LOG_SECTION("create index buffers");
	for (auto& object : objects)
	{
		VkDeviceSize bufferSize = object->indiceBufferSize();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		vulkanDevice->createBuffer(
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory,
			bufferSize);

		void* data;
		vkMapMemory(m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, object->indices.data(), (size_t)bufferSize);
		vkUnmapMemory(m_device, stagingBufferMemory);

		vulkanDevice->createBuffer(
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			object->ibo,
			object->iboMemory,
			bufferSize);

		vulkanDevice->copyBuffer(stagingBuffer, object->ibo, bufferSize);


		LOG << "index buffer object : " << object->ibo << ENDL;
		LOG << "buffer size : " << bufferSize << ENDL;
		vulkanDevice->destroyBuffer(stagingBuffer, stagingBufferMemory);
	}
}

void Scene::initUniformBuffer()
{
	LOG_SECTION("initialize uniform buffer");
	//keep staging buffer alive
	VkDeviceSize bufferSize = sizeof(UBO);

	//flash instance
	vulkanDevice->createBuffer(
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		uniObj.stagingBuffer,
		uniObj.stagingBufferMemory,
		bufferSize);
	//local allocate memory
	vulkanDevice->createBuffer(
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		uniObj.buffer,
		uniObj.bufferMemory,
		bufferSize);
	LOG << "prepared uniform buffer size : " << bufferSize << ENDL;
}

void Scene::releaseBuffers()
{
	//if ((!m_device) && !isBuilt) LOG_WARN("buffer is not correct");
	for (auto &o : objects)
	{
		vkDestroyBuffer(m_device, o->vbo, nullptr);
		vkDestroyBuffer(m_device, o->ibo, nullptr);

		vkFreeMemory(m_device, o->vboMemory, nullptr);
		vkFreeMemory(m_device, o->iboMemory, nullptr);
	}
	/*UBO*/

	vulkanDevice->destroyBuffer(uniObj.stagingBuffer, uniObj.stagingBufferMemory);
	vulkanDevice->destroyBuffer(uniObj.buffer, uniObj.bufferMemory);

}