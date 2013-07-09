#ifndef CTWEAKSYSTEM_H
#define CTWEAKSYSTEM_H

#include <QDialog>
#include "CommonClasses.h"
#include "clayout.h"

namespace Ui {
    class CTweakSystem;
}

class CTweakSystem : public QDialog
{
    Q_OBJECT

public:
    explicit CTweakSystem(QWidget *parent = 0);
    ~CTweakSystem();
    void Fill(XMLScoreWrapper& Score, CLayout* Layout, int ActivePage, int ActiveSystem);
    void GetResult(int &Page, int &System, XMLScoreWrapper& Score);
private:
    Ui::CTweakSystem *ui;
    CLayout* m_Layout;
    int m_ActivePage;
    int m_ActiveSystem;
    void Paint();
private slots:
    void LocationClicked(int value);
    void FillVoicesCombo(int Staff);
    void SelectVoice(int Voice);
    void FillLabel();
    void NextSystem();
    void PrevSystem();
    void LastSystem();
    void FirstSystem();
};

#endif // CTWEAKSYSTEM_H
