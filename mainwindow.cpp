#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>
#include <QFileDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QMessageBox>

#include "autohidingdialog.h"
#include "mysharedata.h"

#define GUITARTEXT_VERSION "1.6.35"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_doc(new QTextDocument),
    m_scrolling_delay(new QTimer(this)),
    m_scrolling(new QTimer(this)),
    m_favourite_timer(new QTimer(this))
{
    setWindowTitle("Guitar Text");

    ui->setupUi(this);

    ui->textEdit->installEventFilter(this);
    ui->textEdit->setAcceptRichText(false);

    //------------wczytanie zapisanych wartości z pliku------------
    MyShareData::loadData();
    SetProperties();

    //------------Auto przewijanie------------
    connect(m_scrolling_delay, SIGNAL(timeout()), this, SLOT(startScrolling()));
    connect(m_scrolling, SIGNAL(timeout()), this, SLOT(moveScrollBar()));

    //------------Ustawienie identyfikatera html akordu------------
    m_doc->setDefaultStyleSheet(".akord{color: " + MyShareData::m_chords_color.name() + "}");

    onChordsChanged(1);

    //------------Ustawienei Centalnego Widgetu------------
    m_central = new QWidget(this);
    this->setCentralWidget(m_central);
    m_layout = new QVBoxLayout;
    m_layout->addWidget(ui->textEdit);
    m_central->setLayout(m_layout);

    m_tool_bar = ui->toolBar;
    SetCentralText();

    ui->menuLast_open->setEnabled(false);
    connect(ui->menuLast_open, SIGNAL(triggered(QAction*)), this, SLOT(historyFileOpen(QAction*)));

    connect(ui->menuFavourite, SIGNAL(triggered(QAction*)), this, SLOT(favouriteOpen(QAction*)));

    //------------Drag and Drop------------
    setAcceptDrops(true); //pozwalamy na przechwytywanie elementów

    LoadHistoryActions();

    //------------Ulubione------------
    connect(m_favourite_timer, SIGNAL(timeout()), this, SLOT(addFavourite()));
    LoadFavourites();

    //------------Okno wyszukiwania------------
    m_search_dialog = new SearchDialog(this);
    connect(m_search_dialog, SIGNAL(doubleClicked(QString)), this, SLOT(Open(QString)));
    connect(m_search_dialog, SIGNAL(encodeChanged(QString)), this, SLOT(onEncodeChanged(QString)));

    m_quick_options = new QuickOptions(this);
    connect(m_quick_options, SIGNAL(chordsChanged(int)), this, SLOT(onChordsChanged(int)));
    connect(m_quick_options, SIGNAL(shiftChanged(int)), this, SLOT(onShiftChanged(int)));
    connect(m_quick_options, SIGNAL(delayChanged(int)), this, SLOT(onDelayChanged(int)));
    connect(m_quick_options, SIGNAL(speedChanged(int)), this, SLOT(onSpeedChanged(int)));
    connect(m_quick_options, SIGNAL(start()), this, SLOT(onStartScrolling()));
    connect(m_quick_options, SIGNAL(stop()), this, SLOT(onStopScrolling()));
    connect(m_quick_options, SIGNAL(encodeChanged(QString)), this, SLOT(onEncodeChanged(QString)));

    if(MyShareData::m_auto_column) ui->textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    else ui->textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    SetShortcuts();
}

MainWindow::~MainWindow()
{
    MyShareData::saveData();
    m_column_text.clear();
    m_text_lines.clear();
    m_favourite_timer->stop();
    m_scrolling_delay->stop();
    m_scrolling->stop();

    if(m_search_dialog != nullptr)
    {
        m_search_dialog->close();
        delete m_search_dialog;
    }
    if(m_quick_options != nullptr)
    {
        m_quick_options->close();
        delete m_quick_options;
    }

    delete m_scrolling_delay;
    delete m_scrolling;
    delete m_favourite_timer;
    delete m_layout;
    delete m_central;
    delete m_doc;
    delete m_clear_history;
    delete ui;
}

