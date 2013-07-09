#ifndef QMACTREEWIDGET_H
#define QMACTREEWIDGET_H

#include <QTreeWidget>

class QMacTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit QMacTreeWidget(QWidget *parent = 0);
    void setIconMode(const bool icons);
    const bool iconMode() const;
    QTreeWidgetItem* createTopLevelItem(const QString& text, const QString& iconPath=QString());
    QTreeWidgetItem* createStandardItem(const QString& text, const QString& iconPath=QString(), const int CloseIcon=0, const int SaveIcon=0);
    void adjust();
signals:
    void Popup(QPoint Pos);
public slots:
protected:
    void resizeEvent(QResizeEvent *event);
private slots:
    void MouseRelease(QMouseEvent* event);
    void MouseMove(QMouseEvent* event);
    void MouseEnter(QEvent* event);
    void MouseLeave(QEvent* event);
private:
    bool m_iconmode;
};

#endif // QMACTREEWIDGET_H
