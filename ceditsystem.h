#ifndef CEDITSYSTEM_H
#define CEDITSYSTEM_H

#include "CommonClasses.h"
#include <QDialog>
#include <QDomLite>
#include <QListWidget>

namespace Ui {
    class CEditSystem;
}

class CEditSystem : public QDialog
{
    Q_OBJECT

public:
    explicit CEditSystem(QWidget *parent = 0);
    ~CEditSystem();
    void Fill(QDomLiteElement* LayoutTemplate,QDomLiteElement* SystemTemplate);
    void GetTemplate(QDomLiteElement *LayoutTemplate, QDomLiteElement *SystemTemplate);
private:
    Ui::CEditSystem *ui;
private slots:
    void Validate(QListWidgetItem* item);
};

#endif // CEDITSYSTEM_H
