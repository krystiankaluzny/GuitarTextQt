#include "propertiesdialog.h"
#include "ui_propertiesdialog.h"
#include <QColorDialog>


PropertiesDialog::PropertiesDialog(QSettings &settings, QMap<QString, TreeItem*>* m_text_bases, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PropertiesDialog),
    s(settings)
{
    ui->setupUi(this);

    m_base_model = new BaseModel(m_text_bases);

    delegate = new TreeItemDelegate;
    ui->tableView_bases->setModel(m_base_model);
    ui->tableView_bases->setItemDelegateForColumn(1, delegate);

    m_column = settings.value("editor/colimn_count", 3).toInt();
    ui->spinBox_column->setValue(m_column);

    m_speed = settings.value("editor/scrolling_speed", 8).toInt();
    ui->spinBox_speed->setValue(m_speed);

    m_fullscreen = settings.value("app/fullscreen", false).toBool();
    ui->checkBox_fullscreen->setChecked(m_fullscreen);

    ui->comboBox_encoding->addItem("UTF-8");
    ui->comboBox_encoding->addItem("UTF-16");
    ui->comboBox_encoding->addItem("Windows-1250");
    ui->comboBox_encoding->addItem("ISO 8859-1");
    ui->comboBox_encoding->addItem("ISO 8859-2");

    m_encode = settings.value("editor/code", "UTF-8").toString();
    ui->comboBox_encoding->setCurrentText(m_encode);

    m_chords = settings.value("editor/chords", 1).toInt();
    ui->spinBox_chords->setValue(m_chords);

    m_shift = settings.value("editor/shift", 0).toInt();
    ui->spinBox_shift->setValue(m_shift);

    m_max_last_open = settings.value("app/files_history_max_size", 10).toInt();
    ui->spinBox__max_last_open->setValue(m_max_last_open);

    QPalette palette = ui->pushButton_text_color->palette();
    m_text = settings.value("editor/text_color", QColor(255, 255, 255)).value<QColor>();
    palette.setColor(QPalette::Button, m_text);
    ui->pushButton_text_color->setPalette(palette);

    QPalette palette2 = ui->pushButton_background_color->palette();
    m_background = settings.value("editor/background_color", QColor(0, 0, 0)).value<QColor>();
    palette2.setColor(QPalette::Button, m_background);
    ui->pushButton_background_color->setPalette(palette2);
}

PropertiesDialog::~PropertiesDialog()
{
    delete ui;
}

void PropertiesDialog::on_spinBox_chords_valueChanged(int arg1)
{
    m_chords = arg1;
}

void PropertiesDialog::on_spinBox_shift_valueChanged(int arg1)
{
    m_shift = arg1;
}

void PropertiesDialog::on_comboBox_encoding_currentIndexChanged(const QString &arg1)
{
    m_encode = arg1;
}

void PropertiesDialog::on_spinBox_column_valueChanged(int arg1)
{
    m_column = arg1;
}

void PropertiesDialog::on_checkBox_fullscreen_clicked(bool checked)
{
    m_fullscreen = checked;
}

void PropertiesDialog::on_spinBox__max_last_open_valueChanged(int arg1)
{
    m_max_last_open = arg1;
}

void PropertiesDialog::on_pushButton_text_color_clicked()
{
    //text color
    QPalette textPalette = ui->pushButton_text_color->palette();
    QColor old = textPalette.color(QPalette::Button);
    QColor textColor = QColorDialog::getColor(old, this);

    textPalette.setColor(QPalette::Button, (textColor.isValid() ? textColor : old));

    m_text = textPalette.color(QPalette::Button);

    ui->pushButton_text_color->setPalette(textPalette);
}

void PropertiesDialog::on_pushButton_background_color_clicked()
{
    //background color
    QPalette textPalette = ui->pushButton_background_color->palette();
    QColor old = textPalette.color(QPalette::Button);
    QColor backgroundColor = QColorDialog::getColor(old, this);

    textPalette.setColor(QPalette::Button, (backgroundColor.isValid() ? backgroundColor : old));

    m_background = textPalette.color(QPalette::Button);

    ui->pushButton_background_color->setPalette(textPalette);
}

void PropertiesDialog::on_spinBox_speed_valueChanged(int arg1)
{
    m_speed = arg1;
}

void PropertiesDialog::on_pushButton_domyslne_clicked()
{
    m_chords = 1;
    m_shift = 0;
    m_encode = "UTF-8";
    m_column = 3;
    m_fullscreen = false;
    m_max_last_open = 10;
    m_text = QColor(255, 255, 255);
    m_background = QColor(0, 0, 0);
    m_speed = 8;

    ui->spinBox_column->setValue(m_column);
    ui->spinBox_speed->setValue(m_speed);
    ui->checkBox_fullscreen->setChecked(m_fullscreen);
    ui->comboBox_encoding->setCurrentText(m_encode);
    ui->spinBox_chords->setValue(m_chords);
    ui->spinBox_shift->setValue(m_shift);
    ui->spinBox__max_last_open->setValue(m_max_last_open);

    QPalette palette = ui->pushButton_text_color->palette();
    palette.setColor(QPalette::Button, m_text);
    ui->pushButton_text_color->setPalette(palette);

    QPalette palette2 = ui->pushButton_background_color->palette();
    palette2.setColor(QPalette::Button, m_background);
    ui->pushButton_background_color->setPalette(palette2);
}

void PropertiesDialog::on_buttonBox_accepted()
{
    s.setValue("editor/colimn_count", m_column);
    s.setValue("editor/scrolling_speed", m_speed);
    s.setValue("app/fullscreen", m_fullscreen);
    s.setValue("editor/code", m_encode);
    s.setValue("editor/chords", m_chords);
    s.setValue("editor/shift", m_shift);
    s.setValue("app/files_history_max_size", m_max_last_open);
    s.setValue("editor/text_color", m_text);
    s.setValue("editor/background_color", m_background);
}

void PropertiesDialog::on_pushButton_add_base_clicked()
{
    m_base_model->appendNewBase();
}

void PropertiesDialog::on_pushButton_delete_base_clicked()
{
    QModelIndex index = ui->tableView_bases->currentIndex();
    m_base_model->removeBase(index);
}
