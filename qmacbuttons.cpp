#include "qmacbuttons.h"
#include "ui_qmacbuttons.h"
//#include <QGraphicsDropShadowEffect>
//#include <QBitmap>
//#include <QLinearGradient>
#include <QAction>
#include <QPainter>

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

void QCustomToolButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QRect roundRect=findRect();
    QImage px;
    //QPixmap mask;
    QRectF iconRect=rect();
    QRect textRect=rect();
    textRect.translate(0,-1);
    QRectF r=textRect;
    r.translate(0,1);
    QPainter p(this);
    p.setWorldMatrixEnabled(false);
    p.setViewTransformEnabled(false);
    p.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
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
        if (!icon().availableSizes().empty()) s = icon().availableSizes()[0];
        double aspect=double(s.width())/double(s.height());
        double thisaspect=double(iconRect.width())/double(iconRect.height());
        if (aspect<thisaspect)
        {
            iconRect.setWidth(double(iconRect.height())*aspect);
            //iconRect.setLeft((float)(rect().width()-iconRect.width())/2.0);
            iconRect.translate((rect().width()-iconRect.width())/2.0,0);
        }
        if (aspect>thisaspect)
        {
            iconRect.setHeight(double(iconRect.width())*aspect);
            iconRect.translate(0,(rect().height()-iconRect.height())/2.0);
        }
        //iconRect.translate(1,1);
        //iconRect.adjust(2,2,-2,-2);
        r=iconRect;
        r.translate(0,1);
        px = icon().pixmap(s).toImage().convertToFormat(QImage::Format_ARGB32,Qt::DiffuseDither);
        //mask = QPixmap(icon().pixmap(s).createHeuristicMask());
    }
    if (this->isEnabled())
    {
        if (isChecked() || isDown())
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
    auto l=new QGridLayout(this);
    l->setContentsMargins(0,0,0,0);
    l->setVerticalSpacing(0);
    l->setHorizontalSpacing(0);
    frame=new QWidget(this);
    l->addWidget(frame,0,0);
    layout=new QGridLayout(frame);
    layout->setContentsMargins(0,0,0,0);
    layout->setVerticalSpacing(0);
    layout->setHorizontalSpacing(0);
    setSelectMode(QMacButtons::SelectNone);
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
    connect(action,&QAction::triggered,b,&QAbstractButton::animateClick);
    addButton(b,action->text());
}

void QMacButtons::addButton(QToolButton *b, const QString& Name)
{
    b->setCheckable(m_selectMode != QMacButtons::SelectNone);
    b->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    layout->addWidget(b,0,Buttons.size());
    connect(b,&QAbstractButton::pressed,this,qOverload<>(&QMacButtons::wasClicked));
    const int i = Buttons.size();
    connect(b, &QToolButton::clicked, [=] { wasClicked(i); });
    Buttons.append(b);
    Names.append(Name);
}

void QMacButtons::wasClicked()
{
    for (int i=0;i<Buttons.size();i++)
    {
        if (Buttons[i]->isDown())
        {
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

QMacButtons::SelectMode QMacButtons::selectMode()
{
    return m_selectMode;
}

void QMacButtons::setSelectMode(const SelectMode sm)
{
    m_selectMode=sm;
    for (QToolButton* b : std::as_const(Buttons))
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
        for (QToolButton* b : std::as_const(Buttons))
        {
            b->setChecked(false);
        }
        return;
    }
    if (selected)
    {
        for (QToolButton* b : std::as_const(Buttons))
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

bool QMacButtons::isSelected(const int index)
{
    return Buttons[index]->isChecked();
}

bool QMacButtons::isSelected(const QString& name)
{
    return isSelected(Names.indexOf(name));
}

bool QMacButtons::isSelected()
{
    for (const QToolButton* b : std::as_const(Buttons))
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
            for (int i=0;i<Buttons.size();i++)
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
            for (QToolButton* b : std::as_const(Buttons))
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
    for (QToolButton* b : std::as_const(Buttons))
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

int QMacButtons::value()
{
    for (int i=0;i<Buttons.size();i++)
    {
        if (Buttons[i]->isChecked()) return i;
    }
    return -1;
}

int QMacButtons::size()
{
    return Buttons.size();
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

bool QMacButtons::isEnabled(const int index)
{
    return Buttons[index]->isEnabled();
}

bool QMacButtons::isEnabled(const QString& name)
{
    return isEnabled(Names.indexOf(name));
}

bool QMacButtons::isEnabled()
{
    return QWidget::isEnabled();
}

void QMacButtons::setMonochrome(const int index, const bool monochrome)
{
    dynamic_cast<QCustomToolButton*>(Buttons[index])->monochrome=monochrome;
}

void QMacButtons::setMonochrome(const QString& name, const bool monochrome)
{
    setMonochrome(Names.indexOf(name),monochrome);
}

void QMacButtons::setMonochrome(const bool monochrome)
{
    for (QToolButton* b : std::as_const(Buttons))
    {
        dynamic_cast<QCustomToolButton*>(b)->monochrome=monochrome;
    }
}

bool QMacButtons::isMonochrome(const int index)
{
    return dynamic_cast<QCustomToolButton*>(Buttons[index])->monochrome;
}

bool QMacButtons::isMonochrome(const QString& name)
{
    return isMonochrome(Names.indexOf(name));
}

void QMacButtons::setIcon(const int index, const QIcon& icon)
{
    dynamic_cast<QCustomToolButton*>(Buttons[index])->setIcon(icon);
}

void QMacButtons::setIcon(const QString& name, const QIcon& icon)
{
    setIcon(Names.indexOf(name),icon);
}

void QMacButtons::setData(const int index, const QVariant& data)
{
    dynamic_cast<QCustomToolButton*>(Buttons[index])->data=data;
}

void QMacButtons::setData(const QString& name, const QVariant& data)
{
    setData(Names.indexOf(name),data);
}

QVariant QMacButtons::data(const int index)
{
    return dynamic_cast<QCustomToolButton*>(Buttons[index])->data;
}

QVariant QMacButtons::data(const QString& name)
{
    return data(Names.indexOf(name));
}

bool QMacButtons::isMonochrome()
{
    for (QToolButton* b : std::as_const(Buttons))
    {
        if (!dynamic_cast<QCustomToolButton*>(b)->monochrome) return false;
    }
    return true;
}

void QMacButtons::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if (Buttons.empty()) return;
    const auto w = int(double(frame->width())/double(Buttons.size()));
    for (QToolButton* b : std::as_const(Buttons))
    {
        b->setIconSize(QSize(w-4,frame->height()-4));
    }
}
