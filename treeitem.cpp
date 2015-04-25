#include "treeitem.h"
#include <QFileInfo>
#include <QStringList>
#include <QDateTime>
#include <QDir>

TreeItem::TreeItem(const QVector<QVariant> &data, bool is_dir, TreeItem *parent)
    : is_dir(is_dir),
      itemData(data),
      parentItem(parent)
{
}

TreeItem::~TreeItem()
{
    deleteChildren();
}

void TreeItem::appendChild(TreeItem *child)
{
    childItems.append(child);
}

TreeItem *TreeItem::child(int row)
{
    return childItems.value(row);
}

QList<TreeItem *> TreeItem::getChildren() const
{
    return childItems;
}

void TreeItem::deleteChildren()
{
    qDeleteAll(childItems);
    childItems.clear();
}

int TreeItem::childCount() const
{
    return childItems.size();
}

int TreeItem::columnCount() const
{
    return itemData.size();
}

QVariant TreeItem::data(int column) const
{
    //"Full Path" << "Name" << "Last Modification" << "Synchronize" << "Priority";
    return itemData.value(column);
}

void TreeItem::setParent(TreeItem *parent)
{
    parentItem = parent;
}

int TreeItem::childNumber() const
{
    if(parentItem)
        return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));
    return 0;
}

TreeItem *TreeItem::parent() const
{
    return parentItem;
}

bool TreeItem::insertChildren(int position, int count, int columns)
{
    if(position < 0 || position > childItems.size())
        return false;

    for(int i = 0; i < count; i++)
    {
        QVector<QVariant> data(columns);
        TreeItem* item = new TreeItem(data, this);
        childItems.insert(position, item);
    }
    return true;
}

bool TreeItem::removeChildren(int position, int count)
{
    if(position < 0)
        return false;

    if(position + count > childItems.size())
        count = childItems.size() - position;

    for(int i = 0; i < count; i++)
    {
        delete childItems.takeAt(position); // usuń z listy a następnie usuń zawartość
    }
    return true;
}

bool TreeItem::removeChild(QList<TreeItem*>::iterator iter)
{
    return removeChildren(childItems.indexOf(*iter), 1);
}

bool TreeItem::clearChildren(int position, int count)
{
    if(position < 0 || position + count > childItems.size())
        return false;

    for(int i = 0; i < count; i++)
    {
        childItems.takeAt(position); // usuń z listy
    }
    return true;
}

void TreeItem::clearChildren()
{
    QList<TreeItem*>::iterator iter;
    for(iter = childItems.begin(); iter != childItems.end(); iter++)
        (*iter)->setParent(nullptr);
    childItems.clear(); //czyszczenie listy ale nie usuwanie obiektów
}

bool TreeItem::insertColumns(int position, int columns)
{
    if(position < 0 || position > itemData.size())
        return false;

    for(int i = 0; i < columns; i++)
        itemData.insert(position, QVariant());

    for(TreeItem* child : childItems)
        child->insertColumns(position, columns);

    return true;
}

bool TreeItem::removeColumns(int position, int columns)
{
    if(position < 0 || position + columns > itemData.size())
        return false;

    for(int i = 0; i < columns; i++)
        itemData.remove(position);

    for(TreeItem* child : childItems)
        child->removeColumns(position, columns);

    return true;
}

bool TreeItem::setData(int column, const QVariant &value)
{
    if(column < 0 || column >= itemData.size())
        return false;

    itemData[column] = value;

    return true;
}

bool TreeItem::setDownTree(int column, const QVariant &value)
{
    int result = !setData(column, value);

    QList<TreeItem*>::iterator iter;
    for(iter = childItems.begin(); iter != childItems.end(); iter++)
        result += !(*iter)->setDownTree(column, value);

    return !result;
}

void TreeItem::setDir(bool dir)
{
    is_dir = dir;
}

bool TreeItem::isDir() const
{
    return is_dir;
}

TreeItem* createDirTree(const QString &dir, TreeItem *parent)
{
    QFileInfo fileInfo(dir);

    if(fileInfo.isDir())
    {
        QVector<QVariant> data;
        data << fileInfo.absoluteFilePath() << fileInfo.fileName() << fileInfo.lastModified() << true << false;

        TreeItem* tree = new TreeItem(data, true, parent);

        QDir d(dir);
        QFileInfoList list = d.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDot | QDir::NoDotDot, QDir::Name | QDir::DirsFirst);

        for(int j = 0; j < list.size(); j++)
            createDirTree(list.at(j).absoluteFilePath(), tree);

        if(parent != nullptr)
            parent->appendChild(tree);

        return tree;
    }
    else if(fileInfo.isFile())
    {
        QVector<QVariant> data;
        data << fileInfo.absoluteFilePath() << fileInfo.fileName() << fileInfo.lastModified() << true << false;

        TreeItem* tree = new TreeItem(data, false, parent);

        if(parent != nullptr)
            parent->appendChild(tree);

        return tree;
    }

    return nullptr;
}
