#ifndef CSTAFFSDIALOG_H
#define CSTAFFSDIALOG_H

//#include <QStyledItemDelegate>
#include "CommonClasses.h"
//#include <QDialog>
#include <QDataWidgetMapper>

namespace Ui {
class CStaffsDialog;
}
/*
class SpeakerDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    SpeakerDelegate(QWidget *parent = nullptr) : QStyledItemDelegate(parent) {}
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
};
*/
class CXMLVoiceModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    XMLScoreWrapper* m_Score;
    int m_Staff;
public:
    CXMLVoiceModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void setXML(XMLScoreWrapper* Score);
    void setStaff(int Staff);
    void reset();
    bool removeRow(int row, const QModelIndex &parent);
    bool insertRow(int row, const QModelIndex &parent);
};

class CXMLStaffModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    XMLScoreWrapper* m_Score;
public:
    CXMLStaffModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    void setXML(XMLScoreWrapper* Score);
    Qt::ItemFlags flags(const QModelIndex & index) const;
    Qt::DropActions supportedDropActions() const;
    void reset();
    QModelIndex lastDropped;
signals:
    void editCompleted(const QString &);
protected:
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    QMimeData* mimeData(const QModelIndexList &indexes) const;
    QStringList mimeTypes() const;
};

class CStaffsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CStaffsDialog(QWidget *parent = nullptr);
    ~CStaffsDialog();
    int ShowModal();
    void Fill(XMLScoreWrapper& Doc);
    QDomLiteDocument* CreateXML();
private:
    Ui::CStaffsDialog *ui;
    QDataWidgetMapper* staffMapper;
    QDataWidgetMapper* scoreMapper;
    CXMLVoiceModel* m_XMLVoiceModel;
    CXMLStaffModel* m_XMLStaffModel;
    DomAttributesModel* m_XMLScoreModel;
private slots:
    void StaffDropped();
    void Update();
    void UpdateStaff(QModelIndex index);
    void AddVoice();
    void DeleteVoice();
    void AddStaff();
    void DeleteStaff();
    void SpacingTooltip(int Value);
private:
    XMLScoreWrapper XMLScore;
};

#endif // CSTAFFSDIALOG_H
