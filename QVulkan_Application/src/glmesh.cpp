#include "glmesh.h"

GLMesh::GLMesh() : Mesh()
{
	glGenBuffers(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
}


GLMesh::~GLMesh()
{
	if(vao != NULL)
		glDeleteBuffers(1, &vao);
	if (vbo != NULL)
		glDeleteBuffers(1, &vbo);
	if (ibo != NULL)
		glDeleteBuffers(1, &ibo);
}

void GLMesh::build()
{
	/*glGenBuffers(1, &vao);
	glGenBuffers(1, &vbo);*/

	//enable vao
	glBindVertexArray(vao);

	//set to vbo and locations
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	/*POS*/
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)0);

	/*NORMAL*/
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, Vertex::normal));

	/*COLOR*/
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, Vertex::color));

	/*ST*/
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, Vertex::st));

	//unbind vao
	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	//IBO
	/*glGenBuffers(1, &ibo);*/
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t),
		indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
}

void GLMesh::getSource(Mesh *mesh)
{
	/*this->vertices = mesh->vertices;
	this->indices = mesh->indices;*/
	vertices.resize(mesh->vertices.size());
	indices.resize(mesh->indices.size());
	std::copy(mesh->vertices.begin(), mesh->vertices.end(), vertices.begin());
	std::copy(mesh->indices.begin(), mesh->indices.end(), indices.begin());

	glGenBuffers(1, &vao);
	glGenBuffers(1, &vbo);

	//enable vao
	glBindVertexArray(vao);

	//set to vbo and locations
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	/*POS*/
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)0);

	/*NORMAL*/
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, Vertex::normal));

	/*COLOR*/
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, Vertex::color));

	/*ST*/
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, Vertex::st));

	//unbind vao
	glBindVertexArray(NULL);

	//IBO
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t),
		indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
}

void GLMesh::render()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
	glBindVertexArray(NULL);
}


void GLMesh::createSphere(float radius, uint32_t rings, uint32_t slice)
{
	int faces = slice * rings;
	int ibv = slice * rings;

	vertices.resize(ibv);
	indices.resize(6 * faces);

	float x, y, z, r, s;
	int idx = 0, tidx = 0;

	float const R = 1.f / (float)(rings - 1);
	float const S = 1.f / (float)(slice - 1);

	for (r = 0; r < rings; ++r)
	{
		for (s = 0; s < slice; ++s)
		{
			x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
			z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);
			y = sin(-M_PI_2 + (M_PI * r * R));

			Vertex vertex;
			vertex.pos = vec3f(x, y, z) * radius;
			vertex.normal = vec3f(x, y, z);
			vertex.st = vec2f(s*S, r*R);

			vertices[idx] = vertex;
			idx++;
			tidx++;
		}
	}

	idx = 0;
	int ringStart, nextRingStart, nextSlice;

	for (r = 0; r < rings; ++r)
	{
		ringStart = r * slice;
		nextRingStart = (r + 1) * slice;
		for (s = 0; s < slice; ++s)
		{
			nextSlice = s + 1;

			indices[idx] = ringStart + s;
			indices[idx + 1] = nextRingStart + s;
			indices[idx + 2] = nextRingStart + nextSlice;
			indices[idx + 3] = ringStart + s;
			indices[idx + 4] = nextRingStart + nextSlice;
			indices[idx + 5] = ringStart + nextSlice;
			idx += 6;
		}
	}
	build();
}