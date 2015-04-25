#ifndef AUTOHIDINGDIALOG_H
#define AUTOHIDINGDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTimer>

class AutoHidingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AutoHidingDialog(QWidget *parent = 0);

    bool eventFilter(QObject * o, QEvent *e);

    bool isDisappear() const;

    enum HideDirection
    {
        HideLeft,
        HideTop,
        HideRight,
        HideBottom
    };
signals:

public slots:
    void startHide();
    void startShow();

private slots:
    void hideWindow();
    void showWindow();
    void onTimerDelay();
    void onFocusChanged(QWidget* old,QWidget* now);

private:
    void neerToDesktopSide();
    bool checkChildren(QObjectList list, QObject* obj, bool now);

    bool m_enable;
    bool is_hide;
    bool is_toggle_enable;
    bool in_process;
    int screen_height;
    int screen_width;
    int hide_speed;
    int delay;
    HideDirection hide_direction;
    int m_left, m_top, m_right, m_bottom, m_frame_top, m_height, m_width;

    QHBoxLayout* layout;
    QPushButton* button;
    QTimer* timer_hide;
    QTimer* timer_show;
    QTimer* hide_delay;
};

#endif // AUTOHIDINGDIALOG_H
