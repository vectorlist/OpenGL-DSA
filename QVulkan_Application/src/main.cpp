
#include <qapplication.h>
#include <qwindow.h>
#include <Windows.h>
#include <MainWindow.h>


//#define CHECK_LEAK
#ifdef CHECK_LEAK
#include <vld.h>
#endif //CHECK LEAK

int main(int argc, char *argv[])
{

	QApplication a(argc, argv);

	MainWindow mw;
	mw.setGeometry(810, 300, 940, 620);
	mw.show();
	
	return a.exec();
}