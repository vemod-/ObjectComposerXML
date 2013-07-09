#ifndef CPROPERTYWINDOW_H
#define CPROPERTYWINDOW_H

#include <QWidget>
#include <QGridLayout>
#include "qmactreewidget.h"
#include <QComboBox>
#include <QLineEdit>
#include <QToolButton>
#include <QCheckBox>
#include <QSpinBox>
#include "scoreviewxml.h"

class CCustomButton :public QWidget
{
    Q_OBJECT
public:
    CCustomButton(QString Caption,QWidget* parent=0);
    QToolButton* tb;
};

class CCustomCheck :public QWidget
{
    Q_OBJECT
public:
    CCustomCheck(QWidget* parent=0);
    QCheckBox* cb;
};

namespace Ui {
    class CPropertyWindow;
}

class CPropertyWindow : public QWidget
{
    Q_OBJECT
public:
    explicit CPropertyWindow(QWidget *parent = 0);
    ~CPropertyWindow();
    void Paint();
    void Clear();
public slots:
    void Fill(OCProperties* Properties);
protected:
    void leaveEvent(QEvent* event);
    void enterEvent(QEvent *event);
signals:
    void Changed(QString Name, OCProperties* p);
private:
    Ui::CPropertyWindow *ui;
    QString LastSelected;
    OCProperties* m_PropColl;
    QMacTreeWidget* table;
    QWidget* edit;
    QTreeWidgetItem* editRow;
    QString PropertyText(OCProperty* p);
    void HideItems();
private slots:
    void Edit();
    void BoolChanged(int Value);
    void BoolChanged(bool Value);
    void ListChanged(int Value);
    void NumberChanged(int Value);
    void NumberChanged();
    void SliderChanged();
    void TextChanged(QString Value);
    void TextChanged();
    void CustomChanged();
};

#endif // CPROPERTYWINDOW_H
