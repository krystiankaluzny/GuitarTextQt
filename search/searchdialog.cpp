#include "searchdialog.h"
#include "ui_searchdialog.h"
#include <QDebug>

inline int Min(int a, int b, int c)
{
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

void Print(int** t, int k, int m)
{
    QString str;
    for(int i = 0; i < k; i++)
    {
        str.clear();
        for(int j = 0; j < m; j++)
            str = str + QString::number(t[i][j]) + " ";

        qDebug() << str;
    }
}

SearchDialog::SearchDialog(QMap<QString, TreeItem *> *m_text_bases, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchDialog),
    m_bases(m_text_bases)
{
    ui->setupUi(this);

    QStringList headers;
    headers << "Full Path" << "Name" << "Last Modification" << "Synchronize" << "Priority";
    m_dir_model = new MyDirModel(headers);
    m_dir_model->setColumnCount(1);
    m_dir_model->canDelete(false);
    ui->treeView_base->setModel(m_dir_model);

    m_key_model = new KeyBaseModel(m_bases);
    ui->comboBox->setModel(m_key_model);

    ui->treeView_base->setDragEnabled(true);

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(newFocus(QWidget*,QWidget*)));
}

SearchDialog::~SearchDialog()
{
    delete ui;
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
    m_dir_model->removeRow(0);
    m_dir_model->appendItem(m_bases->value(arg1));
}

void SearchDialog::FindFiles(TreeItem *item, const QString &t)
{
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
    QString text = arg1;
    if(!text.isEmpty())
    {
        TreeItem* item = m_bases->value(ui->comboBox->currentText());
        ui->listWidget->clear();

        m_found_files.clear();
        FindFiles(item, text.toLower());

        for(int i = 0; i < m_found_files.size(); i++)
        {
            TreeItem* it = m_found_files.at(i);
            QListWidgetItem *item = new QListWidgetItem;
            item->setText(it->data(1).toString());
            item->setData(5, it->data(0).toString());
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
