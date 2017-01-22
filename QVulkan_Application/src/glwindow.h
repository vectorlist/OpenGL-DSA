#pragma once

#include <glad/glad.h>
#include <qopenglwidget.h>

class GLShader;
class GLWindow : public QOpenGLWidget
{
public:
	GLWindow(QWidget* parent = 0);
	~GLWindow();

	
	GLShader* m_shader;

	void initializeGL();
	void resizeGL(int w, int g);
	void paintGL();
};

