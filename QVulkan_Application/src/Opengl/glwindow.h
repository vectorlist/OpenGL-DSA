#pragma once

#include <glad/glad.h>
#include <qopenglwidget.h>
#include <glmesh.h>
#include <ubo.h>

//struct GLUBO
//{
//	UBOData data;
//	GLuint buffer;
//};

class GLShader;
class Scene;
class GLWindow : public QOpenGLWidget
{
public:
	GLWindow(QWidget* parent = 0);
	~GLWindow();

	
	GLShader* m_shader;
	glmesh_ptr m_mesh;

	GLUBO ubo;
	void updateUniformBuffer(int w, int h);

	//test
	Scene* scene;
	void setScene(Scene* s) { scene = s; };

	void initializeGL();
	void resizeGL(int w, int g);
	void paintGL();

	void mousePressEvent(QMouseEvent*);
};

