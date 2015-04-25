#ifndef QUICKOPTIONS_H
#define QUICKOPTIONS_H

#include <QDialog>
#include "autohidingdialog.h"

namespace Ui {
class QuickOptions;
}

class QuickOptions : public AutoHidingDialog
{
    Q_OBJECT

public:
    explicit QuickOptions(QWidget *parent = 0);
    ~QuickOptions();

    void setAll();
signals:
    void chordsChanged(int arg);
    void shiftChanged(int arg);
    void delayChanged(int arg);
    void speedChanged(int arg);
    void start();
    void stop();
    void encodeChanged(const QString& arg);

private slots:
    void on_spinBox_chords_valueChanged(int arg1);

    void on_spinBox_shift_valueChanged(int arg1);

    void on_spinBox_delay_valueChanged(int arg1);

    void on_spinBox_speed_valueChanged(int arg1);

    void on_pushButton_start_clicked();

    void on_pushButton_stop_clicked();

    void on_comboBox_encoder_activated(const QString &arg1);

private:
    Ui::QuickOptions *ui;
};

#endif // QUICKOPTIONS_H
