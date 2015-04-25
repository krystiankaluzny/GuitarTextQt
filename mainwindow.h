#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QApplication>
#include <QSettings>
#include <QTimer>
#include <QKeyEvent>
#include <QTextBlock>
#include <QTextStream>
#include <QTextEdit>
#include "chord.h"
#include "mysimplelist.h"
#include "treeitem.h"
#include "properties/propertiesdialog.h"
#include "search/searchdialog.h"
#include "quick/quickoptions.h"

namespace Ui {
class MainWindow;
}

class Text;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool eventFilter(QObject *o, QEvent *e);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private slots:

    void Open(QString path);
    void moveScrollBar();
    void historyFileOpen(QAction* action);
    void favouriteOpen(QAction* action);
    void onEncodeChanged(const QString &arg1);
    void addFavourite();
    void onStartScrolling();
    void startScrolling();
    void onStopScrolling();
    void onChordsChanged(int arg1);
    void onShiftChanged(int arg1);
    void onSpeedChanged(int arg1);
    void onDelayChanged(int arg1);
    void on_textEdit_textChanged();

    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionExit_triggered();
    void on_actionSave_as_triggered();
    void on_actionRefresh_triggered();
    void on_actionNext_Chords_triggered();
    void on_actionPrevious_Chords_triggered();
    void on_actionNext_schift_chords_triggered();
    void on_actionPrevious_schift_chords_triggered();
    void on_actionAbout_Qt_triggered();
    void on_actionFull_Screen_triggered();
    void on_actionProperties_triggered();
    void on_actionGuitarText_triggered();
    void on_actionSearch_triggered();
    void on_actionQuickOptions_triggered();

private:
    void Save();
    void Load(bool add_action = true);
    void SetShift(int s);
    void SetChordsType(int c);
    void RefreshWidth();
    void DividingText();
    void ShowText();
    void SetCentralText();
    void SetFullScreenText();
    void LoadHistoryActions();
    void MoveHorizontalScrollBar();
    void SetProperties();
    void SaveProperties();
    void LoadFavourites();
    void SetShortcuts();

    Ui::MainWindow* ui;

    QWidget* m_central;
    QVBoxLayout* m_layout;
    QToolBar* m_tool_bar;

    QTextDocument* m_doc;
    QTimer* m_scrolling_delay;
    QTimer* m_scrolling;
    QTimer* m_favourite_timer;

    MySimpleList< MySimpleList<Text>* > m_text_lines;
    QVector<QString> m_column_text;

    QAction* m_clear_history;

    SearchDialog* m_search_dialog;
    QuickOptions* m_quick_options;
};

class Text
{
public:
    Text() : m_text(""), m_chords_type(""), m_chords(false), m_width(0), m_shift(0), m_chords_num(1) {}
    Text(QString t, bool c) : m_text(t), m_chords(c), m_width(0), m_shift(0), m_chords_num(0) { if(c) setChordsType(0); else m_chords_type = ""; }
    QString getChordsType() const { return m_chords_type; }
    QString getText() const { return m_text; }
    bool chords() const { return m_chords; }
    int width() const { return m_width; }
    int shift() const { return m_shift; }
    void setShift(int s)
    {
        m_chords_type = ShiftChordString(m_chords_type.split(" "), s - m_shift);
        m_shift = s % 12;
    }
    void setWidth(int w) { m_width = w; }
    void setChordsType(int i)
    {
        QStringList c = m_text.split("|");
        m_chords_num = i < 0 ? 0 : (i >= c.size() ? c.size()-1 : i);
        m_chords_type = c[m_chords_num];
        m_shift = 0;
    }
private:
    QString m_text; //cały tekst w nawiasach
    QString m_chords_type;
    bool m_chords;
    int m_width;
    int m_shift;
    int m_chords_num;
};

#endif // MAINWINDOW_H
