#ifndef MYSHAREDATA_H
#define MYSHAREDATA_H

#include "mysimplelist.h"
#include "treeitem.h"
#include <QSettings>
#include <QMap>
#include <QString>
#include <QColor>
#include <boost/shared_ptr.hpp>
#include <QFileInfo>
#include <QAction>

typedef boost::shared_ptr<QAction> QActionPtr;

class MyShareData{

public:
    static int m_columns_count;
    static int m_scrolling_delay;
    static int m_scrolling_speed;
    static bool m_changed;
    static bool m_fullscreen;
    static int m_column_width;
    static bool m_auto_column;
    static bool m_text_editable;
    static int m_favourites_max_size;
    static int m_chords;
    static int m_chords_shift;
    static QMap<QString, TreeItem*> m_text_bases;
    static ListSortByWeight<QString> m_favourites;
    static QString m_actual_path;
    static QColor m_chords_color;
    static QColor m_text_color;
    static QColor m_background_color;
    static QString m_encode;
    static MySimpleList<QActionPtr> m_files_history;
    static QFont m_font;
    static QKeySequence kse_new;
    static QKeySequence kse_open;
    static QKeySequence kse_save;
    static QKeySequence kse_save_as;
    static QKeySequence kse_reload;
    static QKeySequence kse_chords_up;
    static QKeySequence kse_chords_down;
    static QKeySequence kse_shift_up;
    static QKeySequence kse_shift_down;
    static QKeySequence kse_fullscreen;
    static QKeySequence kse_properties;
    static QKeySequence kse_find;
    static QKeySequence kse_quick_options;
    static QKeySequence kse_close;

    static void loadData();
    static void saveData();
    static void AddLastOpenAction(QString path, bool front = true);
    static void AddFavouriteAction(QString path, int weight);

};

#endif // MYSHAREDATA_H
