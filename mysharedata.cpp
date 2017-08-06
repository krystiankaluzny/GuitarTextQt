#include "mysharedata.h"
#include <QCoreApplication>
#include <QDebug>

int MyShareData::m_columns_count;
int MyShareData::m_scrolling_delay;
int MyShareData::m_scrolling_speed;
bool MyShareData::m_changed;
bool MyShareData::m_fullscreen;
int MyShareData::m_column_width;
bool MyShareData::m_auto_column;
bool MyShareData::m_text_editable;
int MyShareData::m_favourites_max_size;
int MyShareData::m_chords;
int MyShareData::m_chords_shift;
QMap<QString, TreeItem*> MyShareData::m_text_bases;
ListSortByWeight<QString> MyShareData::m_favourites;
QString MyShareData::m_actual_path;
QColor MyShareData::m_chords_color;
QColor MyShareData::m_text_color;
QColor MyShareData::m_background_color;
QString MyShareData::m_encode;
MySimpleList<QActionPtr> MyShareData::m_files_history;
QFont MyShareData::m_font;
QKeySequence MyShareData::kse_new;
QKeySequence MyShareData::kse_open;
QKeySequence MyShareData::kse_save;
QKeySequence MyShareData::kse_save_as;
QKeySequence MyShareData::kse_reload;
QKeySequence MyShareData::kse_chords_up;
QKeySequence MyShareData::kse_chords_down;
QKeySequence MyShareData::kse_shift_up;
QKeySequence MyShareData::kse_shift_down;
QKeySequence MyShareData::kse_fullscreen;
QKeySequence MyShareData::kse_properties;
QKeySequence MyShareData::kse_find;
QKeySequence MyShareData::kse_quick_options;
QKeySequence MyShareData::kse_close;


MyShareData object;

