#ifndef CWINDOWSTACK_H
#define CWINDOWSTACK_H

#include <QWidget>

namespace Ui {
    class CWindowStack;
}

class CWindowStack : public QWidget
{
    Q_OBJECT

public:
    explicit CWindowStack(QWidget *parent = 0);
    ~CWindowStack();
    QList<QWidget*> subWindowList;
public slots:
    QWidget* addSubWindow(QWidget* sw);
    QWidget* currentSubWindow();
    QWidget* activeSubWindow();
    QWidget* setActiveSubWindow(QWidget* sw);
    QWidget* closeSubWindow(QWidget* sw);
    QWidget* closeActiveSubWindow();
    void activatePreviousSubWindow();
    void activateNextSubWindow();
    void removeSubWindow(QWidget* sw);
signals:
    void subWindowActivated(QWidget* sw);
private:
    Ui::CWindowStack *ui;
    int activeIndex;
};

#endif // CWINDOWSTACK_H
