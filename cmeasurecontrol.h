#ifndef CMEASURECONTROL_H
#define CMEASURECONTROL_H

#include <QGroupBox>

namespace Ui {
    class CMeasureControl;
}

class CMeasureControl : public QGroupBox
{
    Q_OBJECT

public:
    explicit CMeasureControl(QWidget *parent = 0);
    ~CMeasureControl();
    const int Millimeters();
    void setMillimeters(const int MM);
private:
    Ui::CMeasureControl *ui;
private slots:
    void UpdateInches();
    void UpdateMillimeters();
signals:
    void Changed();
};

#endif // CMEASURECONTROL_H
