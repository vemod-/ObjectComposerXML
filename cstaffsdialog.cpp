#include "cstaffsdialog.h"
#include "ui_cstaffsdialog.h"
#include "mouseevents.h"
#include <QPainter>
#include <QMessageBox>
#include <QMimeData>

void SpeakerDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool value = index.data().toUInt();
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());
    if (value)
    {
        QImage Img(":/mute.png");
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        QRect imgrect(option.rect.adjusted(1,1,-1,-1));
        imgrect.setWidth(imgrect.height());
        painter->drawImage(imgrect,Img);
    }
    if(option.state & QStyle::State_Selected)
    {
        painter->setPen(option.palette.highlightedText().color());
    }
    else
    {
        painter->setPen(option.palette.text().color());
    }
    painter->drawText(option.rect.adjusted(22,1,-1,-1),index.model()->data(index.model()->index(index.row(),index.column()+1)).toString());
}

bool SpeakerDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type()==QMouseEvent::MouseButtonPress)
    {
        QRect imgrect(option.rect.adjusted(1,1,-1,-1));
        imgrect.setWidth(imgrect.height());
        if (imgrect.contains(((QMouseEvent*)event)->pos()))
        {
            model->setData(index,!model->data(index).toUInt());
            return true;
        }
    }
    return QStyledItemDelegate::editorEvent(event,model,option,index);
}

QSize SpeakerDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(-1,18);
}

CXMLVoiceModel::CXMLVoiceModel(QObject *parent)
{
    m_Score=0;
    m_Staff=0;
}

void CXMLVoiceModel::setXML(XMLScoreWrapper *Score)
{
    m_Score=Score;
}

void CXMLVoiceModel::setStaff(int Staff)
{
    m_Staff=Staff;
    reset();
}

int CXMLVoiceModel::rowCount(const QModelIndex & /*parent*/) const
{
    if (m_Score)
    {
        return m_Score->NumOfVoices(m_Staff);
    }
    return 0;
}

int CXMLVoiceModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 1;
}

void CXMLVoiceModel::reset()
{
    emit dataChanged(index(0,0),index(rowCount()-1,columnCount()-1));
}

QVariant CXMLVoiceModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.column()==0)
        {
            return QString("Voice %1").arg(index.row() + 1);
        }
    }
    return QVariant();
}

bool CXMLVoiceModel::removeRow(int row, const QModelIndex & /*parent*/)
{
    m_Score->DeleteVoice(m_Staff,row);
    emit dataChanged(index(0,0),index(rowCount()-1,columnCount()-1));
    return true;
}

bool CXMLVoiceModel::insertRow(int row, const QModelIndex & /*parent*/)
{
    m_Score->AddVoice(m_Staff,row);
    emit dataChanged(index(0,0),index(rowCount()-1,columnCount()-1));
    return true;
}

CXMLStaffModel::CXMLStaffModel(QObject *parent)
{
    m_Score=0;
}

void CXMLStaffModel::setXML(XMLScoreWrapper *Score)
{
    m_Score=Score;
    reset();
}

int CXMLStaffModel::rowCount(const QModelIndex & /*parent*/) const
{
    if (m_Score==0) return 0;
    return m_Score->NumOfStaffs();
}

int CXMLStaffModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 3;
}

QVariant CXMLStaffModel::data(const QModelIndex &index, int role) const
{
    if ((role == Qt::EditRole) | (role == Qt::DisplayRole))
    {
        switch  (index.column())
        {
        case 0:
            return !(bool)(m_Score->Staff(index.row())->attributeValue("Muted"));
        case 1:
            return m_Score->StaffName(index.row());
        case 2:
            return m_Score->StaffAbbreviation(index.row());
        }
    }
    return QVariant();
}

Qt::ItemFlags CXMLStaffModel::flags(const QModelIndex & index) const
{
    if (index.isValid())
    {
        if (rowCount()>1)
        {
            return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled;
        }
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    else
    {
        return Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled  | Qt::ItemIsEnabled;
    }
}

Qt::DropActions CXMLStaffModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

bool CXMLStaffModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole)
    {
        switch  (index.column())
        {
        case 0:
            m_Score->Staff(index.row())->setAttribute("Muted",!(value.toUInt()));
            break;
        case 1:
            m_Score->setStaffName(index.row(),value.toString());
            break;
        case 2:
            m_Score->setStaffAbbreviation(index.row(),value.toString());
            break;
        }
        emit dataChanged(index,index);
    }
    return true;
}

