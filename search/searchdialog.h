#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QItemSelectionModel>
#include "mydirmodel.h"
#include "keybasemodel.h"
#include "autohidingdialog.h"

namespace Ui {
class SearchDialog;
}

class SearchDialog : public AutoHidingDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(QWidget *parent = 0);
    ~SearchDialog();

    void setCurrentEncode(const QString& arg1);

    void resetModels();

signals:
    void doubleClicked(QString path);

    void encodeChanged(QString encode);

private slots:
    void newFocus(QWidget* old, QWidget* now);

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_lineEdit_textChanged(const QString &arg1);

    void on_listWidget_activated(const QModelIndex &index);

    void on_treeView_base_activated(const QModelIndex &index);

    void on_comboBox_encoder_activated(const QString &arg1);

private:
    void FindFiles(TreeItem* item, const QString& t);

    Ui::SearchDialog *ui;
    KeyBaseModel* m_key_model;
    MyDirModel* m_dir_model;
    QItemSelectionModel* m_item_selection_model;

    QVector<TreeItem*> m_found_files;
};

#endif // SEARCHDIALOG_H
