#ifndef QMACSPLITTER_H
#define QMACSPLITTER_H

#include <QSplitter>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSettings>
//#include "qpicframe.h"

class QMacSplitterHandle : public QSplitterHandle
{
Q_OBJECT
public:
    QMacSplitterHandle(Qt::Orientation orientation, QSplitter *parent);
    void paintEvent(QPaintEvent *e);
    QSize sizeHint() const;
};

class QMacSplitter : public QSplitter
{
    Q_OBJECT
public:
    QMacSplitter(QWidget* parent=nullptr);
    QMacSplitter(Qt::Orientation o,QWidget* parent=nullptr);
    QSplitterHandle *createHandle();
    void collapse(int index);
    void expand();
    bool isCollapsed();
    void Load(const QString& Tag, QSettings& s);
    void Save(const QString& Tag, QSettings& s);
protected:
    int collapseIndex=0;
    //int collapseSize;
private:
    QParallelAnimationGroup* anim=nullptr;
    QPropertyAnimation* a1=nullptr;
    QPropertyAnimation* a2=nullptr;
    int expandedSize=0;
    QSize expandedMaxSize;
    QSize expandedMinSize;
    int expandIndex=0;
    void init();
private slots:
    void animationFinished();
};

#endif // QMACSPLITTER_H
