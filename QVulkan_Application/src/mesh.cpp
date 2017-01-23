#include <mesh.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <scene.h>



void meshTool::LoadModel(
	const std::string &filename,
	std::vector<Vertex> *vertices,
	std::vector<uint32_t> *indices)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str()))
		LOG_ASSERT(err);

	std::unordered_map<Vertex, int> uniqueVertices = {};

	for (const auto &shape : shapes)
	{
		for (const auto &index : shape.mesh.indices)
		{
			Vertex vertex;
			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.st = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f,1.0f, 0.0f };

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = vertices->size();
				vertices->push_back(vertex);
			}
			indices->push_back(uniqueVertices[vertex]);
		}
	}
	LOG << "vertices num : " << vertices->size() << ENDL;
	LOG << "indices num : " << indices->size() << ENDL;
}

void meshTool::LoadModel(
	const std::string &filename,
	MeshObject* mesh)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str()))
		LOG_ASSERT(err);

	std::unordered_map<Vertex, int> uniqueVertices = {};

	for (const auto &shape : shapes)
	{
		for (const auto &index : shape.mesh.indices)
		{
			Vertex vertex;
			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.normal = {
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			};

			vertex.st = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f,1.0f, 0.0f };

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = mesh->vertices.size();
				mesh->vertices.push_back(vertex);
			}
			mesh->indices.push_back(uniqueVertices[vertex]);
		}
	}
	LOG << "vertices num : " << mesh->vertices.size() << ENDL;
	LOG << "indices num : " << mesh->indices.size() << ENDL;
}