bool CXMLStaffModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    QByteArray encodedData = data->data("text/plain");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QStringList newItems;
    while (!stream.atEnd()) {
        QString text;
        stream >> text;
        newItems << text;
    }
    for (int i=newItems.count()-1;i>=0;i--)
    {
        QString text=newItems[i];
        lastDropped=index(row,0);
        m_Score->MoveStaff(text.toUInt(),row);
    }
    emit dataChanged(index(0,0),index(rowCount()-1,columnCount()-1));
    return true;
}

QMimeData* CXMLStaffModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData * mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes) {
        stream << QString::number(index.row());
    }

    mimeData->setData("text/plain", encodedData);
    return mimeData;
}

QStringList CXMLStaffModel::mimeTypes() const
{
    return QStringList("text/plain");
}

void CXMLStaffModel::reset()
{
    emit dataChanged(index(0,0),index(rowCount()-1,columnCount()-1));
}

CStaffsDialog::CStaffsDialog(QWidget *parent) : QDialog(parent,Qt::Sheet),
    ui(new Ui::CStaffsDialog)
{
    ui->setupUi(this);
    m_XMLVoiceModel=new CXMLVoiceModel(this);
    ui->VoiceList->setModel(m_XMLVoiceModel);
    m_XMLStaffModel=new CXMLStaffModel(this);
    ui->StaffList->setModel(m_XMLStaffModel);
    m_XMLScoreModel=new DomAttributesModel(this);
    m_XMLScoreModel->addColumn(0,"BarNrOffset");
    m_XMLScoreModel->addColumn(1,"NoteSpace");
    m_XMLScoreModel->addColumn(2,"DontShowBN");
    m_XMLScoreModel->addColumn(3,"MasterStave");
    ui->StaffList->setModelColumn(0);
    ui->StaffList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->StaffList->setDragEnabled(true);
    ui->StaffList->setDragDropMode(QListView::InternalMove);
    ui->StaffList->setAcceptDrops(true);
    ui->StaffList->setDropIndicatorShown(true);
    scoreMapper = new QDataWidgetMapper(this);
    scoreMapper->setModel(m_XMLScoreModel);
    scoreMapper->addMapping(ui->HideBarNumbersCheckbox,2);
    scoreMapper->addMapping(ui->BarNumberOffsetSpinBox,0);
    scoreMapper->addMapping(ui->NoteSpaceSlider,1);
    scoreMapper->addMapping(ui->MasterStaffComboBox,3,"currentIndex");
    staffMapper = new QDataWidgetMapper(this);
    staffMapper->setModel(m_XMLStaffModel);
    staffMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    staffMapper->addMapping(ui->AudibleCheckBox, 0);
    staffMapper->addMapping(ui->NameEdit, 1);
    staffMapper->addMapping(ui->AbbreviationEdit, 2);
    ui->MasterStaffComboBox->setModel(m_XMLStaffModel);
    ui->MasterStaffComboBox->setModelColumn(1);
    connect(ui->AudibleCheckBox,SIGNAL(clicked()),staffMapper,SLOT(submit()));
    connect(ui->AudibleCheckBox,SIGNAL(pressed()),staffMapper,SLOT(submit()));
    connect(ui->NameEdit,SIGNAL(textEdited(QString)),staffMapper,SLOT(submit()));
    connect(ui->AbbreviationEdit,SIGNAL(textEdited(QString)),staffMapper,SLOT(submit()));
    setWindowModality(Qt::WindowModal);
    setVisible(false);
    connect(ui->StaffList->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(UpdateStaff(QModelIndex)));
    connect(ui->StaffList->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),staffMapper,SLOT(setCurrentModelIndex(QModelIndex)));
    connect(ui->NoteSpaceSlider,SIGNAL(valueChanged(int)),this,SLOT(SpacingTooltip(int)));
    InternalMoveEvent* Move=new InternalMoveEvent();
    ui->StaffList->installEventFilter(Move);
    connect(Move,SIGNAL(ItemMoved()),this,SLOT(StaffDropped()));
    ui->StaffList->setItemDelegate(new SpeakerDelegate(this));
}

