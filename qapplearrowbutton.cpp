#include "qapplearrowbutton.h"
//#include <QGraphicsDropShadowEffect>
//#include <QBitmap>
//#include <QLinearGradient>
//#include <QTextOption>
#include <QPainter>
#include <QPainterPath>
//#include <QMatrix4x4>
//#include <QPolygon>

QAppleArrowButton::QAppleArrowButton(QWidget *parent) : QToolButton(parent)
{
    m_ArrowDirection=None;
}

void QAppleArrowButton::setArrowDirection(ArrowDirection ad)
{
    m_ArrowDirection=ad;
}

QAppleArrowButton::ArrowDirection QAppleArrowButton::arrowDirection()
{
    return m_ArrowDirection;
}

void drawArrowShape(QPainter &p,const QRectF &r,QAppleArrowButton::ArrowDirection d)
{
    double halfHeight=r.height()/2.0;
    double thirdHeight=r.height()/3.0;
    double sixthHeight=r.height()/6.0;
    double cornerDiam=r.height()/3.0;
    double arrowFactor=0.8;

    QPainterPath rightArrow(QPointF(r.right(),r.top()+halfHeight));
    rightArrow.cubicTo(QPointF(r.right()-(thirdHeight*arrowFactor),r.bottom()-sixthHeight),QPointF(r.right()-(halfHeight*arrowFactor),r.bottom()),QPointF(r.right()-(halfHeight*arrowFactor)-sixthHeight,r.bottom()));
    rightArrow.arcTo(r.left(),r.bottom()-cornerDiam,cornerDiam,cornerDiam,-90,-90);
    rightArrow.arcTo(r.left(),r.top(),cornerDiam,cornerDiam,180,-90);
    rightArrow.lineTo(QPointF(r.right()-(halfHeight*arrowFactor)-sixthHeight,r.top()));
    rightArrow.cubicTo(QPointF(r.right()-(halfHeight*arrowFactor),r.top()),QPointF(r.right()-(thirdHeight*arrowFactor),r.top()+sixthHeight),QPointF(r.right(),r.top()+halfHeight));

    QPainterPath leftRounded(r.topRight());
    leftRounded.lineTo(r.bottomRight());
    leftRounded.arcTo(r.left(),r.bottom()-cornerDiam,cornerDiam,cornerDiam,-90,-90);
    leftRounded.arcTo(r.left(),r.top(),cornerDiam,cornerDiam,180,-90);
    leftRounded.lineTo(r.topRight());

    if (d==QAppleArrowButton::Right)
    {
        p.drawPath(rightArrow);
    }
    else if (d==QAppleArrowButton::Left)
    {
        QTransform mirrorMatrix;
        mirrorMatrix.scale(-1, 1);
        QPainterPath leftArrow = (rightArrow * mirrorMatrix).translated(r.width()-1,0);
        p.drawPath(leftArrow);
        return;

    }
    else if (d==QAppleArrowButton::RoundLeft)
    {
        p.drawPath(leftRounded);
    }
    else if (d==QAppleArrowButton::RoundRight)
    {
        QTransform mirrorMatrix;
        mirrorMatrix.scale(-1, 1);
        QPainterPath rightRounded = (leftRounded * mirrorMatrix).translated(r.width()-1,0);
        p.drawPath(rightRounded);
        return;
    }
    else
    {
        p.drawRoundedRect(r,cornerDiam/2.0,cornerDiam/2.0);
    }
}

QImage QAppleArrowButton::setBrightness(QImage img, int value, bool desaturate)
{
    for (int x=0;x<img.width();x++)
    {
        for (int y=0;y<img.height();y++)
        {
            QRgb rgb=img.pixel(x,y);
            QColor color;
            color.setRgba(rgb);
            int h, s, v, a;
            color.getHsv(&h, &s, &v, &a);
            v=value;
            if (desaturate) s=0;
            color.setHsv(h, s, v, a);
            img.setPixel(x,y, color.rgba());
        }
    }
    return img;
}

QImage QAppleArrowButton::minBrightness(QImage img, int value)
{
    double factor=(255-value)/255.0;
    for (int x=0;x<img.width();x++)
    {
        for (int y=0;y<img.height();y++)
        {
            QRgb rgb=img.pixel(x,y);
            QColor color;
            color.setRgba(rgb);
            int h, s, v, a;
            color.getHsv(&h, &s, &v, &a);
            s=0;
            v=int(v*factor)+value;
            color.setHsv(h, s, v, a);
            img.setPixel(x,y, color.rgba());
        }
    }
    return img;
}

void QAppleArrowButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QRectF roundRect(rect());
    roundRect.adjust(0,0,0,-1);
    //roundRect.translate(1,1);
    QImage px;
    //QPixmap mask;
    QRectF textRect(rect());
    textRect.translate(0,-1);
    QRectF iconRect=rect();
    QRectF r(textRect);
    r.translate(0,1);
    QPainter p(this);
    p.setWorldMatrixEnabled(false);
    p.setViewTransformEnabled(false);
    p.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
    QTextOption o(Qt::AlignCenter);
    p.setFont(font());
    QPen dpen("#333");
    dpen.setJoinStyle(Qt::RoundJoin);
    QRectF shadowRect(roundRect);
    shadowRect.translate(0,1);
    QPen pPen("#666");
    pPen.setJoinStyle(Qt::RoundJoin);
    p.setPen(pPen);
    p.setBrush(Qt::transparent);
    //p.drawRoundedRect(shadowRect,CornerRadius,CornerRadius);
    //p.setRenderHint(QPainter::Antialiasing,true);
    drawArrowShape(p,shadowRect,m_ArrowDirection);
    if (!icon().isNull())
    {
        //iconRect.setWidth(iconRect.width()-2);
        //iconRect.setHeight(iconRect.height()-2);
        iconRect.adjust(0,2,-3,-3);
        QSizeF s(24,24);
        if (!icon().availableSizes().empty()) s = icon().availableSizes()[0];
        double aspect=s.width()/s.height();
        double thisaspect=iconRect.width()/iconRect.height();
        if (aspect<thisaspect)
        {
            iconRect.setWidth(iconRect.height()*aspect);
            //iconRect.setLeft((float)(rect().width()-iconRect.width())/2.0);
            iconRect.translate((rect().width()-iconRect.width())/2.0,0);
        }
        if (aspect>thisaspect)
        {
            iconRect.setHeight(iconRect.width()*aspect);
            iconRect.translate(0,(rect().height()-iconRect.height())/2.0);
        }
        //iconRect.translate(1,1);
        //iconRect.adjust(2,2,-2,-2);
        r=iconRect;
        r.translate(0,1);
        px = icon().pixmap(s.toSize()).toImage().convertToFormat(QImage::Format_ARGB32);
        //mask = QPixmap(icon().pixmap(s).createHeuristicMask());
    }
    if (this->isEnabled())
    {
        if (isDown())
        {
            QLinearGradient lg(0,0,0,height());
            /*
            lg.setColorAt(0,"#ddd");
            lg.setColorAt(0.39999,"#555");
            lg.setColorAt(0.4,"#444");
            lg.setColorAt(1,"#282828");
            */
            lg.setColorAt(0,QColor(150,150,150,150));
            lg.setColorAt(0.4,QColor(150,150,150,0));
            lg.setColorAt(0.6,QColor(0,0,0,0));
            lg.setColorAt(1,QColor(0,0,0,150));

            p.setBrush(QBrush(lg));
            p.setPen(dpen);
            //p.setRenderHint(QPainter::Antialiasing,false);
            drawArrowShape(p,roundRect,m_ArrowDirection);
            if (!icon().isNull())
            {
                //p.setPen("#333333");
                //p.setPen(pen);
                //px=setBrightness(px,-1);
                p.drawImage(r, setBrightness(px,0,true));
                p.drawImage(iconRect,setBrightness(px,250,true));
            }
            else
            {

                p.setPen(Qt::black);
                p.drawText(r,text(),o);
                p.setPen(Qt::white);
                p.drawText(textRect,text(),o);
            }
        }
        else
        {
            //qlineargradient(x1:0, y1:0, x2:0, y2:1, stop: 0 #999, stop: 0.39999 #666, stop: 0.4 #5f5f5f, stop: 1 #484848)
            QLinearGradient lg(0,0,0,height());
            /*
            lg.setColorAt(0,"#eee");
            lg.setColorAt(0.39999,"#666");
            lg.setColorAt(0.4,"#555");
            lg.setColorAt(1,"#383838");
            */
            lg.setColorAt(0,QColor(255,255,255,150));
            lg.setColorAt(0.4,QColor(255,255,255,0));
            lg.setColorAt(0.6,QColor(0,0,0,0));
            lg.setColorAt(1,QColor(0,0,0,100));

            p.setBrush(QBrush(lg));
            p.setPen(dpen);
            //p.setRenderHint(QPainter::Antialiasing,false);
            drawArrowShape(p,roundRect,m_ArrowDirection);
            if (!icon().isNull())
            {
                p.drawImage(r, setBrightness(px,0,true));
                p.drawImage(iconRect,setBrightness(px,220,true));
            }
            else
            {
                p.setPen("#000");
                p.drawText(r,text(),o);
                p.setPen("#eee");
                p.drawText(textRect,text(),o);
            }
        }
    }
    else
    {
        if (!isChecked())
        {
            QLinearGradient lg(0,0,0,height());
            /*
            lg.setColorAt(0,"#ccc");
            lg.setColorAt(0.39999,"#666");
            lg.setColorAt(0.4,"#555");
            lg.setColorAt(1,"#383838");
            */
            lg.setColorAt(0,QColor(200,200,200,150));
            lg.setColorAt(0.5,QColor(100,100,100,50));
            lg.setColorAt(1,QColor(50,50,50,100));

            p.setBrush(QBrush(lg));
            p.setPen(dpen);
        }
        else
        {
            QLinearGradient lg(0,0,0,height());
            /*
            lg.setColorAt(0,"#bbb");
            lg.setColorAt(0.39999,"#555");
            lg.setColorAt(0.4,"#444");
            lg.setColorAt(1,"#282828");
            */
            lg.setColorAt(0,QColor(150,150,150,150));
            lg.setColorAt(0.5,QColor(100,100,100,50));
            lg.setColorAt(1,QColor(50,50,50,150));

            p.setBrush(QBrush(lg));
            p.setPen(dpen);
        }
        //p.setRenderHint(QPainter::Antialiasing,false);
        drawArrowShape(p,roundRect,m_ArrowDirection);
        if (!icon().isNull())
        {
            p.drawImage(r, setBrightness(px,0,true));
            p.drawImage(iconRect,setBrightness(px,150,true));
        }
        else
        {
            p.setPen("#dddddd");
            p.drawText(r,text(),o);
            p.setPen("#666666");
            p.drawText(textRect,text(),o);
        }
    }
    if (roundRect.left()<0)
    {
        p.setPen(dpen);
        p.drawLine(0,0,0,int(roundRect.height()));
    }
}
