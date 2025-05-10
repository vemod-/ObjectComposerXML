#ifndef CEDITSYSTEM_H
#define CEDITSYSTEM_H

#include <QDialog>
#include <QDomLite>
#include <QListWidget>
#include "ocxmlwrappers.h"

namespace Ui {
    class CEditSystem;
}

class CEditSystem : public QDialog
{
    Q_OBJECT

public:
    explicit CEditSystem(QWidget *parent = nullptr);
    ~CEditSystem();
    void Fill(const XMLTemplateWrapper& LayoutTemplate, const XMLTemplateWrapper& SystemTemplate, const XMLScoreWrapper& XMLScore);
    void GetTemplate(const XMLTemplateWrapper& LayoutTemplate, XMLTemplateWrapper& SystemTemplate);
private:
    Ui::CEditSystem *ui;
private slots:
    void Validate(QListWidgetItem* item);
};

#endif // CEDITSYSTEM_H
