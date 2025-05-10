#ifndef CEDITDIALOG_H
#define CEDITDIALOG_H

#include <QDialog>
#include "ceditwidget.h"
#include <QListWidget>
#include <QGridLayout>
#include <QDomLite>
#include <QTextStream>

namespace Ui {
    class CEditDialog;
}

class CEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CEditDialog(QWidget *parent = nullptr);
    ~CEditDialog();
    CEditWidget* EditWidget;
    void ShowList(const QString& TopNode);
    void QuickAccept(bool hide = false);
private slots:
    void AddItem();
    void RemoveItem();
    void SetItem(int Index);
    void RenameItem(QListWidgetItem* item);
    void AcceptItem(QListWidgetItem* item);
    //void HideList();
    void SaveItem();
private:
    Ui::CEditDialog *ui;
    void InitLayout(bool HasList);
    QDomLiteDocument XML;
    QDomLiteElement* theNode=nullptr;
    QString TopNode;
};

#endif // CEDITDIALOG_H
