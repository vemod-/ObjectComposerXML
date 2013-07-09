#ifndef CSCOREWIZARD_H
#define CSCOREWIZARD_H

#define wLWName 32
#define wLWAbbreviation 33
#define LWClef 34
#define LWPan 35
#define LWTranspose 36
#define LWNumOfVoices 37

#include <QDialog>
#include <QPushButton>
#include "ocsymbolscollection.h"

namespace Ui {
    class CScoreWizard;
}

class CScoreWizard : public QDialog
{
    Q_OBJECT

public:
    explicit CScoreWizard(QWidget *parent = 0);
    ~CScoreWizard();
    void Fill(XMLScoreWrapper& XMLScore);
    void Open(const QString& Path);
    QDomLiteDocument* CreateXML();
private:
    Ui::CScoreWizard *ui;
    int Keys[12];
    void ValidateStaffs();
private slots:
    void MoveRight();
    void MoveLeft();
    void MoveAllRight();
    void MoveAllLeft();
    void AppendFile();
};

#endif // CSCOREWIZARD_H
