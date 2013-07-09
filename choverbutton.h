#ifndef CHOVERBUTTON_H
#define CHOVERBUTTON_H

#include <QWidget>
#include <QToolButton>

class CHoverButton : public QToolButton
{
    Q_OBJECT
public:
    explicit CHoverButton(QWidget *parent = 0);
    void setIcon(const QIcon &icon);
    void setID(int ID);
signals:
    void clicked(int ID);
public slots:
protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
private:
    QIcon holdIcon;
    int holdID;
private slots:
    void wasClicked();
};

#endif // CHOVERBUTTON_H
