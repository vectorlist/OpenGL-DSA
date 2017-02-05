#include "framebutton.h"
#include <qevent.h>
#include <qpalette.h>
FrameButton::FrameButton(const ButtonType &_type, QWidget *parent)
    : QAbstractButton(parent), mType(_type), onMouse(false)
{
	if (parent)
		setFixedSize(parent->height(), parent->height());
	else
		setFixedSize(BTN_DAFUALT_SIZE,BTN_DAFUALT_SIZE);

    //setStyleSheet("border:none; border-style: none;");
	QPalette pal;
	pal = parent->palette();
	QBrush g = pal.background();
	
    mColor = QColor(220,220,220);
    bColor = QColor(45,45,48);
    onColor = QColor(65,65,68);
    blankSize = 12;
}

//void FrameButton::resizeButton(int size)
//{
//    resize(size, size);
//    blankSize = (12 - ((31 - size))/2);
//}

void FrameButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    //painter.setRenderHints(QPainter::Antialiasing);
    if (onMouse)
        painter.setBrush(QBrush(onColor));
    else
        painter.setBrush(QBrush(bColor));
    painter.setPen(QPen(Qt::NoPen));
    painter.drawRect(0, 0, width(), height());

    switch(mType)
    {
    case ButtonType::MIN :
        drawMin(painter);
        break;
    case ButtonType::MAX :
        drawMax(painter);
        break;
    case ButtonType::CLOSE :
		painter.setRenderHints(QPainter::Antialiasing);
        drawClose(painter);
        break;
    }
}


void FrameButton::drawMin(QPainter &painter)
{
    QPolygon minPolygon;
    minPolygon

        << QPoint(blankSize - 2, height() - blankSize - 1)
        << QPoint(width() - blankSize + 1, height() - blankSize - 1);

    painter.setPen(QPen(mColor, 2.4));
    painter.drawPolygon(minPolygon);
}
void FrameButton::drawMax(QPainter &painter)
{
    if (window()->windowState() == Qt::WindowMaximized)
    {
        QPolygon maxPolygon, subPolygon;
        maxPolygon << QPoint(blankSize, blankSize + 1)
            << QPoint(blankSize + 1, height() - blankSize)
            << QPoint(width() - blankSize, height() - blankSize)
            << QPoint(width() - blankSize, blankSize + 1);

        subPolygon = maxPolygon;
        maxPolygon.translate(2, -3);
        painter.setPen(QPen(mColor, 1.0));
        painter.drawPolygon(maxPolygon);
        painter.drawPolygon(subPolygon);
    }
    else
    {
        QPolygon maxPolygon, subPolygon;
        maxPolygon << QPoint(blankSize - 1, blankSize - 1)
            << QPoint(blankSize - 1, height() - blankSize)
            << QPoint(width() - blankSize, height() - blankSize)
            << QPoint(width() - blankSize, blankSize - 1);

        subPolygon << QPoint(blankSize - 1, blankSize)
            << QPoint(width() - blankSize, blankSize);
        painter.setPen(QPen(mColor, 1.0));
        painter.drawPolygon(maxPolygon);

        painter.setPen(QPen(mColor, 3.0));
        painter.drawPolygon(subPolygon);
    }

}
void FrameButton::drawClose(QPainter &painter)
{
    QLine line1(QPoint(blankSize - 1, blankSize - 1), QPoint(width() - blankSize, height() - blankSize));
    QLine line2(QPoint(width() - blankSize, blankSize - 1), QPoint(blankSize - 1, height() - blankSize));

    painter.setPen(QPen(mColor, 1.4));
    painter.drawLine(line1);
    painter.drawLine(line2);
}


void FrameButton::mousePressEvent(QMouseEvent *event)
{
    QAbstractButton::mousePressEvent(event);
}
void FrameButton::mouseMoveEvent(QMouseEvent *event)
{
    QAbstractButton::mouseMoveEvent(event);
}

void FrameButton::enterEvent(QEvent *event)
{
    onMouse = true;
	QAbstractButton::enterEvent(event);
}
void FrameButton::leaveEvent(QEvent *event)
{
    onMouse = false;
	QAbstractButton::leaveEvent(event);
}
