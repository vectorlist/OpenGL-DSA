#include <VKWindow.h>
#include <qlayout.h>
#include <qlabel.h>
#include <vkrenderer.h>
#include <vktools.h>
#include <qdebug.h>
#include <QMouseEvent>
#include <camera.h>
#include <scene.h>
#include <texturerenderer.h>

VKWindow::VKWindow(MainWindow* parent) : m_mainWindow(parent)
{
	m_renderer = new TextureRenderer(this);
	connect(&m_timer, &QTimer::timeout, this, &VKWindow::frameUpdate);
}

VKWindow::~VKWindow()
{
	SAFE_DELETE(m_renderer);
}

void VKWindow::resizeEvent(QResizeEvent* e)
{
	QWindow::resizeEvent(e);
	if (!m_renderer->isBuilt)
	{
		m_renderer->buildProcedural();
		m_timer.start(100);
	}
	else
	{
		m_renderer->resize();
	}
	m_renderer->updateUniformBuffers();
	m_renderer->render();
}

void VKWindow::mousePressEvent(QMouseEvent *e)
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

void VKWindow::mouseMoveEvent(QMouseEvent*e)
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

void VKWindow::wheelEvent(QWheelEvent *e)
{
	if (!m_renderer->isBuilt) return;
	m_renderer->m_scene->camera->zoom(e->delta());
	m_renderer->update();

	QWindow::wheelEvent(e);
}

void VKWindow::frameUpdate()
{
	m_renderer->update();
	m_renderer->frame++;
	//qDebug() << 1;
}