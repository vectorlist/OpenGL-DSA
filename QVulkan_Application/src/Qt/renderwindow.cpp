#include <renderwindow.h>
#include <qlayout.h>
#include <qlabel.h>
#include <vkrenderer.h>
#include <vktools.h>
#include <basicrenderer.h>
#include <qdebug.h>
#include <QMouseEvent>
#include <camera.h>
#include <scene.h>
#include <texturerenderer.h>

RenderWindow::RenderWindow()
{
	/*m_renderer = new VkRenderer(this);
	m_renderer->initialize();*/
	//m_renderer = new BasicRenderer(this);
	//m_renderer->initialize();
	m_renderer = new TextureRenderer(this);
}

RenderWindow::~RenderWindow()
{
	SAFE_DELETE(m_renderer);
}

void RenderWindow::resizeEvent(QResizeEvent* e)
{
	QWindow::resizeEvent(e);
	if (!m_renderer->isBuilt)
		m_renderer->buildProcedural();
	else
		m_renderer->resize();
	
	m_renderer->updateUniformBuffers();
	m_renderer->render();
}

void RenderWindow::mousePressEvent(QMouseEvent *e)
{
	if (e->buttons() == Qt::LeftButton)
	{
		m_mousePos.x = e->pos().x();
		m_mousePos.y = e->pos().y();
	}
	if (e->buttons() == Qt::RightButton)
	{
		m_renderer->updateShader();
	}
	QWindow::mousePressEvent(e);
}

void RenderWindow::mouseMoveEvent(QMouseEvent*e)
{
	if (e->buttons() == Qt::LeftButton)
	{
		int posx = e->pos().x();
		int posy = e->pos().y();
		Camera* camera = m_renderer->m_scene->camera.get();
		vec3f rot = camera->m_rotation;
		rot.x += (m_mousePos.y - (float)posy) * 0.86f;
		rot.y -= (m_mousePos.x - (float)posx) * 0.86f;
		camera->rotate(rot);
		
		m_mousePos = { posx, posy };
		m_renderer->update();
	}
	QWindow::mouseMoveEvent(e);
}

void RenderWindow::wheelEvent(QWheelEvent *e)
{
	if (!m_renderer->isBuilt) return;
	m_renderer->m_scene->camera->zoom(e->delta());
	m_renderer->update();

	QWindow::wheelEvent(e);
}