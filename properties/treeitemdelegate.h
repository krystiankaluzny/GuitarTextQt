#ifndef TREEITEMDELEGAT_H
#define TREEITEMDELEGAT_H

#include <QItemDelegate>

class TreeItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit TreeItemDelegate(QObject *parent = 0);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

signals:

public slots:

};

#endif // TREEITEMDELEGAT_H
