#include "treeitemdelegate.h"
#include <QFileDialog>

TreeItemDelegate::TreeItemDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QWidget *TreeItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QFileDialog* dialog = new QFileDialog(parent);
    dialog->setFileMode(QFileDialog::DirectoryOnly);

    return dialog;
}

void TreeItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QFileDialog* dialog = static_cast<QFileDialog*>(editor);

    int x = dialog->x();
    int y = dialog->y();
    dialog->setGeometry(x, y, 500, 300);

    if(index.isValid())
    {
        if(index.data().isValid())
            dialog->setDirectory(index.data().toString());
        else
            dialog->setDirectory("/home");
    }

}

void TreeItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QFileDialog* dialog = static_cast<QFileDialog*>(editor);

    QString path = dialog->selectedFiles().at(0);

    model->setData(index, path, Qt::EditRole);
}
