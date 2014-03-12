#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include "mydirmodel.h"
#include "keybasemodel.h"

namespace Ui {
class SearchDialog;
}

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(QMap<QString, TreeItem *> *m_text_bases, QWidget *parent = 0);
    ~SearchDialog();

signals:
    void doubleClicked(QString path);

private slots:
    void newFocus(QWidget* old, QWidget* now);

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_lineEdit_textChanged(const QString &arg1);

    void on_listWidget_activated(const QModelIndex &index);

    void on_treeView_base_activated(const QModelIndex &index);

private:
    void FindFiles(TreeItem* item, const QString& t);

    Ui::SearchDialog *ui;
    KeyBaseModel* m_key_model;
    MyDirModel* m_dir_model;

    QMap<QString, TreeItem *> *m_bases;
    QVector<TreeItem*> m_found_files;
};

#endif // SEARCHDIALOG_H
