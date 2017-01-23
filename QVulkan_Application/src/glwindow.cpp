#include "glwindow.h"
#include <mesh.h>
#include <glshader.h>

GLWindow::GLWindow(QWidget* parent) 
	: QOpenGLWidget(parent), m_shader(NULL)
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

	

	//AbstractMesh* mesh << m_mesh;
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}

void GLWindow::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
}

void GLWindow::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
