#include "keybasemodel.h"

KeyBaseModel::KeyBaseModel(QMap<QString, TreeItem *> *base, QObject *parent) :
    QAbstractItemModel(parent),
    m_base(base)
{
}

int KeyBaseModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return m_base->size();
}

int KeyBaseModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QModelIndex KeyBaseModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!parent.isValid())
    {
        QList<QString> k = m_base->keys();
        return createIndex(row, column);
    }
    return QModelIndex();
}

QModelIndex KeyBaseModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

QVariant KeyBaseModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    int r = index.row();
    if(r < 0 || r >= m_base->size())
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        QList<QString> k = m_base->keys();
        return QVariant(k.at(r));
    }
    return QVariant();
}