void MyShareData::loadData()
{
    #ifdef Q_OS_LINUX
    QString ini_path(getenv("HOME"));
    ini_path += "/.config/";
    #endif
    #ifdef Q_OS_WIN32
    QString ini_path(QCoreApplication::applicationDirPath());
    #endif
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, ini_path);
    QSettings m_settings(QSettings::IniFormat, QSettings::UserScope, "Obywatel GCC", "Guiter Text");

    m_settings.setValue("editor/chords", 1);
    m_settings.setValue("editor/shift", 0);

    MyShareData::m_columns_count = m_settings.value("editor/column_count", 3).toInt();
    MyShareData::m_scrolling_delay = m_settings.value("editor/scrolling_delay", 8).toInt();
    MyShareData::m_scrolling_speed = m_settings.value("editor/scrolling_speed", 200).toInt();
    MyShareData::m_fullscreen = m_settings.value("app/fullscreen", false).toBool();
    MyShareData::m_text_color = m_settings.value("editor/text_color", QColor(255, 255, 255)).value<QColor>();
    MyShareData::m_background_color = m_settings.value("editor/background_color", QColor(0, 0, 0)).value<QColor>();
    MyShareData::m_chords_color =  m_settings.value("editor/chords_color", QColor(255, 0, 0)).value<QColor>();
    MyShareData::m_chords = m_settings.value("editor/chords", 1).toInt();
    MyShareData::m_chords_shift = m_settings.value("editor/shift", 0).toInt();
    MyShareData::m_auto_column = m_settings.value("editor/auto_column", true).toBool();

    QFont font;
    font.setPointSize(14);
    MyShareData::m_font = m_settings.value("editor/font", font).value<QFont>();
    MyShareData::m_encode = m_settings.value("editor/code", "UTF-8").toString();
    MyShareData::m_files_history.setMaxSize(m_settings.value("app/files_history_max_size", 10).toInt());
    MyShareData::m_favourites_max_size = m_settings.value("app/favourites_max_size", 20).toInt();

    MyShareData::kse_new = m_settings.value("shortcuts/new", QKeySequence(Qt::CTRL + Qt::Key_N)).value<QKeySequence>();
    MyShareData::kse_open = m_settings.value("shortcuts/open", QKeySequence(Qt::CTRL + Qt::Key_O)).value<QKeySequence>();
    MyShareData::kse_save = m_settings.value("shortcuts/save", QKeySequence(Qt::CTRL + Qt::Key_S)).value<QKeySequence>();
    MyShareData::kse_save_as = m_settings.value("shortcuts/save_as", QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S)).value<QKeySequence>();
    MyShareData::kse_reload = m_settings.value("shortcuts/reload", QKeySequence(Qt::Key_F5)).value<QKeySequence>();
    MyShareData::kse_chords_up = m_settings.value("shortcuts/chords_up", QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Up)).value<QKeySequence>();
    MyShareData::kse_chords_down = m_settings.value("shortcuts/chords_down", QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Down)).value<QKeySequence>();
    MyShareData::kse_shift_up = m_settings.value("shortcuts/shift_up", QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Right)).value<QKeySequence>();
    MyShareData::kse_shift_down = m_settings.value("shortcuts/shift_down", QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Left)).value<QKeySequence>();
    MyShareData::kse_fullscreen = m_settings.value("shortcuts/fullscreen", QKeySequence(Qt::Key_F11)).value<QKeySequence>();
    MyShareData::kse_properties = m_settings.value("shortcuts/properties", QKeySequence(Qt::CTRL + Qt::Key_P)).value<QKeySequence>();
    MyShareData::kse_find = m_settings.value("shortcuts/find", QKeySequence(Qt::CTRL + Qt::Key_F)).value<QKeySequence>();
    MyShareData::kse_quick_options = m_settings.value("shortcuts/quick_options", QKeySequence(Qt::CTRL + Qt::Key_G)).value<QKeySequence>();
    MyShareData::kse_close = m_settings.value("shortcuts/close", QKeySequence(Qt::CTRL + Qt::Key_Q)).value<QKeySequence>();

    //------------Bazy tekstów------------
    int size = m_settings.beginReadArray("bases");
    for(int i = 0; i < size; i++)
    {
        m_settings.setArrayIndex(i);
        QString key = m_settings.value("key").toString();
        TreeItem* item = createDirTree(m_settings.value("path").toString(), nullptr);
        MyShareData::m_text_bases.insert(key, item);
    }
    m_settings.endArray();

    //------------Lista ostatnich plików------------
    size = m_settings.beginReadArray("recent");
    for(int i = 0; i < size && i < MyShareData::m_files_history.getMaxSize(); i++)
    {
        m_settings.setArrayIndex(i);
        QString path = m_settings.value("path").toString();
        MyShareData::AddLastOpenAction(path, false);
    }
    m_settings.endArray();

    //------------Ulubione------------
    size = m_settings.beginReadArray("favourites");
    for(int i = 0; i < size; i++)
    {
        m_settings.setArrayIndex(i);
        QString path = m_settings.value("path").toString();
        int weight = m_settings.value("weight").toInt();
        MyShareData::AddFavouriteAction(path, weight);
    }
    m_settings.endArray();
}

