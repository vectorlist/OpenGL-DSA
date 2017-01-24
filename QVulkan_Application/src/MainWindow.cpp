#include "MainWindow.h"

#pragma region INCLUDES

#include <qheaderview.h>
#include <qapplication.h>
#include <qstylefactory.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qstring.h>
#include <qtextstream.h>
#include <renderwindow.h>
#include <glwindow.h>

#pragma endregion

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	setConsoleGeometry(10, 10, 780, 1000,false);
	setStyleFromfile("./qvulkan.css");

	m_center = new QWidget;
	m_layout = new QHBoxLayout;
	setCentralWidget(m_center);
	m_center->setLayout(m_layout);

	m_glwindow = new GLWindow();
	
	//m_layout->addWidget(m_glwindow);

	m_renderWindow = new RenderWindow;
	m_renderWindowContainer = QWidget::createWindowContainer(m_renderWindow);
	m_layout->addWidget(m_renderWindowContainer);

}


MainWindow::~MainWindow()
{
	SAFE_DELETE(m_glwindow);
	SAFE_DELETE(m_renderWindow);
}

void MainWindow::setStyleFromfile(const string &filename)
{
	QFile file(filename.c_str());

	if (file.open(QIODevice::ReadOnly))
	{
		QTextStream stream(&file);
		QString styleCode = stream.readAll();
		file.close();
		setStyleSheet(styleCode);
	}
}


void MainWindow::setConsoleGeometry(
	uint32_t x, uint32_t y,
	uint32_t width, uint32_t height, bool isTop)
{
	
	HWND console = GetConsoleWindow();
	if (!console) return;
	if(!isTop)
		MoveWindow(console, x, y, width, height, TRUE);
	else
		SetWindowPos(console, HWND_TOPMOST,x, y, width, height,SWP_SHOWWINDOW);

}