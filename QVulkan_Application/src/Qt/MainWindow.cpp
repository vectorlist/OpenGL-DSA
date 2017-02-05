#include "MainWindow.h"

#pragma region INCLUDES

#include <qheaderview.h>
#include <qapplication.h>
#include <qstylefactory.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qstring.h>
#include <qtextstream.h>
#include <vkwindow.h>
#include <glwindow.h>
#include <windowframe.h>
#include <vkrenderer.h>
#include <scene.h>

#pragma endregion

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent), m_glwindow(NULL), m_vkWindow(NULL)
{
	setConsoleGeometry(10, 10, 780, 1000,false);
	setStyleFromfile("./qvulkan.css");

	setWindowFlags(Qt::FramelessWindowHint);

	setTopFrames();
	setMidFrames();
	
}


MainWindow::~MainWindow()
{
	SAFE_DELETE(m_glwindow);
	SAFE_DELETE(m_vkWindow);
}

void MainWindow::setTopFrames()
{
	m_center = new QWidget;
	setCentralWidget(m_center);

	m_topLayout = new QVBoxLayout(m_center);
	m_center->setLayout(m_topLayout);
	m_topLayout->setMargin(0);
	m_topLayout->setSpacing(0);

	m_frameless = new WindowFrame();
	m_topLayout->addWidget(m_frameless);

	m_mid = new QWidget();
	m_topLayout->addWidget(m_mid);

}

void MainWindow::setMidFrames()
{
	m_midLayout = new QHBoxLayout(m_mid);

	//m_glwindow = new GLWindow();
	//m_midLayout->addWidget(m_glwindow);

	m_vkWindow = new VKWindow(this);
	m_VKWindowContainer = QWidget::createWindowContainer(m_vkWindow);
	m_midLayout->addWidget(m_VKWindowContainer);


	//m_infoWidget = new InfoWidget(this);
	//m_layout->addWidget(m_infoWidget);
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