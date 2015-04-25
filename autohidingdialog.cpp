#include "autohidingdialog.h"
#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QDebug>

AutoHidingDialog::AutoHidingDialog(QWidget *parent) :
    QDialog(parent),
    timer_hide(nullptr),
    timer_show(nullptr),
    is_hide(false),
    is_toggle_enable(true),
    hide_speed(1),
    in_process(false),
    hide_direction(HideRight),
    m_left(0),
    m_top(0),
    m_right(0),
    m_bottom(0),
    m_frame_top(0),
    m_width(0),
    m_height(0),
    m_enable(false),
    delay(1300)
{
    timer_show = new QTimer(this);
    timer_show->setInterval(hide_speed);
    connect(timer_show, SIGNAL(timeout()), this, SLOT(showWindow()));

    timer_hide = new QTimer(this);
    timer_hide->setInterval(hide_speed);
    connect(timer_hide, SIGNAL(timeout()), this, SLOT(hideWindow()));

    hide_delay = new QTimer(this);
    hide_delay->setInterval(delay);
    connect(hide_delay, SIGNAL(timeout()), this, SLOT(onTimerDelay()));

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(onFocusChanged(QWidget*,QWidget*)));
    setMouseTracking(true);
    installEventFilter(this);
}

bool AutoHidingDialog::eventFilter(QObject *o, QEvent *e)
{
    if(o == this)
    {
        neerToDesktopSide();
        if(m_enable)
        {
            if(e->type() == QEvent::Enter)
            {
                startShow();
                return true;
            }
            else if(e->type() == QEvent::Leave)
            {
                hide_delay->start();
                //startHide();
                return true;
            }
        }
    }
    return QObject::eventFilter(o, e);
}

bool AutoHidingDialog::isDisappear() const
{
    return is_hide;
}

void AutoHidingDialog::startHide()
{
    screen_width = QApplication::desktop()->width();
    screen_height = QApplication::desktop()->height();

    if(!is_hide && is_toggle_enable && !in_process)
    {
        timer_hide->start();
    }
}

void AutoHidingDialog::startShow()
{
    screen_width = QApplication::desktop()->width();
    screen_height = QApplication::desktop()->height();
    hide_delay->stop();
    if(is_hide && is_toggle_enable && !in_process)
    {
        timer_show->start();
    }
}

void AutoHidingDialog::hideWindow()
{
    in_process = true;
#ifdef Q_OS_WIN
    m_left = geometry().left();
    m_top = geometry().top();
    m_right = geometry().right();
    m_bottom = geometry().bottom();
    m_frame_top = frameGeometry().top();
    m_width = geometry().width();
    m_height = geometry().height();

    switch (hide_direction) {
    case HideLeft:
        if(m_right > 1)
        {
            if(m_frame_top < 0)
                setGeometry(m_left - 1, m_top-m_frame_top, m_width, m_height);
            else
                setGeometry(m_left - 1, m_top, m_width, m_height);
            return;
        }
        break;
    case HideTop:
        if(m_bottom > 1)
        {
           setGeometry(m_left, m_top - 1, m_width, m_height);
           return;
        }
        break;
    case HideRight:
        if(screen_width - m_left > 1)
        {
            if(m_frame_top < 0)
                setGeometry(m_left + 1, m_top-m_frame_top, m_width, m_height);
            else
                setGeometry(m_left + 1, m_top, m_width, m_height);

            return;
        }
        break;
    case HideBottom:
        if(screen_height - m_top > 1)
        {
           setGeometry(m_left, m_top + 1, m_width, m_height);
           return;
        }
        break;
    }
#endif
    timer_hide->stop();
    is_hide = true;
    in_process = false;
}

void AutoHidingDialog::showWindow()
{
    in_process = true;
#ifdef Q_OS_WIN
    m_left = geometry().left();
    m_top = geometry().top();
    m_right = geometry().right();
    m_bottom = geometry().bottom();
    m_frame_top = frameGeometry().top();
    m_width = geometry().width();
    m_height = geometry().height();

    switch (hide_direction) {
    case HideLeft:
        if(m_right < m_width-1)
        {
            if(m_frame_top < 0)
                setGeometry(m_left + 1, m_top-m_frame_top, m_width, m_height);
            else
                setGeometry(m_left + 1, m_top, m_width, m_height);
            return;
        }
        break;
    case HideTop:
        if(m_bottom < m_height)
        {
           setGeometry(m_left, m_top + 1, m_width, m_height);
            return;
        }
        break;
    case HideRight:
        if(screen_width - m_left < m_width)
        {
            if(m_frame_top < 0)
                setGeometry(m_left - 1, m_top-m_frame_top, m_width, m_height);
            else
                setGeometry(m_left - 1, m_top, m_width, m_height);
            return;
        }
        break;
    case HideBottom:
        if(screen_height - m_top < m_height)
        {
           setGeometry(m_left, m_top - 1, m_width, m_height);
           return;
        }
        break;
    }
#endif
    timer_show->stop();
    is_hide = false;
    in_process = false;
}

void AutoHidingDialog::onTimerDelay()
{
    startHide();
    hide_delay->stop();
}

void AutoHidingDialog::onFocusChanged(QWidget *old, QWidget *now)
{
    if(m_enable)
    {
        if(checkChildren(this->children(), now, true)) return;
        if(checkChildren(this->children(), old, false)) return;
    }
}

bool AutoHidingDialog::checkChildren(QObjectList list, QObject *obj, bool now)
{
    if(list.empty()) return false;

    if(now)
    {
        foreach (QObject* tmp, list)
        {
            if(tmp == obj)
            {
                timer_hide->stop();
                in_process = false;
                is_hide = true;
                startShow();
                is_toggle_enable = false;
                return true;
            }
            if(checkChildren(tmp->children(), obj, true)) return true;
        }
    }
    else
    {
        foreach (QObject* tmp, list)
        {
            if(tmp == obj)
            {
                is_toggle_enable = true;
                startHide();
                return true;
            }
            if(checkChildren(tmp->children(), obj, false)) return true;
        }
    }
    return false;
}

void AutoHidingDialog::neerToDesktopSide()
{

    screen_width = QApplication::desktop()->width();
    screen_height = QApplication::desktop()->height();

    m_left = geometry().left();
    m_top = geometry().top();
    m_right = geometry().right();
    m_bottom = geometry().bottom();
    m_frame_top = frameGeometry().top();
    m_width = geometry().width();
    m_height = geometry().height();

    bool neer = false;

    if(screen_width - m_right < 20)
    {
        neer = true;
        hide_direction = HideRight;
    }
    else if(m_left < 20)
    {
        neer = true;
        hide_direction = HideLeft;
    }

    m_enable = neer;
}

