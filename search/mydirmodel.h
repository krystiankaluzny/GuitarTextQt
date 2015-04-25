#ifndef MYDIRMODEL_H
#define MYDIRMODEL_H

#include <QAbstractItemModel>
#include <QStringList>
#include <QMimeData>
#include "treeitem.h"

class MyDirModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    MyDirModel(const QStringList &headers, QStringList data = QStringList(), int column_count = 0, QObject *parent = 0);
    ~MyDirModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    TreeItem *appendNewTree(QString& path);
    QList<TreeItem *> getTrees() const;
    void setColumnCount(int count);
    void appendItem(TreeItem* item);
    void canDelete(bool can);
    QMimeData* mimeData(const QModelIndexList &indexes) const;

signals:

public slots:

private:
    TreeItem *getItem(const QModelIndex& index) const;

    TreeItem* rootItem;
    bool m_can_delete; //należy ustawić na fałsz jeżeli drzewa są tworzone na zewnątrz modelu
    int m_column_count;
};

#endif // MYDIRMODEL_H
