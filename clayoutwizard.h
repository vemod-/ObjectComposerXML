#ifndef CLAYOUTWIZARD_H
#define CLAYOUTWIZARD_H

#include "CommonClasses.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QToolButton>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QPrinter>
#include <QPageSetupDialog>
#include <QPrintDialog>
#else
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPageSetupDialog>
#include <QtPrintSupport/QPrintDialog>
#endif

#define LWAllTemplateIndex 32
#define LWSquareBracket 33
#define LWCurlyBracket 34
#define LWName 35
#define LWAbbreviation 36
#define LWSize 37

namespace Ui {
    class CLayoutWizard;
}

class CLayoutWizard : public QDialog
{
    Q_OBJECT

public:
    explicit CLayoutWizard(QWidget *parent = 0);
    ~CLayoutWizard();
    int ShowModal();
    void Fill(XMLScoreWrapper& XMLScore, int Index);
    void ModifyXML(XMLScoreWrapper& XMLScore, int Index);
private:
    Ui::CLayoutWizard *ui;
    void FillBrackets();
    void ValidateBrackets();
    void ValidateStaffs();
    CTextElement TitleElement;
    CTextElement SubtitleElement;
    CTextElement ComposerElement;
    CTextElement NamesElement;
    QGraphicsScene S;
    QPrinter* Printer;
    QToolButton* pageSetupButton;
protected:
private slots:
    void MoveRight();
    void MoveLeft();
    void MoveAllRight();
    void MoveAllLeft();
    void TabChanged(int Index);
    void SizeChanged(int Size);
    QString SliderTooltip(int Value);
    void SpacingTooltip(int Value);
    void AddSquare();
    void AddCurly();
    void RemoveBracket();
    void UpdateMargins();
    void ShowPageSetup();
};

#endif // CLAYOUTWIZARD_H
