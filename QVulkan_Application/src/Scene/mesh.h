#pragma once

#include <vktools.h>
#include <vertex.h>

class VKMesh
{
public:
	

};

class MeshObject;
namespace meshTool
{
	void LoadModel(
		const std::string &filename,
		std::vector<Vertex> *vertices,
		std::vector<uint32_t> *indices);

	void LoadModel(
		const std::string &filename,
		MeshObject* mesh);
}
 

