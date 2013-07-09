#ifndef CPRESETSDIALOG_H
#define CPRESETSDIALOG_H

#include <QDialog>
#include "csymbol.h"

namespace Ui {
    class CPresetsDialog;
}

class CPresetsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CPresetsDialog(QWidget *parent = 0);
    ~CPresetsDialog();
    //QtAbstractPropertyBrowser *editor1;
private:
    Ui::CPresetsDialog *ui;

};

#endif // CPRESETSDIALOG_H
