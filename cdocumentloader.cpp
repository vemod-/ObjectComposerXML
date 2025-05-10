#include "cdocumentloader.h"
#include "cpresetsdialog.h"
#include "cpropertywindow.h"
#include "cscorewizard.h"
#include "czoomwidget.h"

CDocumentLoader::CDocumentLoader(CScoreDoc* doc, QWidget* parent, QStringList arguments)
    : CFileDocument(OCPresets().Organization(), OCPresets().Application(), _DocumentPath,parent), m_Document(doc), m_MainWindow(parent) {
    setVisible(false);

    NameFilter = "XML files (*.xml *.zip *.mus)";

    menuFile->addSeparator();
    actionExportMIDI = menuFile->addAction("Export MIDI...",this,&CDocumentLoader::ExportMidi);
    actionExportAudio = menuFile->addAction("Export Audio...",this,&CDocumentLoader::ExportWave);
    menuFile->addSeparator();
    menuFile->addActions(m_Document->playControl->actionMenu->actions());
    menuFile->addSeparator();
    actionExportPDF = m_Document->printMenu->addAction("Export PDF...",this,&CDocumentLoader::ExportPDFDialog);
    actionExportPDF->setEnabled(false);
    menuFile->addActions(m_Document->printMenu->actions());
    menuFile->addSeparator();
    actionPreferences = menuFile->addAction("Preferences..",this,&CDocumentLoader::ShowPresets);
    actionPreferences->setMenuRole(QAction::ApplicationSpecificRole);

    QList<QAction*> l;
    l.append(m_Document->EditMenu->actions());
    for (QAction* a : m_Document->EditMenu->actions()) m_Document->EditMenu->removeAction(a);
    UndoMenu->actionUndo->setIcon(QIcon(":/mini/mini/repeat.png"));
    UndoMenu->actionRedo->setIcon(QIcon(":/mini/mini/redo.png"));
    m_Document->UndoActions.append(UndoMenu->actions());
    m_Document->EditMenu->addActions(m_Document->UndoActions);
    m_Document->EditMenu->addSeparator();
    m_Document->EditMenu->addActions(l);
    connect(m_Document,&CScoreDoc::aboutToChange,UndoMenu,&CUndoMenu::addItem,Qt::DirectConnection);
    connect(m_Document,&CScoreDoc::aboutToChangeElement,UndoMenu,&CUndoMenu::addElement,Qt::DirectConnection);

    connect(m_Document,&CScoreDoc::showProjects,this,&CDocumentLoader::showProjects,Qt::DirectConnection);
    connect(m_Document,&CScoreDoc::Changed,this,&CDocumentLoader::UpdateAppTitle);

    CZoomWidget* ZoomWidget = new CZoomWidget(doc);
    connect(ZoomWidget,&CZoomWidget::valueChanged,m_Document,&CScoreDoc::SetZoom);
    connect(doc,&CScoreDoc::ZoomChanged,ZoomWidget,&CZoomWidget::setValue);

    CToolBar* leftToolBar=new CToolBar(doc);
    leftToolBar->addWidget(ZoomWidget);

    CToolBar* playToolBar = new CToolBar(doc);
    playToolBar->addAction(m_Document->playControl->getPlayButton());
    playToolBar->addAction(m_Document->playControl->getMixerButton());

    CToolBar* rightToolBar=new CToolBar(doc);
    rightToolBar->addActions(m_Document->ToolBarActions->actions());
    rightToolBar->addActions(UndoMenu->actions());
    rightToolBar->addActions(m_Document->ToolBarActions2->actions());
    rightToolBar->addAction(m_Document->RightSideButton);

    CStatusBar* statusBar = new CStatusBar(doc);
    statusBar->addSpacing(10);
    statusBar->addWidget(leftToolBar,1,Qt::AlignLeft);
    statusBar->addWidget(playToolBar,0,Qt::AlignHCenter);
    statusBar->addWidget(rightToolBar,1,Qt::AlignRight);
    doc->layout()->addWidget(statusBar);
}
CDocumentLoader::~CDocumentLoader() {
    m_Document->MIDI2wav->clear();
}

void CDocumentLoader::NewDoc()
{
    qDebug() << "Documentloader NewDoc";
    XMLScoreWrapper s;
    s.newScore();
    m_Document->SetView(0);
    m_Document->SetXML(s.getXML()->clone());
    m_Document->setWindowTitle("Untitled");
    m_Document->ActivateDoc();
}
void CDocumentLoader::OpenDoc(QString Path)
{
    qDebug() << "Documentloader OpenDoc";
    ImportResult isImport = NoImport;
    QFileInfo f(Path);
    if (f.suffix()=="zip") {
        QDomLiteDocument d = CProjectPage::openFile(Path,".mus");
        QDomLiteElement xml;
        xml.appendChild(d.documentElement->clone());
        m_Document->unserialize(&xml);  //m_Document->SetXML(d.clone());
        //setView(xml.firstChild()->attributeValueBool("DocumentView",true));
        isImport = ImportNativeXML;
    }
    else {
        isImport = m_Document->Load(Path);
    }
    if (isImport)
    {
        (isImport > ImportNativeXML) ? m_Document->setWindowTitle(f.baseName()) : m_Document->setWindowTitle(Path);
        //m_Document->Path = (isImport==ImportMusicXML) ? QString() : Path;
        m_Document->ActivateDoc();
        if (isImport != ImportMusicXML) menuRecent->AddRecentFile(Path);
    }

    m_Document->UpdateStatus();
    if (!isImport)
    {
        NewDoc();
        nativeAlert(m_MainWindow,"Object Composer","Could not Open File: " + Path,{"Ok"});
    }
}
void CDocumentLoader::WizardDoc()
{
    qDebug() << "Documentloader WizardDoc";
    CScoreWizard d(m_MainWindow);
    d.Open(":/Template.mus");
    d.setWindowTitle("New Score");
    if (d.exec()==QDialog::Accepted)
    {
        m_Document->setWindowTitle("Untitled");
        m_Document->SetXML(d.CreateXML());
        m_Document->ActivateDoc();
    }
}
void CDocumentLoader::ShowPresets()
{
    CPresetsDialog d(m_MainWindow);
    d.setWindowTitle("Presets");
    CPropertyWindow* pw=d.findChild<CPropertyWindow*>();
    OCPresets P;
    pw->Fill(P.Properties());
    if (d.exec()==QDialog::Accepted) P.SaveProperties();
}
void CDocumentLoader::UpdateAppTitle()
{
    QString title=m_Document->windowTitle();
    if (UndoMenu->isDirty()) title+="*";
    m_MainWindow->setWindowTitle(title + " - " + apptitle);
}

