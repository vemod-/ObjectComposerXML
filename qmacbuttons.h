#ifndef QMACBUTTONS_H
#define QMACBUTTONS_H

#include <QWidget>
#include <QGridLayout>
#include <QToolButton>
//#include <QFrame>
//#include <QGraphicsView>
#include <QVariant>

namespace Ui {
    class QMacButtons;
}

#define CornerRadius 4

class QCustomToolButton : public QToolButton
{
    Q_OBJECT
public:
    QCustomToolButton(QWidget* parent=nullptr);
    bool monochrome;
    QVariant data;
public slots:
protected:
    void paintEvent(QPaintEvent * event);
private:
    QRect findRect();
    QImage minBrightness(QImage img, int value);
    QImage setBrightness(QImage img, int value, bool desaturate=false);
};

class QMacButtons : public QWidget
{
    Q_OBJECT
public:
    explicit QMacButtons(QWidget *parent = nullptr);
    ~QMacButtons();
    bool isSelected(const QString& name);
    bool isSelected(int index);
    bool isSelected();
    void setSelected(const QString& name, const bool selected);
    void setSelected(const int index, const bool selected);
    void setSelected(const bool selected);
    void setDown(const QString& name, const bool down);
    void setDown(const int index, const bool down);
    void setDown(const bool down);
    void setTooltip(const int index, const QString& tooltip);
    void setTooltip(const QString& name, const QString& tooltip);
    enum SelectMode
    {
        SelectNone,
        SelectOneOrNone,
        SelectOne,
        SelectAll
    };
    void setSelectMode(const SelectMode sm);
    SelectMode selectMode();
    void setEnabled(const QString& name, const bool enabled);
    void setEnabled(const int index, const bool enabled);
    void setEnabled(const bool enabled);
    bool isEnabled(const QString& name);
    bool isEnabled(const int index);
    bool isEnabled();
    void setMonochrome(const QString& name, const bool monochrome);
    void setMonochrome(const int index, const bool monochrome);
    void setMonochrome(const bool monochrome);
    bool isMonochrome(const QString& name);
    bool isMonochrome(const int index);
    bool isMonochrome();
    void setIcon(const QString& name, const QIcon& icon);
    void setIcon(const int index, const QIcon& icon);
    void setData(const QString& name, const QVariant& value);
    void setData(const int index, const QVariant& value);
    QVariant data(const QString& name);
    QVariant data(const int index);
    int value();
    int size();
signals:
    void buttonClicked(int index);
    void buttonClicked(QString name);
    void selected(int index);
    void selected(QString name);
public slots:
    void addButton(const QString& Name, const QString &Tooltip, const QIcon& Icon);
    void addButton(const QString& Name, const QString &Tooltip, const QString& Text, const QFont& Font=QFont());
    void addButton(QAction* action, QKeySequence keySequence);
protected:
    virtual void resizeEvent(QResizeEvent* event);
private slots:
    void wasClicked();
    void wasClicked(int index);
private:
    Ui::QMacButtons *ui;
    SelectMode m_selectMode=SelectNone;
    void addButton(QToolButton* b, const QString& Name);
    QList<QToolButton*> Buttons;
    QStringList Names;
    QGridLayout* layout;
    QWidget* frame;
};

#endif // QMACBUTTONS_H
