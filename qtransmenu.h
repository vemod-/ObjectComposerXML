#ifndef QTRANSMENU_H
#define QTRANSMENU_H

#include <QMenu>

class QTransMenu : public QMenu
{
    Q_OBJECT
public:
    QTransMenu(QWidget* parent=nullptr, bool deleteOnClose = true) : QMenu(parent) {
        QMenu::setAttribute(Qt::WA_DeleteOnClose,deleteOnClose);
    }
    QTransMenu(QMenu* m, QWidget* parent=nullptr, bool deleteOnClose = true) : QMenu(parent) {
        QMenu::setAttribute(Qt::WA_DeleteOnClose,deleteOnClose);
        addActions(m->actions());
    }
    QTransMenu(QList<QAction*> l, QWidget* parent=nullptr, bool deleteOnClose = true) : QMenu(parent) {
        QMenu::setAttribute(Qt::WA_DeleteOnClose,deleteOnClose);
        addActions(l);
    }
    QAction* addActionX(const QString& text, const bool enabled)
    {
        QAction* a=QMenu::addAction(text);
        a->setEnabled(enabled);
        return a;
    }
};

#endif // QTRANSMENU_H
