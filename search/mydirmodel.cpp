#include "mydirmodel.h"
#include <QDir>
#include <QFileInfoList>
#include <QDateTime>
#include <QColor>
#include <QIcon>
#include <QUrl>

MyDirModel::MyDirModel(const QStringList &headers, QStringList data, int column_count, QObject *parent)
    : QAbstractItemModel(parent),
      m_can_delete(true),
      m_column_count(column_count)
{
    QVector<QVariant> rootData;
    for(QString header : headers)
        rootData << header;

    rootItem = new TreeItem(rootData);
    for(int i = 0; i < data.size(); i++)
        createDirTree(data.at(i), rootItem);
}

MyDirModel::~MyDirModel()
{
    if(!m_can_delete)
        rootItem->clearChildren();
    delete rootItem;
}

int MyDirModel::columnCount(const QModelIndex &parent) const
{
    return m_column_count > 0 ? m_column_count : rootItem->columnCount() - 1;
}

QVariant MyDirModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(role == Qt::DecorationRole && index.column() == 0)
    {
        bool is_dir = reinterpret_cast<TreeItem*>(index.internalPointer())->isDir();
        QIcon icon;
        if(is_dir)
            icon.addFile(":/icon/folder.png");
        else
            icon.addFile(":/icon/text-plain.png");
        return icon;
    }

    if(role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    TreeItem* item = getItem(index);

    return item->data(index.column() + 1);
}

bool MyDirModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role != Qt::EditRole)
        return false;

    TreeItem* item = getItem(index);
    bool result;

    if(index.column() == 2 || index.column() == 3)
        result = item->setDownTree(index.column() + 1, value);
    else
        result = item->setData(index.column() + 1, value);


    if(result)
        emit dataChanged(index, index);

    return result;
}

Qt::ItemFlags MyDirModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return 0;

    int c = index.column();

    if(c == 0)
        return Qt::ItemIsDragEnabled | QAbstractItemModel::flags(index);

    if(c == 2 || c == 3)
        return Qt::ItemIsEditable | QAbstractItemModel::flags(index);

    return QAbstractItemModel::flags(index);
}

TreeItem *MyDirModel::getItem(const QModelIndex &index) const
{
    if(index.isValid())
    {
        TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
        if(item)
            return item;
    }
    return rootItem;
}

QVariant MyDirModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section + 1);

    return QVariant();
}

bool MyDirModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if(role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    bool result = rootItem->setData(section, value);

    if(result)
        emit headerDataChanged(orientation, section, section);

    return result;
}

QModelIndex MyDirModel::index(int row, int column, const QModelIndex &parent) const
{
    if(parent.isValid() && parent.column() != 0)
        return QModelIndex();

    TreeItem* parentItem = getItem(parent);

    TreeItem* childItem = parentItem->child(row);

    if(childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

QModelIndex MyDirModel::parent(const QModelIndex &child) const
{
    if(!child.isValid())
        return QModelIndex();

    TreeItem* childItem = getItem(child);
    TreeItem* parentItem = childItem->parent();

    if(parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool MyDirModel::insertRows(int row, int count, const QModelIndex &parent)
{
    TreeItem* parentItem = getItem(parent);
    bool success;

    beginInsertRows(parent, row, row + count - 1);
    success = parentItem->insertChildren(row, count, rootItem->columnCount());
    endInsertRows();

    return success;
}

bool MyDirModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if(count < 1) return false;

    TreeItem* parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, row, row + count - 1);
    if(m_can_delete)
        success = parentItem->removeChildren(row, count);
    else
        success = parentItem->clearChildren(row, count);
    endRemoveRows();

    return success;
}

TreeItem* MyDirModel::appendNewTree(QString &path)
{
    TreeItem* tree;
    beginResetModel();
    tree = createDirTree(path, rootItem);
    endResetModel();

    return tree;
}

QList<TreeItem*> MyDirModel::getTrees() const
{
    return rootItem->getChildren();
}

void MyDirModel::setColumnCount(int count)
{
    beginResetModel();
    m_column_count = count;
    endResetModel();
}

void MyDirModel::appendItem(TreeItem *item)
{
    if(item == nullptr) return;
    item->setParent(rootItem);
    rootItem->appendChild(item);
}

void MyDirModel::canDelete(bool can)
{
    m_can_delete = can;
}

QMimeData* MyDirModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData* mimeD = new QMimeData();
    if(!indexes.isEmpty())
    {
        TreeItem* address = reinterpret_cast<TreeItem*>(indexes.at(0).internalPointer());
//        QByteArray encodedData(address->data(0).toByteArray());
//        mimeD->setData("application/treeitem.ptr", encodedData);
        mimeD->setUrls(QList<QUrl>() << address->data(0).toUrl());
    }

    return mimeD;
}

int MyDirModel::rowCount(const QModelIndex &parent) const
{
    TreeItem* parentItem = getItem(parent);
    return parentItem->childCount();
}
