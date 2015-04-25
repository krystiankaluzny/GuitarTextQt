#include "searchdialog.h"
#include "ui_searchdialog.h"
#include "mysharedata.h"

void Print(int** t, int k, int m)
{
    QString str;
    for(int i = 0; i < k; i++)
    {
        str.clear();
        for(int j = 0; j < m; j++)
            str = str + QString::number(t[i][j]) + " ";
    }
}

SearchDialog::SearchDialog(QWidget *parent) :
    AutoHidingDialog(parent),
    ui(new Ui::SearchDialog),
    m_key_model(nullptr),
    m_dir_model(nullptr),
    m_item_selection_model(nullptr)
{
    ui->setupUi(this);

    resetModels();

    ui->treeView_base->setDragEnabled(true);

    //------------Wypełnianie boxa z kodowaniami------------
    ui->comboBox_encoder->clear();
    ui->comboBox_encoder->addItem("UTF-8");
    ui->comboBox_encoder->addItem("UTF-16");
    ui->comboBox_encoder->addItem("Windows-1250");
    ui->comboBox_encoder->addItem("ISO 8859-1");
    ui->comboBox_encoder->addItem("ISO 8859-2");

    if(!MyShareData::m_encode.isEmpty())
        ui->comboBox_encoder->setCurrentText(MyShareData::m_encode);

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(newFocus(QWidget*,QWidget*)));
}

SearchDialog::~SearchDialog()
{
    delete ui;
}

void SearchDialog::setCurrentEncode(const QString &arg1)
{
    ui->comboBox_encoder->setCurrentText(arg1);
}

void SearchDialog::resetModels()
{
    QStringList headers;
    headers << "Full Path" << "Name" << "Last Modification" << "Synchronize" << "Priority";

    if(m_dir_model) delete m_dir_model;
    m_dir_model = new MyDirModel(headers);
    m_dir_model->setColumnCount(1);
    m_dir_model->canDelete(false);
    QItemSelectionModel *m = ui->treeView_base->selectionModel();
    ui->treeView_base->setModel(m_dir_model);
    delete m;

    ui->comboBox->clear();
    if(m_key_model) delete m_key_model;
    m_key_model = new KeyBaseModel(&MyShareData::m_text_bases);
    ui->comboBox->setModel(m_key_model);
}

void SearchDialog::newFocus(QWidget* old, QWidget* now)
{
    QObjectList c = this->children();
    QObjectList c2 = this->parent()->children();
    if(old != nullptr)
        if(c2.contains(old->parent()) && old->parent() != this)
            if(c.contains(now))
                ui->lineEdit->setFocus();
}

void SearchDialog::on_comboBox_currentIndexChanged(const QString &arg1)
{
    int rows_count = m_dir_model->rowCount();
    m_dir_model->removeRows(0, rows_count);
    m_dir_model->appendItem(MyShareData::m_text_bases.value(arg1));
    ui->lineEdit->clear();
}

void SearchDialog::FindFiles(TreeItem *item, const QString &t)
{
    if(item == nullptr) return;
    if(item->isDir())
    {
        QList<TreeItem*> children = item->getChildren();
        QList<TreeItem*>::iterator iter;
        for(iter = children.begin(); iter != children.end(); iter++)
            FindFiles(*iter, t);
    }
    else
    {
        QString s = item->data(1).toString().toLower();
        if(s.contains(t))
            m_found_files.append(item);
    }
}

void SearchDialog::on_lineEdit_textChanged(const QString &arg1)
{
    ui->listWidget->clear();
    QString text = arg1;
    if(!text.isEmpty())
    {
        TreeItem* item = MyShareData::m_text_bases.value(ui->comboBox->currentText());

        m_found_files.clear();
        FindFiles(item, text.toLower());

        for(int i = 0; i < m_found_files.size(); i++)
        {
            TreeItem* it = m_found_files.at(i);
            QListWidgetItem *item = new QListWidgetItem;
            QStringList strlist = it->data(0).toString().split('/');
            QString parent_dir = strlist.size() >= 2 ? strlist.at(strlist.size() - 2) : "";
            item->setText(it->data(1).toString() + " - " + parent_dir);
            item->setToolTip(it->data(0).toString());
            item->setData(Qt::WhatsThisRole, it->data(0).toString());
            ui->listWidget->addItem(item);
        }
    }
}

void SearchDialog::on_listWidget_activated(const QModelIndex &index)
{
    QString path = index.data(5).toString();
    emit doubleClicked(path);
}

void SearchDialog::on_treeView_base_activated(const QModelIndex &index)
{
    QString path;
    TreeItem* item = reinterpret_cast<TreeItem*>(index.internalPointer());
    if(item)
        if(!item->isDir())
        {
            path = item->data(0).toString();
            emit doubleClicked(path);
        }
}

void SearchDialog::on_comboBox_encoder_activated(const QString &arg1)
{
    emit encodeChanged(arg1);
}
