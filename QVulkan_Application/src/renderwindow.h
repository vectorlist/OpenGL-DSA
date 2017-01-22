#pragma once

#include <qwindow.h>

class Renderer;
class VkRenderer;
class RenderWindow : public QWindow
{
public:
	RenderWindow();
	virtual~RenderWindow();

	VkRenderer* m_renderer;

	void resizeEvent(QResizeEvent*);

	bool isPrepared = false;
};