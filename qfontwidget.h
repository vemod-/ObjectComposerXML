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
    explicit QFontWidget(QWidget *parent = 0);
    ~QFontWidget();
    void Fill(const QFont& Font, const QString& Text="Sample Text", const bool Locked=false, const Qt::Alignment Align=Qt::AlignCenter, const float Scale=1.0);
    const QFont Font();
    const QString Text();
private:
    Ui::QFontWidget *ui;
    float m_Scale;
private slots:
    void UpdateLineEdit();
};

#endif // QFONTWIDGET_H
