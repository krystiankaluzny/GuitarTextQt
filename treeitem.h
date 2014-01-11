#ifndef TREEITEM_H
#define TREEITEM_H
#include <QList>
#include <QVector>
#include <QVariant>

class TreeItem
{
public:
    TreeItem(const QVector<QVariant> &data, bool is_dir = false, TreeItem *parent = 0);
    ~TreeItem();

    void appendChild(TreeItem* child);

    TreeItem *child(int row);
    QList<TreeItem *> getChildren() const;
    void deleteChildren();
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    TreeItem* parent() const;
    void setParent(TreeItem* parent);
    bool insertChildren(int position, int count, int columns);
    bool removeChildren(int position, int count);
    bool removeChild(QList<TreeItem*>::iterator iter);
    bool clearChildren(int position, int count);
    void clearChildren();
    bool insertColumns(int position, int columns);
    bool removeColumns(int position, int columns);
    int childNumber() const;
    bool setData(int column, const QVariant& value);
    bool setDownTree(int column, const QVariant &value);
    void setDir(bool dir);
    bool isDir() const;

private:
    bool is_dir;
    QList<TreeItem*> childItems;
    QVector<QVariant> itemData;
    TreeItem* parentItem;
};

TreeItem* createDirTree(const QString &dir, TreeItem *parent);
#endif // TREEITEM_H