void MyShareData::saveData()
{
    #ifdef Q_OS_LINUX
    QString ini_path(getenv("HOME"));
    ini_path += "/.config/";
    #endif
    #ifdef Q_OS_WIN32
    QString ini_path(QCoreApplication::applicationDirPath());
    #endif
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, ini_path);
    QSettings m_settings(QSettings::IniFormat, QSettings::UserScope, "Obywatel GCC", "Guiter Text");

    m_settings.setValue("editor/loop", false);
    m_settings.setValue("editor/chords", 1);
    m_settings.setValue("editor/shift", 0);
    m_settings.setValue("app/fullscreen", false);
    m_settings.setValue("editor/column_count", MyShareData::m_columns_count);
    m_settings.setValue("editor/scrolling_delay", MyShareData::m_scrolling_delay);
    m_settings.setValue("editor/scrolling_speed", MyShareData::m_scrolling_speed);
    m_settings.setValue("editor/text_color", MyShareData::m_text_color);
    m_settings.setValue("editor/background_color", MyShareData::m_background_color);
    m_settings.setValue("editor/chords_color", MyShareData::m_chords_color);
    m_settings.setValue("editor/auto_column", MyShareData::m_auto_column);
    m_settings.setValue("editor/font", MyShareData::m_font);
    m_settings.setValue("editor/code", MyShareData::m_encode);
    m_settings.setValue("app/files_history_max_size", MyShareData::m_files_history.getMaxSize());
    m_settings.setValue("app/favourites_max_size", MyShareData::m_favourites_max_size);

    m_settings.setValue("shortcuts/new", MyShareData::kse_new);
    m_settings.setValue("shortcuts/open", MyShareData::kse_open);
    m_settings.setValue("shortcuts/save", MyShareData::kse_save);
    m_settings.setValue("shortcuts/save_as", MyShareData::kse_save_as);
    m_settings.setValue("shortcuts/reload", MyShareData::kse_reload);
    m_settings.setValue("shortcuts/chords_up", MyShareData::kse_chords_up);
    m_settings.setValue("shortcuts/chords_down", MyShareData::kse_chords_down);
    m_settings.setValue("shortcuts/shift_up", MyShareData::kse_shift_up);
    m_settings.setValue("shortcuts/shift_down", MyShareData::kse_shift_down);
    m_settings.setValue("shortcuts/fullscreen", MyShareData::kse_fullscreen);
    m_settings.setValue("shortcuts/properties", MyShareData::kse_properties);
    m_settings.setValue("shortcuts/find", MyShareData::kse_find);
    m_settings.setValue("shortcuts/quick_options", MyShareData::kse_quick_options);
    m_settings.setValue("shortcuts/close", MyShareData::kse_close);

    //------------Bazy tekstów------------
    m_settings.remove("bases");
    QMap<QString, TreeItem*>::iterator iter;
    int i = 0;
    m_settings.beginWriteArray("bases");
    for(iter = m_text_bases.begin(); iter != m_text_bases.end(); iter++, i++)
    {
        m_settings.setArrayIndex(i);
        m_settings.setValue("key", iter.key());
        m_settings.setValue("path", iter.value() != nullptr ? iter.value()->data(0).toString() : QString(""));
    }
    m_settings.endArray();

    //------------Lista ostatnich plików------------
    i = 0;
    m_settings.remove("recent");
    MySimpleList<QActionPtr>::iterator files_history_iterator;
    m_settings.beginWriteArray("recent");
    for(files_history_iterator = m_files_history.first(); files_history_iterator != m_files_history.end(); files_history_iterator++, i++)
    {
        m_settings.setArrayIndex(i);
        m_settings.setValue("path", (*files_history_iterator).get() != nullptr ? (*files_history_iterator)->data().toString() : QString(""));
    }
    m_settings.endArray();

    //------------Ulubione------------
    i = 0;
    m_settings.remove("favourites");
    ListSortByWeight<QString>::iterator favourite_iter;
    m_settings.beginWriteArray("favourites");
    for(favourite_iter = m_favourites.first(); favourite_iter != m_favourites.end(); favourite_iter++, i++)
    {
        m_settings.setArrayIndex(i);
        m_settings.setValue("path", *favourite_iter);
        m_settings.setValue("weight", favourite_iter.weight());
    }
    m_settings.endArray();
}

void MyShareData::AddLastOpenAction(QString path, bool front)
{
    QString action_path;
    QFileInfo file_info(path);
    QAction* act = new QAction(file_info.fileName(), nullptr);
    act->setData(path);

    MySimpleList<QActionPtr>::iterator iter = MyShareData::m_files_history.first();

    for(;iter != MyShareData::m_files_history.end(); iter++)
    {
        action_path = (*iter)->data().toString();
        if(action_path == path)
            MyShareData::m_files_history.remove(iter);
    }

    QActionPtr action_ptr(act);
    if(front) MyShareData::m_files_history.addAtFront(action_ptr);
    else MyShareData::m_files_history.addAtEnd(action_ptr);
}

void MyShareData::AddFavouriteAction(QString path, int weight)
{
    if(!path.isEmpty())
        MyShareData::m_favourites.add(path, weight);
}
