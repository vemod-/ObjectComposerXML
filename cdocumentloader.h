#ifndef CDOCUMENTLOADER_H
#define CDOCUMENTLOADER_H

#include <QWidget>
#include "cscoredoc.h"
//#include <QMenuBar>
//#include <QToolBar>
#include "../SoftSynthsWidgets/cprojectapp.h"

#define apptitle "Object Composer XML"
#define _DocumentPath QStandardPaths::writableLocation(QStandardPaths::MusicLocation) + "/Object Composer/"

class CDocumentLoader : public CFileDocument {
    Q_OBJECT
public:
    CDocumentLoader(CScoreDoc* doc, QWidget* parent, QStringList arguments = QStringList());
    ~CDocumentLoader();
    void NewDoc() override;
    void OpenDoc(QString Path) override;
    void WizardDoc() override;
    void SaveDoc(QString path) override;
    void UpdateAppTitle();
    void undoSerialize(QDomLiteElement* xml) const override;
    void undoUnserialize(const QDomLiteElement* xml) override;
    void CloseDoc() override;

    QAction* actionExportMIDI;
    QAction* actionExportAudio;
    QAction* actionExportPDF;
    QAction* actionPreferences;

    void ShowPresets();
    void ExportMidi();
    void ExportWave();
    void ExportPDFDialog();
    CScoreDoc* document();
private:
    void Render(QString path);
    CScoreDoc* m_Document;
    QWidget* m_MainWindow;
};

#endif // CDOCUMENTLOADER_H
