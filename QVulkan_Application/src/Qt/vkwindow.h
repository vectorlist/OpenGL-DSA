#pragma once

#include <qwindow.h>
#include <vec2f.h>
#include <MainWindow.h>
#include <qtimer.h>
#include <qobject.h>

class Renderer;
class VkRenderer;
class Camera;
class VKWindow : public QWindow
{
	Q_OBJECT
public:
	VKWindow(MainWindow* parent = 0);
	virtual~VKWindow();

	VkRenderer* m_renderer;
	vec2i m_mousePos;

	MainWindow* m_mainWindow;

	QTimer m_timer;
	//update when Scene build

	void resizeEvent(QResizeEvent*);
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent *);

	bool isPrepared = false;

public	slots:
	void frameUpdate();

	
};