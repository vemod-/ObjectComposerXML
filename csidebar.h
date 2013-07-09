#ifndef CSIDEBAR_H
#define CSIDEBAR_H

#include "CommonClasses.h"
#include "mouseevents.h"
#include <QWidget>

namespace Ui {
    class CSidebar;
}

class CSidebar : public QWidget
{
    Q_OBJECT

public:
    explicit CSidebar(QWidget *parent = 0);
    ~CSidebar();

private:
    Ui::CSidebar *ui;
};

#endif // CSIDEBAR_H
