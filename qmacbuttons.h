#ifndef QMACBUTTONS_H
#define QMACBUTTONS_H

#include <QWidget>
#include <QGridLayout>
#include <QToolButton>
#include <QFrame>
#include <QGraphicsView>
#include <QSignalMapper>

namespace Ui {
    class QMacButtons;
}

#define CornerRadius 4

class QCustomToolButton : public QToolButton
{
    Q_OBJECT
public:
    QCustomToolButton(QWidget* parent=0);
    bool monochrome;
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
    explicit QMacButtons(QWidget *parent = 0);
    ~QMacButtons();
    const bool isSelected(const QString& name);
    const bool isSelected(int index);
    const bool isSelected();
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
    const SelectMode selectMode();
    void setEnabled(const QString& name, const bool enabled);
    void setEnabled(const int index, const bool enabled);
    void setEnabled(const bool enabled);
    const bool isEnabled(const QString& name);
    const bool isEnabled(const int index);
    const bool isEnabled();
    void setMonochrome(const QString& name, const bool monochrome);
    void setMonochrome(const int index, const bool monochrome);
    void setMonochrome(const bool monochrome);
    const bool isMonochrome(const QString& name);
    const bool isMonochrome(const int index);
    const bool isMonochrome();
    const int value();
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
    SelectMode m_selectMode;
    void addButton(QToolButton* b, const QString& Name);
    QList<QToolButton*> Buttons;
    QStringList Names;
    QGridLayout* layout;
    QWidget* frame;
    //void ShowButtons();
    QSignalMapper* mapper;
};

#endif // QMACBUTTONS_H
