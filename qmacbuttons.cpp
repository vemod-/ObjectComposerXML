#include "qmacbuttons.h"
#include "ui_qmacbuttons.h"
#include <QGraphicsDropShadowEffect>
#include <QBitmap>
#include <QLinearGradient>

QCustomToolButton::QCustomToolButton(QWidget *parent) : QToolButton(parent)
{
    monochrome=true;
}

QRect QCustomToolButton::findRect()
{
    QRect p=parentWidget()->geometry();
    QRect t=this->geometry();
    QRect r=this->rect();
    r.setHeight(r.height()-2);
    if (p.width()==t.width())
    {
        r.setWidth(r.width()-1);
        return r;
    }
    if (p.left()==t.left())
    {
        r.setWidth(r.width()+4);
        return r;
    }
    if (p.right()==t.right())
    {
        r.setWidth(r.width()+CornerRadius-2);
        r.translate(-CornerRadius,0);
        return r;
    }
    r.setWidth(r.width()+(CornerRadius*2)-1);
    r.translate(-CornerRadius,0);
    return r;
}

QImage QCustomToolButton::setBrightness(QImage img, int value, bool desaturate)
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

QImage QCustomToolButton::minBrightness(QImage img, int value)
{
    float factor=(255-value)/255.0;
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
            v=(v*factor)+value;
            color.setHsv(h, s, v, a);
            img.setPixel(x,y, color.rgba());
        }
    }
    return img;
}

void QCustomToolButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QRect roundRect=findRect();
    QImage px;
    //QPixmap mask;
    QRect iconRect=rect();
    QRect textRect=rect();
    textRect.translate(0,-1);
    QRect r=textRect;
    r.translate(0,1);
    QPainter p(this);
    p.setWorldMatrixEnabled(false);
    p.setViewTransformEnabled(false);
    p.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    //p.setRenderHints(QPainter::Antialiasing);
    QTextOption o(Qt::AlignCenter);
    p.setFont(font());
    //QPen pen=p.pen();
    QPen dpen("#555555");
    QRect shadowRect=roundRect;
    shadowRect.translate(1,1);
    p.setPen("#cccccc");
    p.setBrush(Qt::transparent);
    p.drawRoundedRect(shadowRect,CornerRadius,CornerRadius);
    if (!icon().isNull())
    {
        //iconRect.setWidth(iconRect.width()-2);
        //iconRect.setHeight(iconRect.height()-2);
        iconRect.adjust(0,2,-3,-3);
        QSize s(24,24);
        if (icon().availableSizes().count()) s = icon().availableSizes()[0];
        float aspect=(float)s.width()/(float)s.height();
        float thisaspect=(float)iconRect.width()/(float)iconRect.height();
        if (aspect<thisaspect)
        {
            iconRect.setWidth((float)iconRect.height()*aspect);
            //iconRect.setLeft((float)(rect().width()-iconRect.width())/2.0);
            iconRect.translate((rect().width()-iconRect.width())/2.0,0);
        }
        if (aspect>thisaspect)
        {
            iconRect.setHeight((float)iconRect.width()*aspect);
            iconRect.translate(0,(rect().height()-iconRect.height())/2.0);
        }
        //iconRect.translate(1,1);
        //iconRect.adjust(2,2,-2,-2);
        r=iconRect;
        r.translate(0,1);
        px = icon().pixmap(s).toImage().convertToFormat(QImage::Format_ARGB32);
        //mask = QPixmap(icon().pixmap(s).createHeuristicMask());
    }
    if (this->isEnabled())
    {
        if (isChecked() | isDown())
        {
            QLinearGradient lg(0,0,0,height());
            lg.setColorAt(0,"#333333");
            lg.setColorAt(0.4,"#666666");
            p.setBrush(QBrush(lg));
            p.setPen(dpen);
            p.drawRoundedRect(roundRect,CornerRadius,CornerRadius);
            lg=QLinearGradient(0,0,width(),0);
            lg.setColorAt(0,"#555555");
            lg.setColorAt(0.15,Qt::transparent);
            lg.setColorAt(0.85,Qt::transparent);
            lg.setColorAt(1,"#555555");
            p.setBrush(QBrush(lg));
            p.drawRoundedRect(roundRect,CornerRadius,CornerRadius);
            if (!icon().isNull())
            {
                //p.setPen("#333333");
                //p.setPen(pen);
                //px=setBrightness(px,-1);
                if (monochrome)
                {
                    p.drawImage(r,setBrightness(px,20,true));
                    p.drawImage(iconRect,setBrightness(px,230,true));
                }
                else
                {
                    p.drawImage(r, setBrightness(px,20));
                    p.drawImage(iconRect,px);
                }
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
            QLinearGradient lg(0,0,0,height());
            //lg.setColorAt(0,Qt::white);
            //lg.setColorAt(0.8,"#aaaaaa");
            lg.setColorAt(0,Qt::white);
            lg.setColorAt(0.39999,"#eee");
            lg.setColorAt(0.4,"#dfdfdf");
            lg.setColorAt(1,"#c8c8c8");
            p.setBrush(QBrush(lg));
            p.setPen(dpen);
            p.drawRoundedRect(roundRect,CornerRadius,CornerRadius);
            if (!icon().isNull())
            {
                if (monochrome)
                {
                    p.drawImage(r, setBrightness(px,250,true));
                    p.drawImage(iconRect,setBrightness(px,40,true));
                }
                else
                {
                    p.drawImage(r, setBrightness(px,250));
                    p.drawImage(iconRect,px);
                }
            }
            else
            {
                p.setPen("#eeeeee");
                p.drawText(r,text(),o);
                p.setPen(Qt::black);
                p.drawText(textRect,text(),o);
            }
        }
    }
    else
    {
        if (!isChecked())
        {
            QLinearGradient lg(0,0,0,height());
            lg.setColorAt(0,"#ddd");
            lg.setColorAt(0.39999,"#ccc");
            lg.setColorAt(0.4,"#bfbfbf");
            lg.setColorAt(1,"#bbb");

            //lg.setColorAt(0,"#dddddd");
            //lg.setColorAt(0.8,Qt::gray);
            p.setBrush(QBrush(lg));
            p.setPen(dpen);
        }
        else
        {
            QLinearGradient lg(0,0,0,height());
            lg.setColorAt(0,"#666666");
            lg.setColorAt(0.4,"#888888");
            p.setBrush(QBrush(lg));
            p.setPen(dpen);
            p.drawRoundedRect(roundRect,CornerRadius,CornerRadius);
            lg=QLinearGradient(0,0,width(),0);
            lg.setColorAt(0,"#777777");
            lg.setColorAt(0.15,Qt::transparent);
            lg.setColorAt(0.85,Qt::transparent);
            lg.setColorAt(1,"#777777");
            p.setBrush(QBrush(lg));
        }
        p.drawRoundedRect(roundRect,CornerRadius,CornerRadius);
        if (!icon().isNull())
        {
            if (monochrome)
            {
                p.drawImage(r, setBrightness(px,220,true));
                p.drawImage(iconRect,setBrightness(px,100,true));
            }
            else
            {
                p.drawImage(r, setBrightness(px,220,true));
                p.drawImage(iconRect,minBrightness(px,100));
            }
        }
        else
        {
            p.setPen("#dddddd");
            p.drawText(r,text(),o);
            p.setPen("#666666");
            p.drawText(textRect,text(),o);
        }
    }
    if (roundRect.right()>width())
    {
        p.setPen(dpen);
        p.drawLine(width()-1,0,width()-1,roundRect.height());
    }
}

