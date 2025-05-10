#ifndef CTWEAKSYSTEM_H
#define CTWEAKSYSTEM_H

//#include "CommonClasses.h"
//#include "clayout.h"
#include "ocxmlwrappers.h"

namespace Ui {
    class CTweakSystem;
}

class CTweakSystem : public QWidget
{
    Q_OBJECT

public:
    explicit CTweakSystem(QWidget *parent = nullptr);
    ~CTweakSystem();
    void Fill(XMLScoreWrapper& Score, const int activeLayoutIndex, const LayoutLocation& l, const double zoom);
    void GetResult(LayoutLocation& l, XMLScoreWrapper& Score);
    QPointF sysPos();
    bool accepted() { return m_Accepted; }
private:
    Ui::CTweakSystem *ui;
    XMLLayoutWrapper m_Layout;
    int m_ActiveLayout=0;
    LayoutLocation m_ActiveLocation;
    void Paint();
    bool m_Accepted = false;
private slots:
    void LocationClicked(int value);
    void FillVoicesCombo(int Staff);
    void SelectVoice(int Voice);
    void FillLabel();
    void NextSystem();
    void PrevSystem();
    void LastSystem();
    void FirstSystem();
    void Accept() {
        m_Accepted = true;
        static_cast<QWidget*>(parent())->hide();
    }
    void PopupProperties(QPoint p);
    void ChangeProperty(QString Name, QVariant Value, bool Custom);
    void ShowProperties();
};

#endif // CTWEAKSYSTEM_H
