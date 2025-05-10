#ifndef QFONTWIDGET_H
#define QFONTWIDGET_H

#include <QGroupBox>

namespace Ui {
    class QFontWidget;
}

class QFontWidget : public QGroupBox
{
    Q_OBJECT

public:
    explicit QFontWidget(QWidget *parent = nullptr);
    ~QFontWidget();
    void fill(const QFont& Font, const QString& Text="Sample Text", const bool Locked=false, const Qt::Alignment Align=Qt::AlignCenter, const double Scale=1.0);
    const QFont font();
    const QString text();
private:
    Ui::QFontWidget *ui;
    double m_Scale=1;
private slots:
    void updateLineEdit();
signals:
    void Changed();
};

#endif // QFONTWIDGET_H
