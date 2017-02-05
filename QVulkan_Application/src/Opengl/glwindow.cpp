#include "glwindow.h"
#include <mesh.h>
#include <glshader.h>
#include <scene.h>
#include <QMouseEvent>

#ifdef VML_USE_VULKAN
#undef VML_USE_VULKAN
#endif
#include <matrix4x4.h>

GLWindow::GLWindow(QWidget* parent) 
	: QOpenGLWidget(parent), m_shader(NULL), scene(NULL)
{
	/*gladLoadGL();*/
	
}


GLWindow::~GLWindow()
{
	SAFE_DELETE(m_shader);
}

void GLWindow::initializeGL()
{
	gladLoadGL();

	m_shader = new GLShader();
	m_shader->buildShader("./shader/shader.vert", "./shader/shader.frag");

	m_mesh = glmesh_ptr(new GLMesh);
	m_mesh->createSphere(1, 12, 12);

	ubo.buildBuffers();

	//AbstractMesh* mesh << m_mesh;
	glClearColor(0.125f, 0.125f, 0.125f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}

void GLWindow::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
	updateUniformBuffer(w, h);

}

void GLWindow::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	m_shader->bind();

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo.buffer);
	m_mesh->render();


}

void GLWindow::mousePressEvent(QMouseEvent *e)
{
	//if()
	if (scene && e->buttons() == Qt::RightButton)
	{
		LOG << "vulkan mesh to gl mesh" << ENDL;
		//Mesh* mesh = dynamic_cast<Mesh*>(scene->meshs[0].get());
		//m_mesh->getSource(mesh);
		//m_mesh->createSphere(1, 10, 10);
		update();

	}
	QOpenGLWidget::mousePressEvent(e);
}

void GLWindow::updateUniformBuffer(int w, int h)
{
	float aspect = w / (float)h;
	ubo.data.proj = vml::perspectiveRH(45.f, aspect, 0.001f, 1000.f).transposed();
	ubo.data.view = vml::lookAt(vec3f(3, 3, 3), vec3f(0, 0, 0), vec3f(0, 1, 0)).transposed();
	ubo.data.lightPos = vec3f(4, 4, 4);

	glBindBuffer(GL_UNIFORM_BUFFER, ubo.buffer);
	GLvoid* data = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	memcpy(data, &ubo.data, sizeof(ubo.data));
	glUnmapBuffer(GL_UNIFORM_BUFFER);

}
