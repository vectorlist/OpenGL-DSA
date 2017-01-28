#pragma once

#include <qwindow.h>
#include <vec2f.h>

class Renderer;
class VkRenderer;
class Camera;
class RenderWindow : public QWindow
{
public:
	RenderWindow();
	virtual~RenderWindow();

	VkRenderer* m_renderer;
	vec2i m_mousePos;

	void resizeEvent(QResizeEvent*);
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent *);

	bool isPrepared = false;
};