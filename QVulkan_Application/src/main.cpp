
#include <qapplication.h>
#include <qwindow.h>
#include <Windows.h>
#include <MainWindow.h>
#include <vktexture.h>

//#define CHECK_LEAK
#ifdef CHECK_LEAK
#include <vld.h>
#endif //CHECK LEAK

int main(int argc, char *argv[])
{
	/*setConsole(10, 10, 780, 1000);*/
	QApplication a(argc, argv);
	////setConsole(10, 10, 780, 1000);
	MainWindow mw;
	//mw.resize(680, 420);
	mw.setGeometry(810, 300, 940, 620);
	mw.show();

	//system("pause");
	return a.exec();
}
