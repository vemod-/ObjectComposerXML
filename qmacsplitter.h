#ifndef QMACSPLITTER_H
#define QMACSPLITTER_H

#include <QSplitter>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
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
    QMacSplitter(QWidget* parent=0);
    QMacSplitter(Qt::Orientation o,QWidget* parent=0);
    QSplitterHandle *createHandle();
    void collapse(int index);
    void expand();
    bool isCollapsed();
    void Load(QString Tag);
    void Save(QString Tag);
protected:
    int collapseIndex;
    //int collapseSize;
private:
    QParallelAnimationGroup* anim;
    QPropertyAnimation* a1;
    QPropertyAnimation* a2;
    int expandedSize;
    QSize expandedMaxSize;
    QSize expandedMinSize;
    int expandIndex;
    void init();
private slots:
    void animationFinished();
};

#endif // QMACSPLITTER_H
