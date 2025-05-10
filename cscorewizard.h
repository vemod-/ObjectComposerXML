#ifndef CSCOREWIZARD_H
#define CSCOREWIZARD_H

#define wLWName 32
#define wLWAbbreviation 33
#define LWClef 34
#define LWPan 35
#define LWTranspose 36
#define LWNumOfVoices 37

#include <QDialog>
#include <QPushButton>
#include "ocsymbolscollection.h"

namespace Ui {
    class CScoreWizard;
}

class CScoreWizard : public QDialog
{
    Q_OBJECT

public:
    explicit CScoreWizard(QWidget *parent = nullptr);
    ~CScoreWizard();
    void Fill(XMLScoreWrapper& XMLScore);
    void Open(const QString& Path);
    QDomLiteDocument* CreateXML();
private:
    Ui::CScoreWizard *ui;
    std::array<int,12> Keys={{0}};
    void ValidateStaffs();
    inline XMLSimpleSymbolWrapper createSymbol(const QString& symbolName)
    {
#ifdef OCSYMBOLSCOLLECTION_H
        XMLSimpleSymbolWrapper w = OCSymbolsCollection::GetDefaultSymbol(symbolName);
#else
        XMLSimpleSymbolWrapper w(symbolName);
#endif
        return w;
    }
    inline XMLSimpleSymbolWrapper createSymbol(const QString& symbolName, const QString& attrName, const QVariant& attrValue)
    {
#ifdef OCSYMBOLSCOLLECTION_H
        XMLSimpleSymbolWrapper w = OCSymbolsCollection::GetDefaultSymbol(symbolName);
#else
        XMLSimpleSymbolWrapper w(symbolName);
#endif
        w.setAttribute(attrName,attrValue);
        return w;
    }
    inline XMLSimpleSymbolWrapper createSymbol(const QString& symbolName, const QStringList& attrNames, const QVariantList& attrValues)
    {
#ifdef OCSYMBOLSCOLLECTION_H
        XMLSimpleSymbolWrapper w = OCSymbolsCollection::GetDefaultSymbol(symbolName);
#else
        XMLSimpleSymbolWrapper w(symbolName);
#endif
        for (int i = 0; i < attrNames.size(); i++) w.setAttribute(attrNames[i],attrValues[i]);
        return w;
    }
private slots:
    void MoveRight();
    void MoveLeft();
    void MoveAllRight();
    void MoveAllLeft();
    void AppendFile();
};

#endif // CSCOREWIZARD_H