int CStaffsDialog::ShowModal()
{
    show();
    while (this->isVisible())
    {
        QApplication::processEvents();
    }
    return result();
}

void CStaffsDialog::Fill(XMLScoreWrapper &Doc)
{
    XMLScore.setCopy(Doc);
    ui->TempoBox->Fill(XMLScore.TempoFont(),"a tempo.",true,Qt::AlignCenter,0.1);
    ui->DynamicBox->Fill(XMLScore.DynamicFont(),"cresc.",true,Qt::AlignCenter,0.1);
    m_XMLStaffModel->setXML(&XMLScore);
    ui->MasterStaffComboBox->setCurrentIndex(0);
    m_XMLVoiceModel->setXML(&XMLScore);
    m_XMLScoreModel->setXML(XMLScore.Score());
    scoreMapper->toFirst();
    Update();
}

QDomLiteDocument* CStaffsDialog::CreateXML()
{
    scoreMapper->submit();
    XMLScore.setDynamicFont(ui->DynamicBox->Font());
    XMLScore.setTempoFont(ui->TempoBox->Font());
    return XMLScore.getClone();
}

CStaffsDialog::~CStaffsDialog()
{
    delete ui;
    delete m_XMLVoiceModel;
    delete m_XMLStaffModel;
    delete m_XMLScoreModel;
}

void CStaffsDialog::Update()
{
    ui->RemoveStaff->setEnabled(m_XMLStaffModel->rowCount()>1);
    m_XMLStaffModel->reset();
    if (ui->StaffList->currentIndex().row()<0) ui->StaffList->setCurrentIndex(m_XMLStaffModel->index(0,0));
    SpacingTooltip(ui->NoteSpaceSlider->value());
}

void CStaffsDialog::UpdateStaff(QModelIndex index)
{
    if (index.row()<0) return;
    m_XMLVoiceModel->setStaff(index.row());
    if (ui->VoiceList->currentIndex().row()<0) ui->VoiceList->setCurrentIndex(m_XMLVoiceModel->index(0,0));
    if (ui->VoiceList->currentIndex().row()>=m_XMLVoiceModel->rowCount()) ui->VoiceList->setCurrentIndex(m_XMLVoiceModel->index(m_XMLVoiceModel->rowCount()-1,0));
    ui->RemoveVoice->setEnabled(m_XMLVoiceModel->rowCount()>1);
}

void CStaffsDialog::StaffDropped()
{
    ui->StaffList->setCurrentIndex(m_XMLStaffModel->lastDropped);
}

void CStaffsDialog::AddStaff()
{
    QStringList Names;
    for (int i=0;i<XMLScore.NumOfStaffs();i++)
    {
        Names.append(XMLScore.StaffName(i));
    }
    QString NewName="New Staff";
    int StaffCount=1;
    while (Names.contains(NewName))
    {
        NewName="New Staff - "+QString::number(StaffCount);
        StaffCount++;
    }
    XMLScore.AddStaff(m_XMLStaffModel->rowCount(),NewName);
    Update();
    ui->StaffList->setCurrentIndex(m_XMLStaffModel->index(m_XMLStaffModel->rowCount()-1,0));
}