QMacButtons::QMacButtons(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QMacButtons)
{
    ui->setupUi(this);
    QGridLayout* l=new QGridLayout(this);
    l->setMargin(0);
    l->setVerticalSpacing(0);
    l->setHorizontalSpacing(0);
    frame=new QWidget(this);
    l->addWidget(frame,0,0);
    layout=new QGridLayout(frame);
    layout->setMargin(0);
    layout->setVerticalSpacing(0);
    layout->setHorizontalSpacing(0);
    setSelectMode(QMacButtons::SelectNone);
    mapper=new QSignalMapper(this);
    connect(mapper,SIGNAL(mapped(int)),this,SLOT(wasClicked(int)));
}

QMacButtons::~QMacButtons()
{
    qDeleteAll(Buttons);
    Buttons.clear();
    Names.clear();
    delete layout;
    delete frame;
    delete ui;
}

void QMacButtons::addButton(const QString& Name, const QString& Tooltip, const QIcon& Icon)
{
    QToolButton* b=new QCustomToolButton(this);
    b->setToolTip(Tooltip);
    b->setIcon(Icon);
    addButton(b,Name);
}

void QMacButtons::addButton(const QString& Name, const QString& Tooltip, const QString& Text, const QFont& Font)
{
    QToolButton* b=new QCustomToolButton(this);
    b->setToolTip(Tooltip);
    b->setText(Text);
    b->setFont(Font);
    addButton(b,Name);
}

void QMacButtons::addButton(QAction *action, QKeySequence keySequence)
{
    QToolButton* b=new QCustomToolButton(this);
    b->setIcon(action->icon());
    b->setToolTip(action->text()+" "+keySequence.toString());
    QWidget::addAction(action);
    action->setShortcut(keySequence);
    connect(action,SIGNAL(triggered()),b,SLOT(animateClick()));
    addButton(b,action->text());
}

void QMacButtons::addButton(QToolButton *b, const QString& Name)
{
    b->setCheckable(m_selectMode != QMacButtons::SelectNone);
    b->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    layout->addWidget(b,0,Buttons.count());
    connect(b,SIGNAL(pressed()),this,SLOT(wasClicked()));
    connect(b,SIGNAL(clicked()),mapper,SLOT(map()));
    mapper->setMapping(b,Buttons.count());
    Buttons.append(b);
    Names.append(Name);
    //ShowButtons();
}

void QMacButtons::wasClicked()
{
    for (int i=0;i<Buttons.count();i++)
    {
        if (Buttons[i]->isDown())
        {
            //emit buttonClicked(i);
            //emit buttonClicked(Names[i]);
            if (m_selectMode != QMacButtons::SelectNone)
            {
                if (!Buttons[i]->isChecked())
                {
                    emit selected(i);
                    emit selected(Names[i]);
                }
            }
        }
        else
        {
            if (m_selectMode==QMacButtons::SelectOneOrNone)
            {
                Buttons[i]->setChecked(false);
            }
        }
        if (m_selectMode==QMacButtons::SelectOne)
        {
            Buttons[i]->setChecked(false);
        }
    }
}

void QMacButtons::wasClicked(int index)
{
    emit buttonClicked(index);
    emit buttonClicked(Names[index]);
}

const QMacButtons::SelectMode QMacButtons::selectMode()
{
    return m_selectMode;
}

void QMacButtons::setSelectMode(const SelectMode sm)
{
    m_selectMode=sm;
    foreach (QToolButton* b,Buttons)
    {
        b->setCheckable(sm != QMacButtons::SelectNone);
    }
}

void QMacButtons::setSelected(const int index, const bool selected)
{
    if (m_selectMode==QMacButtons::SelectAll)
    {
        if (selected)
        {
            emit this->selected(index);
            emit this->selected(Names[index]);
        }
        Buttons[index]->setChecked(selected);
        return;
    }
    if (m_selectMode==QMacButtons::SelectNone)
    {
        foreach (QToolButton* b,Buttons)
        {
            b->setChecked(false);
        }
        return;
    }
    if (selected)
    {
        foreach (QToolButton* b,Buttons)
        {
            b->setChecked(false);
        }
        emit this->selected(index);
        emit this->selected(Names[index]);
        Buttons[index]->setChecked(true);
    }
    else
    {
        Buttons[index]->setChecked(false);
    }
}

