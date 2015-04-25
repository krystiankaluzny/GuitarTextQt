#include "propertiesdialog.h"
#include "ui_propertiesdialog.h"
#include "mysharedata.h"
#include <QColorDialog>
#include <QFontDialog>

PropertiesDialog::PropertiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PropertiesDialog),
    m_auto_column(MyShareData::m_auto_column)
{
    ui->setupUi(this);

    QList<QAbstractButton *> buttons = ui->buttonBox->buttons();
    if(buttons.size() == 2)
    {
        buttons.at(0)->setText("Ok");
        buttons.at(1)->setText("Anuluj");
    }
    m_base_model = new BaseModel(&MyShareData::m_text_bases);

    delegate = new TreeItemDelegate;
    ui->tableView_bases->setModel(m_base_model);
    ui->tableView_bases->setItemDelegateForColumn(1, delegate);

    m_column = MyShareData::m_columns_count;
    ui->spinBox_column->setValue(m_column);

    m_fullscreen = MyShareData::m_fullscreen;
    ui->checkBox_fullscreen->setChecked(m_fullscreen);

    ui->comboBox_encoding->addItem("UTF-8");
    ui->comboBox_encoding->addItem("UTF-16");
    ui->comboBox_encoding->addItem("Windows-1250");
    ui->comboBox_encoding->addItem("ISO 8859-1");
    ui->comboBox_encoding->addItem("ISO 8859-2");

    m_encode = MyShareData::m_encode;
    ui->comboBox_encoding->setCurrentText(m_encode);

    m_chords = MyShareData::m_chords;
    ui->spinBox_chords->setValue(m_chords);

    m_shift = MyShareData::m_chords_shift;
    ui->spinBox_shift->setValue(m_shift);

    m_max_last_open = MyShareData::m_files_history.getMaxSize();
    ui->spinBox__max_last_open->setValue(m_max_last_open);

    m_max_favourites = MyShareData::m_favourites_max_size;
    ui->spinBox__max_favourites->setValue(m_max_favourites);

    m_text = MyShareData::m_text_color;
    m_background = MyShareData::m_background_color;
    m_chords_color = MyShareData::m_chords_color;

    m_font = MyShareData::m_font;

    if(!m_auto_column)
        ui->checkBox_auto_column->setChecked(false);

    doc = new QTextDocument;
    doc->setDefaultStyleSheet(".akord{color: " + MyShareData::m_chords_color.name() + "}");
    doc->setHtml(QString("Test <span class = \"akord\">a C D</span>"));
    ui->et_simple_font->setDocument(doc);

    QPalette textPalette = ui->et_simple_font->palette();
    textPalette.setColor(QPalette::Text, m_text);
    textPalette.setColor(QPalette::Base, m_background);
    ui->et_simple_font->setPalette(textPalette);

    ui->et_simple_font->setFont(m_font);

    ui->kse_new->setKeySequence(MyShareData::kse_new);
    ui->kse_open->setKeySequence(MyShareData::kse_open);
    ui->kse_save->setKeySequence(MyShareData::kse_save);
    ui->kse_save_as->setKeySequence(MyShareData::kse_save_as);
    ui->kse_reload->setKeySequence(MyShareData::kse_reload);
    ui->kse_chords_up->setKeySequence(MyShareData::kse_chords_up);
    ui->kse_chords_down->setKeySequence(MyShareData::kse_chords_down);
    ui->kse_shift_up->setKeySequence(MyShareData::kse_shift_up);
    ui->kse_shift_down->setKeySequence(MyShareData::kse_shift_down);
    ui->kse_fullscreen->setKeySequence(MyShareData::kse_fullscreen);
    ui->kse_properties->setKeySequence(MyShareData::kse_properties);
    ui->kse_find->setKeySequence(MyShareData::kse_find);
    ui->kse_quick_options->setKeySequence(MyShareData::kse_quick_options);
    ui->kse_close->setKeySequence(MyShareData::kse_close);
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
    QColor textColor = QColorDialog::getColor(textColor, this);

    if(textColor.isValid())
    {
        m_text = textColor;
        QPalette textPalette = ui->et_simple_font->palette();
        textPalette.setColor(QPalette::Text, m_text);
        ui->et_simple_font->setPalette(textPalette);
    }
}

void PropertiesDialog::on_pushButton_background_color_clicked()
{
    //background color
    QColor backgroundColor = QColorDialog::getColor(m_background, this);

    if(backgroundColor.isValid())
    {
        m_background = backgroundColor;
        QPalette textPalette = ui->et_simple_font->palette();
        textPalette.setColor(QPalette::Base, m_background);
        ui->et_simple_font->setPalette(textPalette);
    }
}

