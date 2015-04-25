#include "basemodel.h"

BaseModel::BaseModel(QMap<QString, TreeItem *> *base, QObject *parent) :
    m_base(base),
    QAbstractItemModel(parent)
{
}

void BaseModel::appendNewBase()
{
    QString key = "NAZWA_0";
    int x = 0;
    QList<QString> k;

    if(!m_base->isEmpty())
        k = m_base->keys();

    while(k.contains(key))
    {
        key = key.left(6) + QString::number(++x);
    }

    beginInsertRows(QModelIndex(), k.size(), k.size());
        m_base->insert(key, nullptr);
    endInsertRows();
}

void BaseModel::removeBase(QModelIndex &index)
{
    if(index.isValid())
    {
        int row = index.row();
        TreeItem* item = reinterpret_cast<TreeItem*>(index.internalPointer());
        QString key = this->index(row, 0, index.parent()).data().toString();

        beginRemoveRows(index.parent(), row, row);
        m_base->remove(key);
        endRemoveRows();
        delete item;
    }
}

void BaseModel::refresh()
{
    QList<QString> keys = m_base->keys();
    QList<QString>::iterator iter;
    for(iter = keys.begin(); iter != keys.end(); iter++)
    {
        QString key = *iter;
        TreeItem* old = m_base->value(key);
        if(old != nullptr)
        {
            m_base->operator [](key) = createDirTree(old->data(0).toString(), old->parent());
            delete old;
        }
    }
}

int BaseModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return m_base->size();
}

int BaseModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QModelIndex BaseModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!parent.isValid() && !m_base->isEmpty())
    {
        QList<QString> k = m_base->keys();
        TreeItem* item = m_base->value(k.at(row));
        return createIndex(row, column, item);
    }

    return QModelIndex();
}

QModelIndex BaseModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

QVariant BaseModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    int r = index.row();

    if(r < 0 || r >= m_base->size())
        return QVariant();

    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if(index.column() == 0 && !m_base->isEmpty())
        {
            QList<QString> k = m_base->keys();
            return QVariant(k.at(r));
        }
        else if(index.column() == 1)
        {
            TreeItem* item = reinterpret_cast<TreeItem*>(index.internalPointer());
            if(item != nullptr)
                return item->data(0);
        }
    }

    return QVariant();
}

Qt::ItemFlags BaseModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return QAbstractItemModel::flags(index);

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool BaseModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;
    int c = index.column();
    int r = index.row();

    if(role == Qt::EditRole)
    {
        if(c == 0 && !m_base->isEmpty())
        {
            QList<QString> k = m_base->keys();
            if(!k.contains(value.toString()))
            {
                TreeItem* item = m_base->take(index.data().toString());
                m_base->insert(value.toString(), item);
                return true;
            }
        }
        else if(c == 1 && !m_base->isEmpty())
        {
            QString key = this->index(r, 0, index.parent()).data().toString();
            TreeItem* old = m_base->value(key);
            if(old != nullptr)
            {
                m_base->operator [](key) = createDirTree(value.toString(), old->parent());
                delete old;
            }
            else
                m_base->operator [](key) = createDirTree(value.toString(), nullptr);

            return true;
        }
    }
    return false;
}

QVariant BaseModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    if(orientation == Qt::Horizontal)
    {
        if(section == 0)
            return QString("Nazwa");
        if(section == 1)
            return QString("Ścieżka");
    }

    return QVariant();
}
