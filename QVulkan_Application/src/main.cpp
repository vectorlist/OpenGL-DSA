
#include <qapplication.h>
#include <qwindow.h>
#include <Windows.h>
#include <MainWindow.h>
#include <windowframe.h>
#include <qdebug.h>
//test
#include <rgb.h>
#include <Mipmap.h>
#include <qlabel.h>
#include <mathutil.h>

//#define CHECK_LEAK
#ifdef CHECK_LEAK
#include <vld.h>
#endif //CHECK LEAK

int main(int argc, char *argv[])
{

	QApplication a(argc, argv);

	MainWindow mw;
	mw.setGeometry(810, 300, 1024, 620);
	mw.show();
	
	//QImage image("./image/checker.jpg");
	//QImage img = image.convertToFormat(QImage::Format_RGBA8888);

	////test mip level test
	//Mipmap mip(img.width(), img.height(), img.constBits(),5);


	//MapBuffer* buffer = mip.mapBuffer(2);
	//QImage img2((uchar*)buffer->m_pixels.get(),
	//	buffer->width,
	//	buffer->height, QImage::Format_ARGB32);

	//QLabel widget;
	//widget.setPixmap(QPixmap::fromImage(img2));
	//widget.show();
	//
	//qDebug() << math::isPowOf2(648);
	
	return a.exec();
}