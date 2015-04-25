#include "quickoptions.h"
#include "ui_quickoptions.h"
#include "mysharedata.h"

QuickOptions::QuickOptions(QWidget *parent) :
    AutoHidingDialog(parent),
    ui(new Ui::QuickOptions)
{
    ui->setupUi(this);

    //------------Wypełnianie boxa z kodowaniami------------
    ui->comboBox_encoder->clear();
    ui->comboBox_encoder->addItem("UTF-8");
    ui->comboBox_encoder->addItem("UTF-16");
    ui->comboBox_encoder->addItem("Windows-1250");
    ui->comboBox_encoder->addItem("ISO 8859-1");
    ui->comboBox_encoder->addItem("ISO 8859-2");

    if(!MyShareData::m_encode.isEmpty())
        ui->comboBox_encoder->setCurrentText(MyShareData::m_encode);

    ui->spinBox_chords->setValue(MyShareData::m_chords);
    ui->spinBox_shift->setValue(MyShareData::m_chords_shift);
    ui->spinBox_delay->setValue(MyShareData::m_scrolling_delay);
    ui->spinBox_speed->setValue(MyShareData::m_scrolling_speed);
}

QuickOptions::~QuickOptions()
{
    delete ui;
}

void QuickOptions::setAll()
{
    if(!MyShareData::m_encode.isEmpty())
        ui->comboBox_encoder->setCurrentText(MyShareData::m_encode);
    ui->spinBox_chords->setValue(MyShareData::m_chords);
    ui->spinBox_shift->setValue(MyShareData::m_chords_shift);
    ui->spinBox_delay->setValue(MyShareData::m_scrolling_delay);
    ui->spinBox_speed->setValue(MyShareData::m_scrolling_speed);
}

void QuickOptions::on_spinBox_chords_valueChanged(int arg1)
{
    emit chordsChanged(arg1);
}

void QuickOptions::on_spinBox_shift_valueChanged(int arg1)
{
    emit shiftChanged(arg1);
}

void QuickOptions::on_spinBox_delay_valueChanged(int arg1)
{
    emit delayChanged(arg1);
}

void QuickOptions::on_spinBox_speed_valueChanged(int arg1)
{
    emit speedChanged(arg1);
}

void QuickOptions::on_pushButton_start_clicked()
{
    emit start();
}

void QuickOptions::on_pushButton_stop_clicked()
{
    emit stop();
}

void QuickOptions::on_comboBox_encoder_activated(const QString &arg1)
{
    emit encodeChanged(arg1);
}
