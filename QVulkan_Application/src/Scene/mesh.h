#pragma once

#include <vktools.h>
#include <vertex.h>

typedef struct Buffer
{
	VkBuffer buffer;
	VkDeviceMemory memory;
}Buffer;

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

};

class VKMesh : public Mesh
{
public:
	VKMesh() : Mesh(){}

	Buffer vbo;
	Buffer ibo;
	VkPipeline pipeline = VK_NULL_HANDLE;
	/*VkPipeline pipeline;*/

	void render(VkCommandBuffer cmd, VkPipeline inPipeline = NULL)
	{
		VkDeviceSize offset[1] = { 0 };
		if (inPipeline)
			vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, inPipeline);
		else
			vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		vkCmdBindVertexBuffers(cmd, 0, 1, &vbo.buffer, offset);
		vkCmdBindIndexBuffer(cmd, ibo.buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(cmd, indices.size(), 1, 0, 0, 0);
	}

	//VkPipeline& getPipeline()  { return pipeline; };
	uint64_t indiceBufferSize() const
	{
		return uint64_t(sizeof(indices[0]) * indices.size());
	}
};

typedef std::shared_ptr<VKMesh> vkmesh_ptr;

namespace meshTool
{
	void LoadModel(
		const std::string &filename,
		std::vector<Vertex> *vertices,
		std::vector<uint32_t> *indices);

	void LoadModel(
		const std::string &filename,
		Mesh* mesh);
}
 

