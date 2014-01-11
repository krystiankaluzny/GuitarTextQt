#ifndef KEYBASEMODEL_H
#define KEYBASEMODEL_H

#include <QAbstractItemModel>
#include "treeitem.h"

class KeyBaseModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit KeyBaseModel(QMap<QString, TreeItem*>* base, QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    QVariant data(const QModelIndex &index, int role) const;
signals:

public slots:

private:
    QMap<QString, TreeItem*>* m_base;
};

#endif // KEYBASEMODEL_H
