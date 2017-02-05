#ifndef QMAINFRAME_H
#define QMAINFRAME_H

#include <QWidget>
#include <qicon.h>
#include <QPixmap>
#include <QLabel>
#include <QPaintEvent>
#include <FrameButton.h>

#define FRAME_SIZE 31

class WindowFrame : public QWidget
{
	Q_OBJECT
public:
	WindowFrame(QWidget *parent = 0);
	~WindowFrame();

	QIcon  frameIcon;
	QLabel frameName;
	QBrush frameBrush;
	QPixmap *frameCache;
	QPixmap *m_titleIcon;
	//QImage frameImage;

	FrameButton *minButton;
	FrameButton *maxButton;
	FrameButton *closeButton;

	void setConnect();
	void setFrameIcon(const QString &filename);

	public slots:
	void minimized();
	void maximized();
	void quit();

protected:
	void paintEvent(QPaintEvent  *event);
	void resizeEvent(QResizeEvent *event);

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);


private:
	QPoint lastPos;
	bool onMove;

};

#endif // QMAINFRAME_H