void PropertiesDialog::on_pushButton_chords_color_3_clicked()
{
    //chords color
    QColor chordsColor = QColorDialog::getColor(m_chords_color, this);

    if(chordsColor.isValid())
    {
        m_chords_color = chordsColor;
        doc->setDefaultStyleSheet(".akord{color: " + m_chords_color.name() + "}");
        doc->setHtml(QString("Test <span class = \"akord\">a C D</span>"));
    }
}

void PropertiesDialog::on_spinBox__max_favourites_valueChanged(int arg1)
{
    m_max_favourites = arg1;
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
    m_chords_color = QColor(255, 0, 0);

    ui->spinBox_column->setValue(m_column);
    ui->checkBox_fullscreen->setChecked(m_fullscreen);
    ui->comboBox_encoding->setCurrentText(m_encode);
    ui->spinBox_chords->setValue(m_chords);
    ui->spinBox_shift->setValue(m_shift);
    ui->spinBox__max_last_open->setValue(m_max_last_open);

    QPalette textPalette = ui->et_simple_font->palette();
    textPalette.setColor(QPalette::Text, m_text);
    textPalette.setColor(QPalette::Base, m_background);
    ui->et_simple_font->setPalette(textPalette);

    ui->et_simple_font->setFont(m_font);

    ui->kse_new->setKeySequence(QKeySequence(Qt::CTRL + Qt::Key_N));
    ui->kse_open->setKeySequence(QKeySequence(Qt::CTRL + Qt::Key_O));
    ui->kse_save->setKeySequence(QKeySequence(Qt::CTRL + Qt::Key_S));
    ui->kse_save_as->setKeySequence(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
    ui->kse_reload->setKeySequence(QKeySequence(Qt::Key_F5));
    ui->kse_chords_up->setKeySequence(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Up));
    ui->kse_chords_down->setKeySequence(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Down));
    ui->kse_shift_up->setKeySequence(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Right));
    ui->kse_shift_down->setKeySequence(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Left));
    ui->kse_fullscreen->setKeySequence(QKeySequence(Qt::Key_F11));
    ui->kse_properties->setKeySequence(QKeySequence(Qt::CTRL + Qt::Key_P));
    ui->kse_find->setKeySequence(QKeySequence(Qt::CTRL + Qt::Key_F));
    ui->kse_quick_options->setKeySequence(QKeySequence(Qt::CTRL + Qt::Key_G));
    ui->kse_close->setKeySequence(QKeySequence(Qt::CTRL + Qt::Key_Q));
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

void PropertiesDialog::on_checkBox_auto_column_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked)
    {
        ui->label_5->setDisabled(true);
        ui->spinBox_column->setDisabled(true);
        m_column = 1000;
        m_auto_column = true;
    }
    else if(arg1 == Qt::Unchecked)
    {
        ui->label_5->setDisabled(false);
        ui->spinBox_column->setDisabled(false);
        m_column = ui->spinBox_column->value();
        m_auto_column = false;
    }
}

void PropertiesDialog::on_pushButton_refresh_clicked()
{
    m_base_model->refresh();
}

void PropertiesDialog::on_pushButton_font_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, m_font, this);
    if(ok)
    {
        m_font = font;
        ui->et_simple_font->setFont(m_font);
    }
}

void PropertiesDialog::on_buttonBox_accepted()
{
    MyShareData::kse_new = ui->kse_new->keySequence();
    MyShareData::kse_open = ui->kse_open->keySequence();
    MyShareData::kse_save = ui->kse_save->keySequence();
    MyShareData::kse_save_as = ui->kse_save_as->keySequence();
    MyShareData::kse_reload = ui->kse_reload->keySequence();
    MyShareData::kse_chords_up = ui->kse_chords_up->keySequence();
    MyShareData::kse_chords_down = ui->kse_chords_down->keySequence();
    MyShareData::kse_shift_up = ui->kse_shift_up->keySequence();
    MyShareData::kse_shift_down = ui->kse_shift_down->keySequence();
    MyShareData::kse_fullscreen = ui->kse_fullscreen->keySequence();
    MyShareData::kse_properties = ui->kse_properties->keySequence();
    MyShareData::kse_find = ui->kse_find->keySequence();
    MyShareData::kse_quick_options = ui->kse_quick_options->keySequence();
    MyShareData::kse_close = ui->kse_close->keySequence();
}
