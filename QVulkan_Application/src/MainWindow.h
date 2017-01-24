#pragma once

#include <qmainwindow.h>
#include <vkTools.h>
#include <qlayout.h>
#include <qtreewidget.h>
#include <qstring.h>
#include <qlabel.h>

using std::string;
using std::vector;

class RenderWindow;
class GLWindow;
class MainWindow : public QMainWindow
{
public:
	MainWindow(QWidget* parent = NULL);
	~MainWindow();

	QHBoxLayout* m_layout;
	QWidget* m_widget;
	QWidget* m_center;
	QLabel* m_label;
	QWidget* m_renderWindowContainer;
	GLWindow* m_glwindow;
	RenderWindow* m_renderWindow;

	vector<int> m_c;
	void setStyleFromfile(const string &filename);
	void setConsoleGeometry(
		uint32_t x, uint32_t y,
		uint32_t width, uint32_t height, bool isTop = false);
};