bool MainWindow::eventFilter(QObject *o, QEvent *e)
{
    if(o == ui->textEdit)
    {
        if(e->type() == QEvent::KeyPress)
        {
            QKeyEvent* key_event = static_cast<QKeyEvent*>(e);

            if(key_event->modifiers().testFlag(Qt::ControlModifier) && key_event->key() == Qt::Key_Plus)
            {
                QFont font = ui->textEdit->font();
                int size = font.pixelSize();
                size++;
                font.setPixelSize(size);
                ui->textEdit->setFont(font);
            }
            else if(key_event->modifiers().testFlag(Qt::ControlModifier) && key_event->key() == Qt::Key_Minus)
            {
                QFont font = ui->textEdit->font();
                int size = font.pixelSize();
                if(size > 0) size--;
                font.setPixelSize(size);
                ui->textEdit->setFont(font);
            }
            else if(key_event->modifiers().testFlag(Qt::ControlModifier) && key_event->key() == Qt::Key_Space && !MyShareData::m_text_editable)
            {
                if(m_scrolling->isActive())
                {
                    m_scrolling->stop();
                }
                else if(MyShareData::m_scrolling_speed > 0)
                {
                    m_scrolling_delay->start(MyShareData::m_scrolling_delay);
                }

                return true;
            }
            else if(key_event->key() == Qt::Key_Space)
            {
                if(MyShareData::m_text_editable)
                    return QMainWindow::eventFilter(o, e);

                MoveHorizontalScrollBar();
                return true;
            }
            else if(MyShareData::m_text_editable && key_event->modifiers().testFlag(Qt::ControlModifier) && (key_event->key() == Qt::Key_BracketLeft || key_event->key() == Qt::Key_BracketRight))
            {
                QTextCursor textCursor = ui->textEdit->textCursor();
                textCursor.insertText("[]");
                textCursor.movePosition(QTextCursor::Left); //przesuń o jeden w lewo
                ui->textEdit->setTextCursor(textCursor);
            }
            return false;
        }
        else if(e->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouse_event = static_cast<QMouseEvent*>(e);

            if(mouse_event->button() == Qt::MiddleButton)
            {
                MoveHorizontalScrollBar();
                return true;
            }
            return false;
        }
        else if(e->type() == QEvent::Resize && !MyShareData::m_text_editable)
        {
            DividingText();
            ShowText();
        }
    }
    return QMainWindow::eventFilter(o, e);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QString tmp = event->mimeData()->urls().at(0).toString(QUrl::RemoveScheme);

#ifdef Q_OS_WIN
    while(tmp.size() > 0)
    {
        if(tmp.at(0) == '/' || tmp.at(0) == '\\')
                tmp = tmp.right(tmp.size() - 1);
        else
            break;
    }
#endif
    QFileInfo file(tmp);

    if(file.exists() && file.isFile() && MyShareData::m_actual_path != file.absoluteFilePath())
    {
        Open(file.absoluteFilePath());
    }
}

void MainWindow::Open(QString path)//scieżka musi istnieć
{
    if(path.isEmpty() || path == MyShareData::m_actual_path)
        return;

    MyShareData::m_actual_path = path;

    Load();
    DividingText();
    ShowText();
    LoadHistoryActions();
    m_favourite_timer->start(20000);//20s

    MyShareData::m_chords_shift = 0;
    onChordsChanged(1);
}

void MainWindow::moveScrollBar()
{
    QScrollBar *bar = ui->textEdit->verticalScrollBar();

    if(bar != NULL)
    {
        if(bar->value() >= bar->maximum())
        {
            m_scrolling_delay->stop();
            m_scrolling->stop();
        }
        else
            bar->setValue(bar->value() + 1);
    }
}

void MainWindow::historyFileOpen(QAction *action)
{
    if(action == nullptr) return;
    if(action == m_clear_history)
    {
        #ifdef Q_OS_LINUX
        QString ini_path(getenv("HOME"));
        ini_path += "/.config/";
        #endif

        #ifdef Q_OS_WIN32
        QString ini_path(QCoreApplication::applicationDirPath());
        #endif

        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, ini_path);
        QSettings m_settings (QSettings::IniFormat, QSettings::UserScope, "Obywatel GCC", "Guiter Text");
        m_settings.remove("recent");
        MyShareData::m_files_history.clear();
        ui->menuLast_open->clear();
        ui->menuLast_open->setEnabled(false);
    }
    else
    {
        QString dir = action->data().toString();

        QFile file(dir);
        if(file.exists())
            Open(dir);

        MySimpleList<QActionPtr>::iterator iter;
        for(iter = MyShareData::m_files_history.first(); iter != MyShareData::m_files_history.end(); iter++)
        {
            if((*iter).get() == action) break;
        }
        MyShareData::m_files_history.moveOnTop(iter);
    }
    LoadHistoryActions();
}

