#ifndef FrameButton_H
#define FrameButton_H

#define BTN_DAFUALT_SIZE        30

#include <qabstractbutton.h>
#include <qpainter.h>
#include <QPixmap>

enum class ButtonType : size_t
{
    MIN,MAX,CLOSE
};

class FrameButton : public QAbstractButton
{
    Q_OBJECT
public:

    FrameButton(const ButtonType &_type, QWidget *parent = 0);
    ~FrameButton(){}

    //void resizeButton(int sizea);

protected:
    void drawMin(QPainter &painter);
    void drawMax(QPainter &painter);
    void drawClose(QPainter &painter);

    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    ButtonType mType;
    bool onMouse;
	int drawSize;
    int blankSize;
    QColor mColor,bColor,onColor;
};

#endif // FrameButton_H
