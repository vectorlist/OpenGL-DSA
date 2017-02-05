#include "windowframe.h"
#include <QPainter>
#include <QBitmap>
#include <QApplication>
#include <QMouseEvent>
#include <qdebug.h>

WindowFrame::WindowFrame(QWidget *parent)
	: QWidget(parent),
	frameCache(0), // NULL
	frameName(qApp->applicationName(), this),
	onMove(false),
	m_titleIcon(NULL)
{
	setFixedHeight(FRAME_SIZE);
	setAttribute(Qt::WA_TranslucentBackground);

	minButton = new FrameButton(ButtonType::MIN, this);
	maxButton = new FrameButton(ButtonType::MAX , this);
	closeButton = new FrameButton(ButtonType::CLOSE, this);
	setFrameIcon("./icon/logo/qtlogo_white.png");
	setConnect();

	QPalette textPalette;
	textPalette.setBrush(QPalette::WindowText, QBrush(QColor(255, 255, 255)));

	frameName.setPalette(textPalette);

}

WindowFrame::~WindowFrame()
{
	delete m_titleIcon;
	delete frameCache;
}

void WindowFrame::setConnect()
{
	connect(minButton, SIGNAL(clicked()), this, SLOT(minimized()));
	connect(maxButton, SIGNAL(clicked()), this, SLOT(maximized()));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(quit()));
}

void WindowFrame::setFrameIcon(const QString &filename)
{
	m_titleIcon = new QPixmap(filename);
	//frameImage = QImage(filename);
}

void WindowFrame::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);        //ignore event
	QPainter painter(this);
	painter.drawPixmap(0, 0, *frameCache);
}

void WindowFrame::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event);        //ignore event

	if (frameCache != NULL)
		frameCache = 0;

	frameCache = new QPixmap(size());
	frameCache->fill(Qt::transparent);

	QPainter painter(frameCache);
	if(m_titleIcon)
		painter.drawPixmap(0, (height() - m_titleIcon->height()) / 2, *m_titleIcon);
	
	frameName.setFixedHeight(FRAME_SIZE);
	frameName.move(m_titleIcon->width() + 16, (height() - frameName.height()) / 2);

	minButton->move(width() - (minButton->width() * 3), (height() - minButton->height()) / 2);

	maxButton->move(width() - (maxButton->width() * 2), (height() - maxButton->height()) / 2);

	closeButton->move(width() - closeButton->width(), (height() - closeButton->height()) / 2);
}

void WindowFrame::minimized()
{
	if (window()->windowState() == Qt::WindowState::WindowMinimized)
	{
		window()->showNormal();
		window()->update();
	}
	else
		window()->showMinimized();
	
}

void WindowFrame::maximized()
{
	if (window()->windowState() == Qt::WindowMaximized)
	{
		window()->showNormal();
	}
	else
	{
		window()->showMaximized();
	}
}

void WindowFrame::quit()
{
	qApp->quit();
}



void WindowFrame::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();
	onMove = true;
}

void WindowFrame::mouseMoveEvent(QMouseEvent *event)
{

	if (onMove)
	{
		QPoint coord = event->globalPos() - lastPos;
		window()->move(coord.x(), coord.y());
	}
}

void WindowFrame::mouseReleaseEvent(QMouseEvent *event)
{
	onMove = false;
	QWidget::mouseReleaseEvent(event);
}
void WindowFrame::mouseDoubleClickEvent(QMouseEvent *event)
{
	maximized();
	QWidget::mouseDoubleClickEvent(event);
}