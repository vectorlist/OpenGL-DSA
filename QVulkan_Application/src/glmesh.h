#pragma once

#include <glad/glad.h>
#include <mesh.h>

//test
class GLMesh : public Mesh
{
public:
	GLMesh();
	~GLMesh();

	GLuint vao = NULL;
	GLuint vbo = NULL;
	GLuint ibo = NULL;



	void build();
	void getSource(Mesh* mesh);
	void render();

	//primitive baisc
	void createSphere(float radius, uint32_t rings, uint32_t slice);

};

typedef std::shared_ptr<GLMesh> glmesh_ptr;

