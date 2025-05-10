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
#include "csymbol.h"

class CCustomButton :public QWidget
{
    Q_OBJECT
public:
    CCustomButton(const QString& Caption,QWidget* parent=nullptr);
    QToolButton* tb;
};

class CCustomCheck :public QWidget
{
    Q_OBJECT
public:
    CCustomCheck(QWidget* parent=nullptr);
    QCheckBox* cb;
};

namespace Ui {
    class CPropertyWindow;
}

class CPropertyWindow : public QWidget
{
    Q_OBJECT
public:
    explicit CPropertyWindow(QWidget *parent = nullptr);
    ~CPropertyWindow();
    void Paint();
    void Clear();
    void Fill(OCProperties *Properties);
    void Fill(const XMLSimpleSymbolWrapper& Symbol,const int Voice);
    void UpdateProperties(const XMLSimpleSymbolWrapper& Symbol,const int Voice);
    QSize contentSize();
protected:
    void leaveEvent(QEvent* event);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    void enterEvent(QEnterEvent *event);
#else
    void enterEvent(QEvent *event);
#endif
signals:
    void Changed(QString Name, QVariant Value, bool Custom);
private:
    Ui::CPropertyWindow *ui;
    QString LastSelected;
    OCProperties* m_PropColl=nullptr;
    QMacTreeWidget* table;
    QWidget* edit=nullptr;
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
    void TextChanged(const QString &Value);
    void TextChanged();
    void CustomChanged();
};

#endif // CPROPERTYWINDOW_H
