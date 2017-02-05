#include "scene.h"
#include <vklog.h>
#include <vkrenderer.h>
#include <vkdevice.h>

//#define VML_USE_VULKAN
//#include <matrix4x4.h>

Scene::Scene(VkRenderer *renderer)
	: m_renderer(renderer)
{
	vulkanDevice = m_renderer->m_vulkanDevice;
	m_device = m_renderer->m_device;
}


Scene::~Scene()
{
	releaseBuffers();
}

void Scene::buildVertexBuffer()
{
	LOG_SECTION("create vertex buffers");
	if (!meshs.size()) return;
	
	for (auto &mesh : meshs)
	{
		VkDeviceSize bufferSize = sizeof(mesh->vertices[0]) * mesh->vertices.size();
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
		memcpy(data, mesh->vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(m_device, stagingBufferMemory);

		//create buffer for real vertex buffer object and memory
		vulkanDevice->createBuffer(
			VK_BUFFER_USAGE_TRANSFER_DST_BIT |
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT ,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			mesh->vbo.buffer,
			mesh->vbo.memory,
			bufferSize
		);

		vulkanDevice->copyBuffer(stagingBuffer, mesh->vbo.buffer, bufferSize);


		LOG << "vertex buffer object : " << mesh->vbo.buffer << ENDL;
		LOG << "buffer size : " << bufferSize << ENDL;
		//we dont need staging buffer anymore
		vulkanDevice->destroyBuffer(stagingBuffer, stagingBufferMemory);
	}
}


void Scene::buildIndiceBuffer()
{
	LOG_SECTION("create index buffers");
	for (auto& mesh : meshs)
	{
		VkDeviceSize bufferSize = mesh->indiceBufferSize();

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
		memcpy(data, mesh->indices.data(), (size_t)bufferSize);
		vkUnmapMemory(m_device, stagingBufferMemory);

		vulkanDevice->createBuffer(
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			mesh->ibo.buffer,
			mesh->ibo.memory,
			bufferSize);
		
		vulkanDevice->copyBuffer(stagingBuffer, mesh->ibo.buffer, bufferSize);


		LOG << "index buffer object : " << mesh->ibo.buffer << ENDL;
		LOG << "buffer size : " << bufferSize << ENDL;
		vulkanDevice->destroyBuffer(stagingBuffer, stagingBufferMemory);
	}
}

void Scene::initUniformBuffer()
{
	LOG_SECTION("initialize uniform buffer");
	//keep staging buffer alive
	VkDeviceSize bufferSize = sizeof(ubo);

	//staging allocate memory buffer
	vulkanDevice->createBuffer(
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		ubo.stagingBuffer,
		ubo.stagingMemory,
		bufferSize);
	//local allocate memory
	vulkanDevice->createBuffer(
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		ubo.buffer,
		ubo.memory,
		bufferSize);
	LOG << "prepared uniform buffer size : " << bufferSize << ENDL;

}


void Scene::buildInputState()
{
	vertexInputBinding = Vertex::getBindingDescribtion();
	vertexInputAttrib = Vertex::getAttributeDescribtions();

	vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputState.pNext = NULL;
	vertexInputState.vertexBindingDescriptionCount = 1;
	vertexInputState.pVertexBindingDescriptions = &vertexInputBinding;
	vertexInputState.vertexAttributeDescriptionCount = vertexInputAttrib.size();
	vertexInputState.pVertexAttributeDescriptions = vertexInputAttrib.data();

}

void Scene::updateUnifomrBuffers()
{
	camera->update();
	Matrix4x4 modelRot;
	modelRot.rotate(AXIS::Y, 90);
	ubo.data.model = modelRot.transposed();

	float aspect = m_renderer->width / (float)m_renderer->height;
	camera->perspective(45.0f, aspect, 0.001f, 1000.f);

	ubo.data.proj = camera->proj.transposed();
	ubo.data.view = camera->view.transposed();

	//working far now
	ubo.data.lightPos = vec3f(-5*sin(m_renderer->frame * 0.2), 5, 5);


	void* data;
	vkMapMemory(m_device, ubo.stagingMemory, 0, sizeof(ubo.data), 0,&data);
	memcpy(data, &ubo.data, sizeof(ubo.data));
	vkUnmapMemory(m_device, ubo.stagingMemory);
	vulkanDevice->copyBuffer(ubo.stagingBuffer, ubo.buffer, sizeof(ubo.data));
}

void Scene::releaseBuffers()
{
	/*VBO IBO*/
	for (auto &mesh : meshs)
	{
		//delete buffer
		destroyBuffer(mesh->vbo);
		destroyBuffer(mesh->ibo);
		//delete pipeline
		vkDestroyPipeline(m_device, mesh->pipeline, nullptr);
	}
	/*UBO*/
	destroyBuffer(ubo.stagingBuffer, ubo.stagingMemory);
	destroyBuffer(ubo.buffer, ubo.memory);
}