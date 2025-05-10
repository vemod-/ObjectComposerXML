#ifndef QMACTREEWIDGET_H
#define QMACTREEWIDGET_H

#define tablerowheight 17

#include <QTreeWidget>
#include <QDragMoveEvent>

class QMacTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit QMacTreeWidget(QWidget *parent = nullptr);
    void setIconMode(const bool icons);
    bool iconMode() const;
    QTreeWidgetItem* createTopLevelItem(const QString& text, const QString& iconPath=QString());
    QTreeWidgetItem* createStandardItem(const QString& text, const QString& iconPath=QString(), const int CloseIcon=0, const int SaveIcon=0);
    void adjust();
    int visibleRowCount();
    QSize contentSize();
signals:
    void Popup(QPoint Pos);
    void itemsMoved(QList<QTreeWidgetItem*> dragItems);
public slots:
protected:
    void resizeEvent(QResizeEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
private slots:
    void MouseRelease(QMouseEvent* event);
    void MouseMove(QMouseEvent* event);
    void MouseEnter(QEvent* event);
    void MouseLeave(QEvent* event);
private:
    bool m_iconmode;
};

#endif // QMACTREEWIDGET_H