void CStaffsDialog::DeleteStaff()
{
    int Staff=ui->StaffList->currentIndex().row();
    QDomLiteElement* XMLStaff=XMLScore.Staff(Staff);
    int SymbolCount=0;
    QString Msg;
    for (int i=0;i<XMLStaff->childCount();i++)
    {
        QDomLiteElement* XMLVoice=XMLScore.Voice(Staff,i);
        SymbolCount+=XMLVoice->childCount()-1;
    }
    if (SymbolCount)
    {
        Msg+="This Staff already includes "+QString::number(XMLStaff->childCount())+" Voices with "+QString::number(SymbolCount)+" Symbols!\n";
    }
    QStringList LayoutNames;
    QStringList SingleStaffLayoutNames;
    for (int i=0;i<XMLScore.NumOfLayouts();i++)
    {
        QDomLiteElement* LayoutTemplate=XMLScore.LayoutTemplate(i);
        foreach(QDomLiteElement* e,LayoutTemplate->childElements)
        {
            if (XMLScore.AllTemplateIndex(e)==Staff)
            {
                if (LayoutTemplate->childCount()==1)
                {
                    SingleStaffLayoutNames.append(XMLScore.LayoutName(i));
                }
                else
                {
                    LayoutNames.append(XMLScore.LayoutName(i));
                }
            }
        }
    }
    if (SingleStaffLayoutNames.count())
    {
        Msg+="The following Layouts will be deleted:\n";
        foreach(QString s,SingleStaffLayoutNames) Msg+=s+"\n";
    }
    if (LayoutNames.count())
    {
        Msg+="The Staff will be deleted from the following Layouts:\n";
        foreach(QString s,LayoutNames) Msg+=s+"\n";
    }
    if (Msg.length())
    {
        if (QMessageBox::warning(this, "Object Composer XML",Msg,QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok)==QMessageBox::Cancel) return;
    }
    foreach(QString s,SingleStaffLayoutNames) XMLScore.LayoutCollection()->removeChild(XMLScore.Layout(s));
    foreach(QString s,LayoutNames)
    {
        QDomLiteElement* Layout=XMLScore.Layout(s);
        QDomLiteElementList Templates=Layout->elementsByTag("Template",true);
        foreach(QDomLiteElement* t,Templates)
        {
            foreach(QDomLiteElement* e,t->childElements) if (XMLScore.AllTemplateIndex(e)==Staff) t->removeChild(e);
            XMLScore.UpdateIndexes(t);
        }
    }
    XMLScore.DeleteStaff(Staff);
    if (Staff>=m_XMLStaffModel->rowCount()) Staff=m_XMLStaffModel->rowCount()-1;
    Update();
    ui->StaffList->setCurrentIndex(m_XMLStaffModel->index(Staff,0));
}

void CStaffsDialog::AddVoice()
{
    m_XMLVoiceModel->insertRow(m_XMLVoiceModel->rowCount(),m_XMLVoiceModel->index(-1,-1));
    ui->VoiceList->setCurrentIndex(m_XMLVoiceModel->index(m_XMLVoiceModel->rowCount()-1,0));
    ui->RemoveVoice->setEnabled(m_XMLVoiceModel->rowCount()>1);
}

void CStaffsDialog::DeleteVoice()
{
    int Voice=ui->VoiceList->currentIndex().row();
    m_XMLVoiceModel->removeRow(Voice,m_XMLVoiceModel->index(-1,-1));
    if (Voice>=m_XMLVoiceModel->rowCount()) Voice=m_XMLVoiceModel->rowCount()-1;
    ui->VoiceList->setCurrentIndex(m_XMLVoiceModel->index(Voice,0));
    ui->RemoveVoice->setEnabled(m_XMLVoiceModel->rowCount()>1);
}

void CStaffsDialog::SpacingTooltip(int Value)
{
    int v=qRound(((float)(Value-8)/25.0)*9);
    QString retval;
    switch (v)
    {
    case 9:
        retval=QString("Extremely Wide");
        break;
    case 8:
        retval=QString("Very Wide");
        break;
    case 7:
        retval=QString("Wide");
        break;
    case 6:
        retval=QString("Quite Wide");
        break;
    case 5:
        retval=QString("Slightly Wide");
        break;
    case 4:
        retval=QString("Normal");
        break;
    case 3:
        retval=QString("Slightly Narrow");
        break;
    case 2:
        retval=QString("Quite Narrow");
        break;
    case 1:
        retval=QString("Very Narrow");
        break;
    case 0:
        retval=QString("Extremely Narrow");
        break;
    }
    retval+=" ("+QString::number((float)(Value-16)/25.0)+")";
    ui->SpacingLabel->setText(retval);
}
