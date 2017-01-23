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
	setConsoleNative(10, 10, 780, 1000);
	//qApp->setStyle(QStyleFactory::create("Fusion"));
	
	setStyleFromfile("./qvulkan.css");

	resize(700, 500);

	m_center = new QWidget;
	m_layout = new QHBoxLayout;
	setCentralWidget(m_center);
	m_center->setLayout(m_layout);
	
	//m_label = new QLabel("QVulkan_Application");
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


void MainWindow::setConsoleNative(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	/*AllocConsole();
	FILE *pFileCon = NULL;
	pFileCon = freopen("CONOUT$", "w", stdout);

	COORD coordInfo;
	coordInfo.X = 130;
	coordInfo.Y = 9000;

	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coordInfo);
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_QUICK_EDIT_MODE
	| ENABLE_EXTENDED_FLAGS);*/

	//fix later
	/*AllocConsole();
	AttachConsole(GetCurrentProcessId());
	FILE *stream;
	freopen_s(&stream, "COUNT$", "w+", stdout);
	SetConsoleTitle(TEXT("debugger window"));*/

	HWND console = GetConsoleWindow();
	MoveWindow(console, x, y, width, height, TRUE);
}