void MainWindow::favouriteOpen(QAction *action)
{
    if(action == nullptr) return;
    else
    {
        QString dir = action->data().toString();

        QFile file(dir);
        if(file.exists())
            Open(dir);
    }
}

void MainWindow::on_actionNew_triggered()
{
    m_doc->clear();
    MyShareData::m_actual_path.clear();
    MyShareData::m_changed = false;
    setWindowTitle("New file");
    m_text_lines.clear();
    m_column_text.clear();
    onChordsChanged(0);
}

void MainWindow::on_actionOpen_triggered()
{
    //open
    QString newdir;
    newdir = QFileDialog::getOpenFileName(this, tr("Otwórz"), (MyShareData::m_actual_path.isEmpty() ? "/home/krystek/Pulpit/Dokumenty/gitara/TEKSTY/" : MyShareData::m_actual_path), tr("Text Files (*.txt);;All Files (*)"));

    if(!newdir.isEmpty() && newdir != MyShareData::m_actual_path)
        Open(newdir);
}

void MainWindow::on_actionSave_triggered()
{
    //save
    if(MyShareData::m_changed)
    {
        if(MyShareData::m_actual_path.isEmpty())
        {
            QString path = "";
            if(!MyShareData::m_text_bases.isEmpty())
            {
                QStringList keys = MyShareData::m_text_bases.keys();
                path = MyShareData::m_text_bases.value(keys.at(0))->data(0).toString();
            #ifdef Q_OS_WIN
                path = path + "\\" + m_doc->firstBlock().text();
            #endif
            #ifdef Q_OS_LINUX
                path = path + "/" + m_doc->firstBlock().text();
            #endif
            }
            MyShareData::m_actual_path = QFileDialog::getSaveFileName(this, tr("Zapisz"), path, tr("Text Files (*.txt);;All Files (*)"));
        }

        Save();
        ui->statusBar->showMessage("Zapisano do " + MyShareData::m_actual_path, 5000);
    }
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionSave_as_triggered()
{
    MyShareData::m_actual_path = QFileDialog::getSaveFileName(this, tr("Zapisz"), MyShareData::m_actual_path, tr("Text Files (*.txt);;All Files (*)"));

    Save();

    ui->statusBar->showMessage("Zapisano do " + MyShareData::m_actual_path, 5000);
}

void MainWindow::Save()
{
    if(MyShareData::m_text_editable)
    {
        if(!MyShareData::m_actual_path.isEmpty())
        {
            QScrollBar* barv = ui->textEdit->verticalScrollBar();
            QScrollBar* barh = ui->textEdit->horizontalScrollBar();
            int barv_pos = barv->value();
            int barh_pos = barh->value();
            int cursor_pos = ui->textEdit->textCursor().position();

            QFile file(MyShareData::m_actual_path);
            if(file.open(QIODevice::WriteOnly))
            {
                QTextStream out(&file);

                QString text = m_doc->toPlainText();
                QTextStream stream(&text);
                QString line;

                while(!stream.atEnd())
                {
                    line = stream.readLine() + "\n";
                    out << line;
                }

                out.flush();
                file.close();
            }

            MyShareData::m_changed = false;

            Load();
            DividingText();
            ShowText();
            LoadHistoryActions();

            barv->setValue(barv_pos);
            barh->setValue(barh_pos);
            QTextCursor cursor = ui->textEdit->textCursor();
            cursor.setPosition(cursor_pos);
            ui->textEdit->setTextCursor(cursor);
        }
    }
}

void MainWindow::Load(bool add_action)
{
    QFile file(MyShareData::m_actual_path);

    if(file.open(QIODevice::ReadOnly))
    {
        MyShareData::m_column_width = 0;
        m_text_lines.clear();

        QTextStream textStream(&file);
        textStream.setCodec(MyShareData::m_encode.toStdString().c_str());

        QString line, tmp;
        bool chord;

        while(!textStream.atEnd())
        {
            chord = false;
            tmp.clear();
            MySimpleList<Text>* m_line = new MySimpleList<Text>;
            line = textStream.readLine();
            QString::iterator iter;
            for(iter = line.begin(); iter != line.end(); iter++)
            {
                if((*iter) == '[' && !chord)
                {
                    if(!tmp.isEmpty())
                    {
                        Text t(tmp, false);
                        t.setShift(MyShareData::m_chords_shift);
                        m_line->addAtEnd(t);
                        tmp.clear();
                    }
                    chord = true;
                    continue;
                }
                if((*iter) == ']' && chord)
                {
                    if(!tmp.isEmpty())
                    {
                        Text t(tmp, true);
                        t.setShift(MyShareData::m_chords_shift);
                        m_line->addAtEnd(t);
                        tmp.clear();
                    }
                    chord = false;
                    continue;
                }
                tmp += *iter;
            }
            if(!tmp.isEmpty())
            {
                Text t(tmp, false);
                t.setShift(MyShareData::m_chords_shift);
                m_line->addAtEnd(t);
                tmp.clear();
            }
            m_text_lines.addAtEnd(m_line);
        }

        RefreshWidth();

        if(add_action) MyShareData::AddLastOpenAction(MyShareData::m_actual_path);
        ui->statusBar->showMessage("Otworzono " + MyShareData::m_actual_path, 15000);
    }
    else
        ui->statusBar->showMessage("Nie można otworzyć " + MyShareData::m_actual_path + ". Plik przestał istnieć lub jest zablokowany przed odczytem", 50000);

    if(m_text_lines.getSize() >= 2)
    {
        MySimpleList< MySimpleList<Text>* >::iterator iter = m_text_lines.first();
        QString tmp = (*(*iter)->first()).getText() + " - ";
        iter++;
        tmp += (*(*iter)->first()).getText();
        setWindowTitle(tmp);
    }
    else
        setWindowTitle("Guitar Text");
}

void MainWindow::SetShift(int s)
{
    QFontMetricsF font_metrics(m_doc->defaultFont());
    MySimpleList< MySimpleList<Text>* >::iterator line_iter;
    MySimpleList<Text>::iterator iter;
    for(line_iter = m_text_lines.first(); line_iter != m_text_lines.end(); line_iter++)
        for(iter = (*line_iter)->first(); iter != (*line_iter)->end(); iter++)
            if((*iter).chords())
            {
                (*iter).setShift(s);
                (*iter).setWidth(font_metrics.width((*iter).getChordsType()));
            }
}

void MainWindow::SetChordsType(int c)
{
    QFontMetricsF font_metrics(m_doc->defaultFont());
    MySimpleList< MySimpleList<Text>* >::iterator line_iter;
    MySimpleList<Text>::iterator iter;
    for(line_iter = m_text_lines.first(); line_iter != m_text_lines.end(); line_iter++)
        for(iter = (*line_iter)->first(); iter != (*line_iter)->end(); iter++)
            if((*iter).chords())
            {
                (*iter).setChordsType(c);
                (*iter).setWidth(font_metrics.width((*iter).getChordsType()));
            }
}

void MainWindow::RefreshWidth()
{
    QFontMetricsF font_metrics(m_doc->defaultFont());
    MySimpleList< MySimpleList<Text>* >::iterator line_iter;
    MySimpleList<Text>::iterator iter;
    for(line_iter = m_text_lines.first(); line_iter != m_text_lines.end(); line_iter++)
        for(iter = (*line_iter)->first(); iter != (*line_iter)->end(); iter++)
            if((*iter).chords()) (*iter).setWidth(font_metrics.width((*iter).getChordsType()));
            else (*iter).setWidth(font_metrics.width((*iter).getText()));
}

void MainWindow::DividingText()
{
    QFontMetricsF font_metrics(m_doc->defaultFont());
    int lines_in_column = (ui->textEdit->height() - ui->textEdit->horizontalScrollBar()->height() - 2) / (font_metrics.height());
    if(MyShareData::m_auto_column)
        MyShareData::m_columns_count = m_text_lines.getSize() / lines_in_column + 1;
    m_column_text.resize(MyShareData::m_columns_count);

    QString text = "";
    m_column_text.fill("");

    if(!MyShareData::m_text_editable)
    {
        MyShareData::m_column_width = 0;
        int i=0, j = 0, width;
        MySimpleList< MySimpleList<Text>* >::iterator line_iter;
        MySimpleList<Text>::iterator iter;
        for(line_iter = m_text_lines.first(); line_iter != m_text_lines.end(); line_iter++)
        {
            width = 0;
            text.clear();
            for(iter = (*line_iter)->first(); iter != (*line_iter)->end(); iter++)
            {
                width += (*iter).width();
                if((*iter).chords())
                {
                    text.append(QString("<span class = \"akord\">") + (*iter).getChordsType() + QString("</span>"));
                }
                else
                    text.append((*iter).getText());
            }
            text.append("<br />");

            if(width > MyShareData::m_column_width) MyShareData::m_column_width = width + 15;
            if(j < MyShareData::m_columns_count-1 && i % lines_in_column == 0 && i > 0)
                j++;

            m_column_text[j] = m_column_text.at(j) + text;
            i++;
        }
    }
}

void MainWindow::ShowText()
{
    m_scrolling->stop();

    QScrollBar *bar = ui->textEdit->verticalScrollBar();
    bar->setValue(bar->minimum());

    m_doc->clear();

    QString tmp;

    if(!MyShareData::m_text_editable)
    {
        tmp = "<table><tr>";
        for(int i = 0; i < MyShareData::m_columns_count; i++)
        {

            tmp += "<td width=" + QString::number(MyShareData::m_column_width) + ">";
            tmp += m_column_text.at(i);
            tmp += "</td>";
        }

        tmp += "</tr></table>";

        m_doc->setHtml(tmp);
    }
    else
    {
        MySimpleList< MySimpleList<Text>* >::iterator line_iter;
        MySimpleList<Text>::iterator iter;
        for(line_iter = m_text_lines.first(); line_iter != m_text_lines.end(); line_iter++)
        {

            for(iter = (*line_iter)->first(); iter != (*line_iter)->end(); iter++)
            {
                if((*iter).chords())
                    ui->textEdit->insertPlainText("[" + (*iter).getText() + "]");
                else
                    ui->textEdit->insertPlainText((*iter).getText());
            }

            ui->textEdit->insertPlainText("\n");
        }
    }
    MyShareData::m_changed = false;
}

void MainWindow::SetCentralText()
{
    m_tool_bar->show();
    m_layout->setContentsMargins(10, 5, 10, 5);
}

void MainWindow::SetFullScreenText()
{
    m_tool_bar->hide();
    m_layout->setContentsMargins(0, 0, 0, 0);
}

void MainWindow::LoadHistoryActions()
{
    ui->menuLast_open->setEnabled(true);
    ui->menuLast_open->clear();

    MySimpleList<QActionPtr>::iterator iter;

    for(iter = MyShareData::m_files_history.first(); iter != MyShareData::m_files_history.end(); iter++)
    {
        ui->menuLast_open->addAction(&(*(*iter)));
    }

    m_clear_history = new QAction(QString("Wyczyść wszystko"), nullptr);
    ui->menuLast_open->addSeparator();
    ui->menuLast_open->addAction(m_clear_history);
}

void MainWindow::MoveHorizontalScrollBar()
{
    QScrollBar *bar = ui->textEdit->horizontalScrollBar();
    if(bar->maximum() > 0)
        bar->setValue(bar->value() + MyShareData::m_column_width);
}

void MainWindow::SetProperties()
{
    m_column_text.resize(MyShareData::m_columns_count);
    //------------Kolor tekstu------------
    QPalette textPalette = ui->textEdit->palette();
    textPalette.setColor(QPalette::Text, MyShareData::m_text_color);
    ui->textEdit->setPalette(textPalette);
    //------------Kolor tła------------
    textPalette.setColor(QPalette::Base, MyShareData::m_background_color);
    ui->textEdit->setPalette(textPalette);
    //------------Ustawienie wielkości czcionki------------
    m_doc->setDefaultFont(MyShareData::m_font);
    ui->textEdit->setDocument(m_doc);
}

void MainWindow::LoadFavourites()
{
    ui->menuFavourite->clear();

    ListSortByWeight<QString>::iterator iter;
    QFileInfo file;
    QAction* action;
    int i = 0;
    for(iter = MyShareData::m_favourites.first(); iter != MyShareData::m_favourites.end() && i < MyShareData::m_favourites_max_size; iter++)
    {
        file.setFile(*iter);
        if(file.exists())
        {
            action = new QAction(file.fileName() + QString(" (%1)").arg(iter.weight()), nullptr);
            action->setData(*iter);
            ui->menuFavourite->addAction(action);
            i++;
        }
        else
            MyShareData::m_favourites.remove(iter);
    }
}

void MainWindow::SetShortcuts()
{
    ui->actionNew->setShortcut(MyShareData::kse_new);
    ui->actionOpen->setShortcut(MyShareData::kse_open);
    ui->actionSave->setShortcut(MyShareData::kse_save);
    ui->actionSave_as->setShortcut(MyShareData::kse_save_as);
    ui->actionRefresh->setShortcut(MyShareData::kse_reload);
    ui->actionNext_Chords->setShortcut(MyShareData::kse_chords_up);
    ui->actionPrevious_Chords->setShortcut(MyShareData::kse_chords_down);
    ui->actionNext_schift_chords->setShortcut(MyShareData::kse_shift_up);
    ui->actionPrevious_schift_chords->setShortcut(MyShareData::kse_shift_down);
    ui->actionFull_Screen->setShortcut(MyShareData::kse_fullscreen);
    ui->actionProperties->setShortcut(MyShareData::kse_properties);
    ui->actionSearch->setShortcut(MyShareData::kse_find);
    ui->actionQuickOptions->setShortcut(MyShareData::kse_quick_options);
    ui->actionExit->setShortcut(MyShareData::kse_close);
}

void MainWindow::on_actionRefresh_triggered()
{
    if(MyShareData::m_text_editable) Save();

    m_doc->clear();

    Load();
    DividingText();
    ShowText();
}

void MainWindow::on_actionNext_Chords_triggered()
{
    onChordsChanged(MyShareData::m_chords + 1);
}

void MainWindow::on_actionPrevious_Chords_triggered()
{
    onChordsChanged(MyShareData::m_chords - 1);
}

void MainWindow::on_actionNext_schift_chords_triggered()
{
    onShiftChanged(MyShareData::m_chords_shift + 1);
}

void MainWindow::on_actionPrevious_schift_chords_triggered()
{
    onShiftChanged(MyShareData::m_chords_shift - 1);
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_actionFull_Screen_triggered()
{
    if(MyShareData::m_fullscreen)
    {
        SetCentralText();
        if(this->windowState().testFlag(Qt::WindowFullScreen)) //testFlag - czy flaga jest ustawiona
        {
            this->setWindowState(this->windowState() ^ Qt::WindowFullScreen);
        }
        ui->statusBar->show();
        MyShareData::m_fullscreen = false;
    }
    else
    {
        SetFullScreenText();
        if(!this->windowState().testFlag(Qt::WindowFullScreen))
        {
            this->setWindowState(this->windowState() ^ Qt::WindowFullScreen);
        }
        ui->statusBar->hide();
        MyShareData::m_fullscreen = true;
    }
}

void MainWindow::on_actionProperties_triggered()
{
    PropertiesDialog dialog;
    dialog.setModal(true);
    dialog.exec();

    if(dialog.result() == QDialog::Accepted)
    {
        m_search_dialog->setCurrentEncode(dialog.m_encode);
        onEncodeChanged(dialog.m_encode);

        MyShareData::m_chords_shift = dialog.m_shift;
        MyShareData::m_chords = dialog.m_chords;

        if(dialog.m_fullscreen != MyShareData::m_fullscreen)
        {
            on_actionFull_Screen_triggered();
            MyShareData::m_fullscreen = dialog.m_fullscreen;
        }
        MyShareData::m_auto_column = dialog.m_auto_column;
        if(MyShareData::m_auto_column) ui->textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        else ui->textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

        MyShareData::m_columns_count = dialog.m_column;

        MyShareData::m_files_history.setMaxSize(dialog.m_max_last_open);
        if(MyShareData::m_favourites_max_size != dialog.m_max_favourites)
        {
            MyShareData::m_favourites_max_size = dialog.m_max_favourites;
            LoadFavourites();
        }

        MyShareData::m_text_color = dialog.m_text;
        MyShareData::m_background_color = dialog.m_background;
        QPalette textPalette = ui->textEdit->palette();
        textPalette.setColor(QPalette::Text, MyShareData::m_text_color);
        textPalette.setColor(QPalette::Base, MyShareData::m_background_color);
        ui->textEdit->setPalette(textPalette);

        MyShareData::m_chords_color = dialog.m_chords_color;
        m_doc->setDefaultStyleSheet(".akord{color: " + MyShareData::m_chords_color.name() + "}");

        if(MyShareData::m_font != dialog.m_font)
        {
            int sizeOld = (MyShareData::m_font.pixelSize() != -1 ? MyShareData::m_font.pixelSize() : MyShareData::m_font.pointSize());
            int sizeNew = (dialog.m_font.pixelSize() != -1 ? dialog.m_font.pixelSize() : dialog.m_font.pointSize());
            MyShareData::m_column_width = (double) MyShareData::m_column_width * sizeNew / sizeOld;
            RefreshWidth();
            MyShareData::m_font = dialog.m_font;
            m_doc->setDefaultFont(MyShareData::m_font);
        }

        m_quick_options->setAll();
        SetShortcuts();
        DividingText();
        ShowText();

        m_search_dialog->resetModels();
    }
}

void MainWindow::onChordsChanged(int arg1)
{
    if(arg1 == 0)
    {
        MyShareData::m_text_editable = true;
        ui->actionSave->setEnabled(true);
        ui->textEdit->setReadOnly(false);

        m_scrolling->stop();
        ui->textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }
    else if(arg1 > 0)
    {
        MyShareData::m_text_editable = false;
        ui->actionSave->setEnabled(false);
        ui->textEdit->setReadOnly(true);
        ui->textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    if(arg1 >= 0)
    {
        MyShareData::m_chords = arg1;

        m_doc->clear();
        SetChordsType(MyShareData::m_chords-1);
        SetShift(MyShareData::m_chords_shift);
        DividingText();
        ShowText();
    }
}

void MainWindow::onShiftChanged(int arg1)
{
    MyShareData::m_chords_shift = (arg1 < 0) ? 12 + arg1 % 12 : arg1 % 12;;

    SetShift(MyShareData::m_chords_shift);
    DividingText();
    ShowText();
}

void MainWindow::onSpeedChanged(int arg1)
{
    MyShareData::m_scrolling_speed = arg1;

    if(MyShareData::m_scrolling_speed == 0)
    {
        m_scrolling->stop();
    }
    else if(m_scrolling->isActive())
    {
        //timer start z nową szybkością
        onStartScrolling();
    }
}

void MainWindow::onDelayChanged(int arg1)
{
    MyShareData::m_scrolling_delay = arg1;
}

void MainWindow::onStartScrolling()
{
    m_scrolling_delay->start(MyShareData::m_scrolling_delay * 1000);
}

void MainWindow::startScrolling()
{
    m_scrolling_delay->stop();
    if(MyShareData::m_scrolling_speed > 0)
        m_scrolling->start(MyShareData::m_scrolling_speed);
}

void MainWindow::onStopScrolling()
{
    m_scrolling_delay->stop();
    m_scrolling->stop();
}

void MainWindow::on_textEdit_textChanged()
{
    MyShareData::m_changed = true;
}

void MainWindow::onEncodeChanged(const QString &arg1)
{
    MyShareData::m_encode = arg1;

    m_doc->clear();

    Load(false);
    DividingText();
    ShowText();
}

void MainWindow::addFavourite()
{
    MyShareData::AddFavouriteAction(MyShareData::m_actual_path, 1);
    LoadFavourites();
    m_favourite_timer->stop();
}

void MainWindow::on_actionGuitarText_triggered()
{
    QMessageBox::about(this, "O GuitarText", "GuitarText " + QString(GUITARTEXT_VERSION) + "\nProgram został stworzony dla łatwiejszego\nprzeglądanie tekstów piosenek i akordów gitarowych.\nNapisany przez Obywatel GCC" );
}

void MainWindow::on_actionSearch_triggered()
{
    if(m_search_dialog->isHidden())
        m_search_dialog->show();
    else
    {
        if(m_search_dialog->isDisappear())
        {
            m_search_dialog->activateWindow();
            m_search_dialog->startShow();
        }
        else
            m_search_dialog->hide();
    }
}

void MainWindow::on_actionQuickOptions_triggered()
{
    if(m_quick_options->isHidden())
    {
        m_quick_options->setAll();
        m_quick_options->show();
    }
    else
    {
        if(m_quick_options->isDisappear())
        {
            m_quick_options->activateWindow();
            m_quick_options->setAll();
            m_quick_options->startShow();
        }
        else
            m_quick_options->hide();
    }
}