void CDocumentLoader::undoSerialize(QDomLiteElement* xml) const  {
    m_Document->serialize(xml);
}

void CDocumentLoader::undoUnserialize(const QDomLiteElement* xml)  {
    m_Document->unserialize(xml);
}

CScoreDoc* CDocumentLoader::document() {
    return m_Document;
}

void CDocumentLoader::Render(QString path) {
    m_Document->MIDI2wav->renderWaveFile(path);
}

void CDocumentLoader::CloseDoc() {
    qDebug() << "Documentloader CloseDoc";
    if (!m_Document->isClosed) {
        if (m_Document->MIDI2wav->isPlaying()) m_Document->MIDI2wav->pause();
        m_MainWindow->setWindowTitle(apptitle);
        m_Document->playControl->hide();
        m_Document->MIDI2wav->clearMixer();
        m_Document->isClosed = true;
    }
}

void CDocumentLoader::SaveDoc(QString path)
{
    qDebug() << "Documentloader SaveDoc";
    //if (m_Document->Path.length()==0) return SaveDialog();
    QFileInfo f(path);
    QString p = _DocumentPath + f.baseName() + ".zip";
    QDomLiteElement xml;
    m_Document->serialize(&xml);
    xml.firstChild()->setAttribute("DocumentView",m_Document->isVisible());
    QDomLiteDocument Doc;
    Doc.replaceDoc(xml.firstChild()->clone());
    CProjectPage::saveFile(p,&Doc,m_Document->grab());
}

void CDocumentLoader::ExportMidi()
{
    QFileDialog d(m_MainWindow,Qt::Sheet);
    d.setWindowTitle("Export MIDI");
    d.setNameFilter(tr("MIDI Files (*.mid)"));
    d.setAcceptMode(QFileDialog::AcceptSave);
    d.setFileMode(QFileDialog::AnyFile);
    if (FileName.isEmpty())
    {
        d.selectFile("Untitled.mid");
    }
    else
    {
        d.selectFile(FileName.replace("." + QFileInfo(FileName).completeSuffix(),".mid",Qt::CaseInsensitive));
    }
    if (d.exec() != QDialog::Accepted) return;
    if (d.selectedFiles().size())
    {
        QString path=d.selectedFiles().first();
        m_Document->sv->play(0,0,path);
    }
}
void CDocumentLoader::ExportWave()
{
    QFileDialog d(m_MainWindow,Qt::Sheet);
    d.setWindowTitle("Export Audio");
    d.setNameFilter(tr("Wave Files (*.wav)"));
    d.setAcceptMode(QFileDialog::AcceptSave);
    d.setFileMode(QFileDialog::AnyFile);
    if (FileName.isEmpty())
    {
        d.selectFile("Untitled.wav");
    }
    else
    {
        d.selectFile(FileName.replace("." + QFileInfo(FileName).completeSuffix(),".wav",Qt::CaseInsensitive));
    }
    if (d.exec() != QDialog::Accepted) return;
    if (d.selectedFiles().size())
    {
        QString path=d.selectedFiles().first();
        CConcurrentDialog::run(this,&CDocumentLoader::Render,path);
    }
}

void CDocumentLoader::ExportPDFDialog() {
    QFileDialog d(m_MainWindow,Qt::Sheet);
    d.setWindowTitle("Export PDF");
    d.setNameFilter(tr("PDF Files (*.pdf)"));
    d.setAcceptMode(QFileDialog::AcceptSave);
    d.setFileMode(QFileDialog::AnyFile);
    if (FileName.isEmpty())
    {
        d.selectFile("Untitled.pdf");
    }
    else
    {
        QFileInfo f(FileName);
        QString LayoutName = f.baseName();
        int index = m_Document->lv->activeLayoutIndex();
        if (index > -1) LayoutName = m_Document->lv->LayoutName(index);
        QString path = f.path() + "/" + LayoutName + ".pdf";
        d.selectFile(path);
    }
    if (d.exec() != QDialog::Accepted) return;
    if (d.selectedFiles().size()) {
        QString path=d.selectedFiles().first();
        m_Document->ExportPDF(path);
    }
}
