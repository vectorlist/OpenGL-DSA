#include <renderwindow.h>
#include <qlayout.h>
#include <qlabel.h>
#include <vkrenderer.h>
#include <vktools.h>
#include <basicrenderer.h>

RenderWindow::RenderWindow()
{
	/*m_renderer = new VkRenderer(this);
	m_renderer->initialize();*/
	m_renderer = new BasicRenderer(this);
	m_renderer->initialize();
}

RenderWindow::~RenderWindow()
{
	SAFE_DELETE(m_renderer);
}

void RenderWindow::resizeEvent(QResizeEvent* e)
{
	QWindow::resizeEvent(e);
	//for setSurface by Qwindow

	if (!m_renderer->isBuilt)
		m_renderer->buildProcedural();
	else
		m_renderer->resized();
	/*m_renderer->resized();*/
	m_renderer->updateUniformBuffers();
	m_renderer->render();
}