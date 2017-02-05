#pragma once

#include <qmainwindow.h>
#include <vkTools.h>
#include <qlayout.h>
#include <qtreewidget.h>
#include <qstring.h>
#include <qlabel.h>
#include <qtreewidget.h>

using std::string;
using std::vector;

class VKWindow;
class GLWindow;
class WindowFrame;
class MainWindow : public QMainWindow
{
public:
	MainWindow(QWidget* parent = NULL);
	~MainWindow();

	/*TOP*/
	QVBoxLayout* m_topLayout;				//vertical
	QWidget* m_center;
	QWidget* m_mid;

	/*MID*/
	QHBoxLayout* m_midLayout;
	QWidget* m_widget;
	
	/*RENDERER*/
	GLWindow* m_glwindow;
	VKWindow* m_vkWindow;
	QWidget* m_VKWindowContainer;

	/*FRAMES*/
	WindowFrame* m_frameless;

	vector<int> m_c;

	void setTopFrames();
	void setMidFrames();
	//void setRenderers();


private:
	void setStyleFromfile(const string &filename);
	void setConsoleGeometry(
		uint32_t x, uint32_t y,
		uint32_t width, uint32_t height, bool isTop = false);
};
