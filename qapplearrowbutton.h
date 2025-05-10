#ifndef QAPPLEARROWBUTTON_H
#define QAPPLEARROWBUTTON_H
#include <QToolButton>

#define CornerRadius 4

class QAppleArrowButton : public QToolButton
{
public:
    enum ArrowDirection
    {
        None,
        Left,
        Right,
        RoundLeft,
        RoundRight
    };
    ArrowDirection arrowDirection();
    void setArrowDirection(ArrowDirection ad);
    QAppleArrowButton(QWidget* parent=nullptr);
protected:
    void paintEvent(QPaintEvent * event);
private:
    ArrowDirection m_ArrowDirection;
    QImage minBrightness(QImage img, int value);
    QImage setBrightness(QImage img, int value, bool desaturate=false);
};

#endif // QAPPLEARROWBUTTON_H
