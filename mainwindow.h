#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QVBoxLayout>
#include "mysimplelist.h"
#include <boost/shared_ptr.hpp>

typedef boost::shared_ptr<QAction> QActionPtr;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool eventFilter(QObject *o, QEvent *e);

protected:
    virtual void dragEnterEvent(QDragEnterEvent* event);
    virtual void dropEvent(QDropEvent* event);

private slots:
    void textChange();

    void encodingChange(QString encode);

    void chordsChange(int chords);

    void moveScrollBar();

    void setScrollingSpeed(int speed);

    void historyFileOpen(QAction* action);

    void on_actionNew_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionFont_triggered();

    void on_actionText_Color_triggered();

    void on_actionBackground_Color_triggered();

    void on_actionExit_triggered();

    void on_actionSave_as_triggered();

    void on_actionRefresh_triggered();

    void on_actionNext_Chords_triggered();

    void on_actionPrevious_Chords_triggered();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_actionAbout_Qt_triggered();

    void on_spinBox_column_valueChanged(int arg1);

    void on_sb_shift_valueChanged(int arg1);

    void on_actionNext_schift_chords_triggered();

    void on_actionPrevious_schift_chords_triggered();

    void on_actionFull_Screen_triggered();

    void on_actionSwap_colors_triggered();

private:
    void Save();
    void Load();
    void PartitionText();
    void ShowText();
    void HideAllBars();
    void ShowAllBars();
    void AddViewToolBar();
    void AddScrollToolBar();
    void SetCentralText();
    void SetFullScreenText();
    void LoadHistoryActions();
    void AddAction(QString path);

    QToolBar* m_view_tool_bar;
    QToolBar* m_scroll_tool_bar;
    QWidget* m_central;
    QVBoxLayout* m_layout;

    Ui::MainWindow* ui;
    QString m_dir;
    QTextDocument* m_doc;
    QTimer* m_timer;
    QStringList m_text_lines;
    QVector<QString> m_column_text;
    int m_columns_count;
    int m_scrolling_speed;
    bool m_changed;
    bool m_fullscreen;

    QSettings settings;
    MySimpleList<QActionPtr> m_files_history;
};

#endif // MAINWINDOW_H
