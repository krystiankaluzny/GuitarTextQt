#ifndef BASEMODEL_H
#define BASEMODEL_H

#include <QAbstractItemModel>
#include "treeitem.h"

class BaseModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit BaseModel(QMap<QString, TreeItem*>* base, QObject *parent = 0);
    void appendNewBase();
    void removeBase(QModelIndex& index);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

signals:

public slots:

private:
    QMap<QString, TreeItem*>* m_base;
};

#endif // BASEMODEL_H
