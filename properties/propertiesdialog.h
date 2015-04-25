#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H

#include <QDialog>
#include <QTextDocument>
#include "treeitem.h"
#include "basemodel.h"
#include "treeitemdelegate.h"

namespace Ui {
class PropertiesDialog;
}

class PropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PropertiesDialog(QWidget *parent = 0);
    ~PropertiesDialog();

    int m_chords;
    int m_shift;
    QString m_encode;
    int m_column;
    bool m_fullscreen;
    int m_max_last_open;
    int m_max_favourites;
    QColor m_text;
    QColor m_background;
    QColor m_chords_color;
    bool m_auto_column;
    QFont m_font;

private slots:

    void on_spinBox_chords_valueChanged(int arg1);

    void on_spinBox_shift_valueChanged(int arg1);

    void on_comboBox_encoding_currentIndexChanged(const QString &arg1);

    void on_spinBox_column_valueChanged(int arg1);

    void on_checkBox_fullscreen_clicked(bool checked);

    void on_spinBox__max_last_open_valueChanged(int arg1);

    void on_pushButton_text_color_clicked();

    void on_pushButton_background_color_clicked();

    void on_pushButton_domyslne_clicked();

    void on_pushButton_add_base_clicked();

    void on_pushButton_delete_base_clicked();

    void on_checkBox_auto_column_stateChanged(int arg1);

    void on_pushButton_chords_color_3_clicked();

    void on_pushButton_refresh_clicked();

    void on_spinBox__max_favourites_valueChanged(int arg1);

    void on_pushButton_font_clicked();

    void on_buttonBox_accepted();

private:
    Ui::PropertiesDialog *ui;
    QTextDocument* doc;

    BaseModel* m_base_model;
    TreeItemDelegate* delegate;
};

#endif // PROPERTIESDIALOG_H