void QMacButtons::setSelected(const QString& name, const bool selected)
{
    setSelected(Names.indexOf(name),selected);
}

const bool QMacButtons::isSelected(const int index)
{
    return Buttons[index]->isChecked();
}

const bool QMacButtons::isSelected(const QString& name)
{
    return isSelected(Names.indexOf(name));
}

const bool QMacButtons::isSelected()
{
    foreach (QToolButton* b, Buttons)
    {
        if (b->isChecked()) return true;
    }
    return false;
}

void QMacButtons::setSelected(const bool selected)
{
    if (selected)
    {
        if (m_selectMode==QMacButtons::SelectAll)
        {
            for (int i=0;i<Buttons.count();i++)
            {
                emit this->selected(i);
                emit this->selected(Names[i]);
                Buttons[i]->setChecked(true);
            }
        }
    }
    else
    {
        if ((m_selectMode==QMacButtons::SelectNone) || (m_selectMode==QMacButtons::SelectOneOrNone))
        {
            foreach (QToolButton* b,Buttons)
            {
                b->setChecked(false);
            }
        }
    }
}

void QMacButtons::setDown(const int index, const bool down)
{
    Buttons[index]->setDown(down);
}

void QMacButtons::setDown(const QString& name, const bool down)
{
    setDown(Names.indexOf(name),down);
}

void QMacButtons::setDown(const bool down)
{
    foreach (QToolButton* b,Buttons)
    {
        b->setDown(down);
    }
}

void QMacButtons::setTooltip(const int index, const QString& tooltip)
{
    Buttons[index]->setToolTip(tooltip);
}

void QMacButtons::setTooltip(const QString& name, const QString& tooltip)
{
    setTooltip(Names.indexOf(name),tooltip);
}

const int QMacButtons::value()
{
    for (int i=0;i<Buttons.count();i++)
    {
        if (Buttons[i]->isChecked()) return i;
    }
    return -1;
}

void QMacButtons::setEnabled(const int index, const bool enabled)
{
    Buttons[index]->setEnabled(enabled);
}

void QMacButtons::setEnabled(const QString& name, const bool enabled)
{
    setEnabled(Names.indexOf(name),enabled);
}

void QMacButtons::setEnabled(const bool enabled)
{
    QWidget::setEnabled(enabled);
}

const bool QMacButtons::isEnabled(const int index)
{
    return Buttons[index]->isEnabled();
}

const bool QMacButtons::isEnabled(const QString& name)
{
    return isEnabled(Names.indexOf(name));
}

const bool QMacButtons::isEnabled()
{
    return QWidget::isEnabled();
}

void QMacButtons::setMonochrome(const int index, const bool monochrome)
{
    ((QCustomToolButton*)Buttons[index])->monochrome=monochrome;
}

void QMacButtons::setMonochrome(const QString& name, const bool monochrome)
{
    setMonochrome(Names.indexOf(name),monochrome);
}

void QMacButtons::setMonochrome(const bool monochrome)
{
    foreach (QToolButton* b,Buttons)
    {
        ((QCustomToolButton*)b)->monochrome=monochrome;
    }
}

const bool QMacButtons::isMonochrome(const int index)
{
    return ((QCustomToolButton*)Buttons[index])->monochrome;
}

const bool QMacButtons::isMonochrome(const QString& name)
{
    return isMonochrome(Names.indexOf(name));
}

const bool QMacButtons::isMonochrome()
{
    foreach (QToolButton* b,Buttons)
    {
        if (!((QCustomToolButton*)b)->monochrome) return false;
    }
    return true;
}

void QMacButtons::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if (Buttons.count()==0) return;
    float w=frame->width()/Buttons.count();
    foreach (QToolButton* b,Buttons)
    {
        b->setIconSize(QSize(w-4,frame->height()-4));
    }
}
