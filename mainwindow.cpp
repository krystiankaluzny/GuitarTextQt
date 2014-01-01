#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>
#include <QFileDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QMessageBox>
#include <QDebug>
#include "chord.h"
#include "properties/propertiesdialog.h"

/*TODO
 *zrobić opcję ze ścieżkami z tekstami
 *stworzyć posortowany wektor plików
 *monitorować dodawane pliki i dopisywać je do wektora
 *przeszukiwać listę
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_dir(""),
    m_doc(new QTextDocument),
    m_timer(new QTimer(this)),
    m_changed(false)
{
    setWindowTitle("Guitar Text");

    ui->setupUi(this);

    ui->textEdit->installEventFilter(this);

    //------------Połączenia------------
    connect(m_timer, SIGNAL(timeout()), this, SLOT(moveScrollBar()));

    //------------Liczba kolumn------------
    if(m_settings.contains("editor/colimn_count"))
    {
        m_columns_count = m_settings.value("editor/colimn_count", 3).toInt();
        m_column_text.resize(m_columns_count);
    }
    else
    {
        m_columns_count = 3;
        m_settings.setValue("editor/colimn_count", m_columns_count);
    }
    //------------Tempo przewijania------------
    if(m_settings.contains("editor/scrolling_speed"))
        m_scrolling_speed = m_settings.value("editor/scrolling_speed", 8).toInt();
    else
    {
        m_scrolling_speed = 8;
        m_settings.setValue("editor/scrolling_speed", m_scrolling_speed);
    }
    //------------Pełny ekran------------
    if(m_settings.contains("app/fullscreen"))
        m_fullscreen = m_settings.value("app/fullscreen", false).toBool();
    else
    {
        m_fullscreen = 8;
        m_settings.setValue("app/fullscreen", m_fullscreen);
    }

    //------------Ustawienie identyfikatera html akordu------------
    m_doc->setDefaultStyleSheet(".akord{color: #ff0000}");


    //------------Ustawienie wielkości czcionki------------
    if(m_settings.contains("editor/font"))
    {
        QFont font = m_settings.value("editor/font", m_doc->defaultFont()).value<QFont>();
        m_doc->setDefaultFont(font);
        ui->textEdit->setDocument(m_doc);
    }
    else
    {
        QFont font = m_doc->defaultFont();
        font.setPointSize(14);
        m_doc->setDefaultFont(font);
        ui->textEdit->setDocument(m_doc);
        m_settings.setValue("editor/font", font);
    }

    //------------Wypełnianie boxa z kodowaniami------------
    ui->comboBox_encoder->clear();
    ui->comboBox_encoder->addItem("UTF-8");
    ui->comboBox_encoder->addItem("UTF-16");
    ui->comboBox_encoder->addItem("Windows-1250");
    ui->comboBox_encoder->addItem("ISO 8859-1");
    ui->comboBox_encoder->addItem("ISO 8859-2");

    if(m_settings.contains("editor/code"))
    {
        ui->comboBox_encoder->setCurrentText(m_settings.value("editor/code", "UTF-8").toString());
    }
    else
    {
        m_settings.setValue("editor/code", ui->comboBox_encoder->currentText());
    }

    //------------Nie zapętlanie------------
    ui->checkBox_loop->setChecked(false);
    m_settings.setValue("editor/loop", false);

    //------------Liczba kolumn------------
    m_column_text.resize(m_columns_count);

    on_spinBox_chords_valueChanged(1);

    m_settings.setValue("editor/chords", 1);
    m_settings.setValue("editor/shift", 0);

    //------------Ustawienei Centalnego Widgetu------------
    m_central = new QWidget(this);
    this->setCentralWidget(m_central);
    m_layout = new QVBoxLayout;
    m_layout->addWidget(ui->textEdit);
    m_central->setLayout(m_layout);

    //------------Ustawienie Tool Barów------------
    ui->mainToolBar->setWindowTitle("Tools Bar");
    AddViewToolBar();
    AddScrollToolBar();
    SetCentralText();

    //------------Ustawienie Histori Otwartych Plików------------
    if(m_settings.contains("app/files_history_max_size"))
    {
        m_files_history.setMaxSize(m_settings.value("app/files_history_max_size", 10).toInt());
    }
    else
    {
        m_files_history.setMaxSize(10);
        m_settings.setValue("app/files_history_max_size", 10);
    }
    ui->menuLast_open->setEnabled(false);
    connect(ui->menuLast_open, SIGNAL(triggered(QAction*)), this, SLOT(historyFileOpen(QAction*)));

    //------------Drag and Drop------------
    setAcceptDrops(true); //pozwalamy na przechwytywanie elementów

    //------------Kolor tekstu------------
    if(m_settings.contains("editor/text_color"))
    {
        QPalette textPalette = ui->textEdit->palette();
        textPalette.setColor(QPalette::Text, m_settings.value("editor/text_color", QColor(255, 255, 255)).value<QColor>());
        ui->textEdit->setPalette(textPalette);
    }
    else
    {
        QPalette textPalette = ui->textEdit->palette();
        textPalette.setColor(QPalette::Text, QColor(255, 255, 255));
        ui->textEdit->setPalette(textPalette);
        m_settings.setValue("editor/text_color", textPalette.color(QPalette::Text));
    }

    //------------Kolor tła------------
    if(m_settings.contains("editor/background_color"))
    {
        QPalette textPalette = ui->textEdit->palette();
        textPalette.setColor(QPalette::Base, m_settings.value("editor/background_color", QColor(0, 0, 0)).value<QColor>());
        ui->textEdit->setPalette(textPalette);
    }
    else
    {
        QPalette textPalette = ui->textEdit->palette();
        textPalette.setColor(QPalette::Base, QColor(0, 0, 0));
        ui->textEdit->setPalette(textPalette);
        m_settings.setValue("editor/background_color", QColor(0, 0, 0));
    }
}

MainWindow::~MainWindow()
{
    m_settings.setValue("editor/loop", false);
    m_settings.setValue("editor/chords", 1);
    m_settings.setValue("editor/shift", 0);
    m_settings.setValue("app/fullscreen", false);

    m_timer->stop();
    delete m_timer;
    delete m_layout;
    delete m_central;
    delete m_scroll_tool_bar;
    delete m_view_tool_bar;
    delete m_doc;
    delete ui;
}

bool MainWindow::eventFilter(QObject *o, QEvent *e)
{
    if(o == ui->textEdit)
    {
        if(e->type() == QEvent::KeyPress)
        {
            QKeyEvent* key_event = static_cast<QKeyEvent*>(e);

            if(key_event->key() == Qt::Key_Space && ui->spinBox_chords->value() > 0)
            {
                if(m_timer->isActive())
                {
                    m_timer->stop();
                }
                else if(ui->spinBox_speed->value() > 0)
                {
                    m_timer->start();
                }

                return true;
            }

            return false;
        }
        else if(e->type() == QEvent::Resize)
        {
            PartitionText();
            ShowText();
        }
    }

    return QMainWindow::eventFilter(o, e);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    QUrl path = event->mimeData()->urls().at(0);

    if(path.isLocalFile()) //czy to jest ścieżka lokalna (nie sprawdza czy to jest plik)
    {
        QFileInfo file(path.toString(QUrl::RemoveScheme));

        QString name = file.absoluteFilePath();
        if(file.isFile() && file.suffix() == "txt")
        {
            qDebug() << name;
            event->acceptProposedAction();
        }
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QFileInfo file(event->mimeData()->urls().at(0).toString(QUrl::RemoveScheme));

    if(m_dir != file.absoluteFilePath())
    {
        m_dir = file.absoluteFilePath();

        AddAction(m_dir);
        Load();
        PartitionText();
        ShowText();
        LoadHistoryActions();

        ui->spinBox_shift->setValue(0);
    }
}

void MainWindow::moveScrollBar()
{
    QScrollBar *bar = ui->textEdit->verticalScrollBar();

    if(bar != NULL)
    {
        if(bar->value() == bar->maximum() && ui->checkBox_loop->isChecked())
        {
            bar->setValue(bar->minimum());
        }
        else
        {
            bar->setValue(bar->value() + 1 + m_scrolling_speed/10);
        }
    }
}

void MainWindow::historyFileOpen(QAction *action)
{
    QString dir = action->data().toString();

    if(dir != m_dir)
    {
        m_dir = dir;
        Load();
        PartitionText();
        ShowText();

        MySimpleList<QActionPtr>::iterator iter;
        for(iter = m_files_history.first(); iter != m_files_history.end(); iter++)
        {
            if((*iter).get() == action) break;
        }
        m_files_history.moveOnTop(iter);

        LoadHistoryActions();
    }
}

void MainWindow::on_actionNew_triggered()
{
    m_doc->clear();
    m_dir.clear();
    m_changed = false;
    setWindowTitle("New file");
    ui->spinBox_chords->setValue(0); //wywołanie chordsChange(0);
}

void MainWindow::on_actionOpen_triggered()
{
    //open
    QString newdir;
    newdir = QFileDialog::getOpenFileName(this, tr("Otwórz"), (m_dir.isEmpty() ? "/home/krystek/Pulpit/Dokumenty/gitara/TEKSTY/" : m_dir), tr("Text Files (*.txt);;All Files (*)"));

    if(!newdir.isEmpty() && newdir != m_dir)
    {
        m_doc->clear();
        m_dir = newdir;

        AddAction(m_dir);

        Load();
        PartitionText();
        ShowText();
        LoadHistoryActions();

        ui->spinBox_shift->setValue(0);

        ui->statusBar->showMessage("Otworzono " + m_dir, 5000);
    }
}

void MainWindow::on_actionSave_triggered()
{
    //save
    if(m_changed)
    {
        if(m_dir.isEmpty())
        {
            m_dir = QFileDialog::getSaveFileName(this, tr("Zapisz"), QString(), tr("Text Files (*.txt);;All Files (*)"));
        }
        Save();
        ui->statusBar->showMessage("Zapisano do " + m_dir, 5000);
    }
}

void MainWindow::on_actionFont_triggered()
{
    //font
    bool ok;
    QFont font = m_doc->defaultFont();
    font = QFontDialog::getFont(&ok, font, this);

    if(ok)
    {
        m_settings.setValue("editor/font", font);

        m_doc->setDefaultFont(font);

        PartitionText();
        ShowText();
    }
}

void MainWindow::on_actionText_Color_triggered()
{
    //text color
    QPalette textPalette = ui->textEdit->palette();
    QColor old = textPalette.color(QPalette::Text);
    QColor textColor = QColorDialog::getColor(old, this);

    textPalette.setColor(QPalette::Text, (textColor.isValid() ? textColor : old));

    m_settings.setValue("editor/text_color", textPalette.color(QPalette::Text));

    ui->textEdit->setPalette(textPalette);
}

void MainWindow::on_actionBackground_Color_triggered()
{
    //background color
    QPalette textPalette = ui->textEdit->palette();
    QColor old = textPalette.color(QPalette::Base);
    QColor backgroundColor = QColorDialog::getColor(old, this);

    textPalette.setColor(QPalette::Base, (backgroundColor.isValid() ? backgroundColor : old));

    m_settings.setValue("editor/background_color", textPalette.color(QPalette::Base));

    ui->textEdit->setPalette(textPalette);
}

void MainWindow::on_actionExit_triggered()
{
    //Exit
    //TODO zapytanie czy zapisać plik jeśli zmodyfikowany
    close();
}

void MainWindow::on_actionSave_as_triggered()
{

    QString old = m_dir;
    m_dir = QFileDialog::getSaveFileName(this, tr("Zapisz"), QString(), tr("Text Files (*.txt);;All Files (*)"));

    Save();

    ui->statusBar->showMessage("Zapisano do " + m_dir, 5000);
}

void MainWindow::Save()
{
    if(ui->spinBox_chords->value() == 0)
    {
        if(!m_dir.isEmpty())
        {
            QFile file(m_dir);
            if(file.open(QIODevice::WriteOnly))
            {
                QTextStream out(&file);

                QString text = m_doc->toPlainText();
                QTextStream stream(&text);
                QString line;

                m_text_lines.clear();

                while(!stream.atEnd())
                {
                    line = stream.readLine() + "\n";
                    m_text_lines.append(line);
                    out << line;
                }

                out.flush();
                file.close();
            }

            m_changed = false;

            Load();
            PartitionText();
            ShowText();
        }

    }
}

void MainWindow::Load()
{
    m_text_lines.clear();

    QFile file(m_dir);

    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream textStream(&file);
        textStream.setCodec(ui->comboBox_encoder->currentText().toStdString().c_str());

        while(!textStream.atEnd())
        {
            m_text_lines.append(textStream.readLine());
        }
    }

    if(m_text_lines.size() >= 2)
    {
        setWindowTitle(m_text_lines.at(0) + " - " + m_text_lines.at(1));
    }
    else
    {
        setWindowTitle("Guitar Text");
    }
}

void MainWindow::PartitionText()
{
    QFontMetricsF font_metrics(m_doc->defaultFont());
    int lines_in_column = (ui->textEdit->height() - 2)/ (font_metrics.height()); //w ostatniej kolumnie może być więcej

    QString text = "";
    m_column_text.fill("");

    QStringList start, end, middle;
    bool snobrack = false; //linijka nie zaczyna się nawiasem

    if(ui->spinBox_chords->value() > 0)
    {
        for(int i = 0, j = 0; i < m_text_lines.size(); i++)
        {
            text = m_text_lines.at(i); //kolejne linijki

            start = text.split("[");
            if(text[0] != '[')
                snobrack = true;

            text.clear();

            if(start.size() == 1) //linijka nie zawiera wstawek akordów
                text.append(start[0]);

            for(int k = 1; k < start.size(); k++)
            {
                if(snobrack) //linijka nie zaczyna się od akordów
                {
                    text.append(start[0]);
                    snobrack = false;
                }

                text.append("[");

                end = start[k].split("]");

                switch(end.size())
                {
                case 2:
                    if(!end[0].isEmpty())
                    {
                        middle = end[0].split("|");

                        int num = ui->spinBox_chords->value(); //powinno być wieksze od 0

                        if(num <= middle.size() && num >= 1)
                        {
                            QString tmp = ShiftChordString(middle[num - 1].split(" "), ui->spinBox_shift->value());
                            text.append(tmp + "]");
                        }
                        else
                        {
                            QString tmp = ShiftChordString(middle[0].split(" "), ui->spinBox_shift->value());
                            text.append(tmp + "]");
                        }
                    }

                    text.append(end[1]);
                    break;
                case 1:
                    text.append(end[0]);
                    break;
                }
            }

            text.replace(QString("["), QString("<span class = \"akord\">"));
            text.replace(QString("]"), QString("</span>"));
            text.append("<br />");

            if(j < m_columns_count-1 && i % lines_in_column == 0 && i > 0)
            {
                j++;
            }

            m_column_text[j] = m_column_text.at(j) + text;

            text.clear();
        }
    }
}

void MainWindow::ShowText()
{
    m_timer->stop();

    QScrollBar *bar = ui->textEdit->verticalScrollBar();
    bar->setValue(bar->minimum());

    m_doc->clear();

    QString tmp;

    if(ui->spinBox_chords->value() > 0)
    {
        tmp = "<table><tr>";
        for(int i = 0; i < m_columns_count; i++)
        {

            tmp += "<td>";
            tmp += m_column_text.at(i);
            tmp += "</td>";
        }

        tmp += "</tr></table>";

        m_doc->setHtml(tmp);
    }
    else
    {
        for(int i = 0; i < m_text_lines.size(); i++)
        {
            ui->textEdit->insertPlainText(m_text_lines.at(i) + "\n");
        }
    }

    m_changed = false;
}

void MainWindow::HideAllBars()
{
    ui->mainToolBar->hide();
    m_view_tool_bar->hide();
    m_scroll_tool_bar->hide();
}

void MainWindow::ShowAllBars()
{
    ui->mainToolBar->show();
    m_view_tool_bar->show();
    m_scroll_tool_bar->show();
}

void MainWindow::AddViewToolBar()
{
    m_view_tool_bar = new QToolBar(this);
    m_view_tool_bar->setWindowTitle("View Bar");
    m_view_tool_bar->addWidget(ui->view_widget);

    this->addToolBar(m_view_tool_bar);
}

void MainWindow::AddScrollToolBar()
{
    m_scroll_tool_bar = new QToolBar(this);
    m_scroll_tool_bar->setWindowTitle("Scroll Tools");
    m_scroll_tool_bar->addWidget(ui->scroling_widget);

    this->addToolBar(m_scroll_tool_bar);
}

void MainWindow::SetCentralText()
{
    m_layout->setContentsMargins(10, 5, 10, 5);
}

void MainWindow::SetFullScreenText()
{
    m_layout->setContentsMargins(0, 0, 0, 0);
}

void MainWindow::LoadHistoryActions()
{
    ui->menuLast_open->setEnabled(true);
    ui->menuLast_open->clear();

    MySimpleList<QActionPtr>::iterator iter;

    for(iter = m_files_history.first(); iter != m_files_history.end(); iter++)
    {
        ui->menuLast_open->addAction(&(*(*iter)));
    }
}

void MainWindow::AddAction(QString path)
{
    QFileInfo file_info(path);
    QAction* act = new QAction(file_info.fileName(), nullptr);
    act->setData(path);

    if(m_files_history.getSize() == m_files_history.getMaxSize())
    {
        MySimpleList<QActionPtr>::iterator iter_last, iter_first;
        iter_last = m_files_history.last();
        iter_first = m_files_history.first();

        //usuwamy ostatni bo lista jest czykliczna
        (*iter_last).reset();

        QActionPtr action_ptr(act);
        m_files_history.addAtBeginning(action_ptr);
    }
    else
    {
        QActionPtr action_ptr(act);
        m_files_history.addAtBeginning(action_ptr);
    }
}

void MainWindow::on_actionRefresh_triggered()
{
    if(ui->spinBox_chords->value() == 0)
    {
        Save();
    }

    m_doc->clear();

    Load();
    PartitionText();
    ShowText();
}

void MainWindow::on_actionPrevious_Chords_triggered()
{
    if(ui->spinBox_chords->value() > ui->spinBox_chords->minimum())
        ui->spinBox_chords->stepDown();
}

void MainWindow::on_actionNext_Chords_triggered()
{
    if(ui->spinBox_chords->value() < ui->spinBox_chords->maximum())
        ui->spinBox_chords->stepUp();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_spinBox_column_valueChanged(int arg1)
{
    m_columns_count = arg1;

    m_settings.setValue("editor/colimn_count", m_columns_count);

    m_column_text.resize(m_columns_count);

    PartitionText();
    ShowText();
}

void MainWindow::on_actionNext_schift_chords_triggered()
{
    if(ui->spinBox_shift->value() < ui->spinBox_shift->maximum())
        ui->spinBox_shift->stepUp();
    else
        ui->spinBox_shift->setValue(0);
}

void MainWindow::on_actionPrevious_schift_chords_triggered()
{
    if(ui->spinBox_shift->value() > ui->spinBox_shift->minimum())
        ui->spinBox_shift->stepDown();
    else
        ui->spinBox_shift->setValue(11);
}

void MainWindow::on_actionFull_Screen_triggered()
{
    if(m_fullscreen)
    {
        ShowAllBars();
        SetCentralText();
        if(this->windowState().testFlag(Qt::WindowFullScreen)) //testFlag - czy flaga jest ustawiona
        {
            this->setWindowState(this->windowState() ^ Qt::WindowFullScreen);
        }
        ui->statusBar->show();
        m_fullscreen = false;
    }
    else
    {
        HideAllBars();
        SetFullScreenText();
        if(!this->windowState().testFlag(Qt::WindowFullScreen))
        {
            this->setWindowState(this->windowState() ^ Qt::WindowFullScreen);
        }
        ui->statusBar->hide();
        m_fullscreen = true;
    }

    m_settings.setValue("app/fullscreen", m_fullscreen);
}

void MainWindow::on_actionSwap_colors_triggered()
{
    QPalette textEditPalette = ui->textEdit->palette();
    QColor text = textEditPalette.color(QPalette::Text);
    QColor background = textEditPalette.color(QPalette::Base);

    textEditPalette.setColor(QPalette::Base, text);
    textEditPalette.setColor(QPalette::Text, background);

    ui->textEdit->setPalette(textEditPalette);
}

void MainWindow::on_actionProperties_triggered()
{
    TreeItem *item = createDirTree("/home/krystek/test2", nullptr);
    m_text_bases.insert("test", item);
    TreeItem *item2 = createDirTree("/home/krystek/test2", nullptr);
    m_text_bases.insert("test2", item2);

    PropertiesDialog dialog(m_settings, &m_text_bases);
    dialog.setModal(true);
    dialog.exec();

    if(dialog.result() == QDialog::Accepted)
    {
        ui->comboBox_encoder->setCurrentText(dialog.m_encode);
        ui->spinBox_shift->setValue(dialog.m_shift);
        ui->spinBox_chords->setValue(dialog.m_chords);
        ui->spinBox_column->setValue(dialog.m_column);
        this->m_fullscreen = dialog.m_fullscreen;
        m_files_history.setMaxSize(dialog.m_max_last_open);
        ui->spinBox_speed->setValue(dialog.m_speed);

        QPalette textPalette = ui->textEdit->palette();

        textPalette.setColor(QPalette::Text, dialog.m_text);
        textPalette.setColor(QPalette::Base, dialog.m_background);

        ui->textEdit->setPalette(textPalette);
    }
}

void MainWindow::on_spinBox_chords_valueChanged(int arg1)
{
    if(arg1 == 0)
    {
        ui->actionSave->setEnabled(true);
        ui->textEdit->setReadOnly(false);

        m_timer->stop();
    }
    else
    {
        ui->actionSave->setEnabled(false);
        ui->textEdit->setReadOnly(true);
    }
    m_settings.setValue("editor/chords", arg1);

    m_doc->clear();
    PartitionText();
    ShowText();
}

void MainWindow::on_spinBox_shift_valueChanged(int arg1)
{
    m_settings.setValue("editor/shift", arg1);

    PartitionText();
    ShowText();
}

void MainWindow::on_spinBox_speed_valueChanged(int arg1)
{
    m_scrolling_speed = arg1;

    m_settings.setValue("editor/scroling_speed", m_scrolling_speed);

    if(m_scrolling_speed == 0)
    {
        m_timer->stop();
    }
    else if(m_timer->isActive())
    {
        //timer start z nową szybkością
        on_pushButton_start_clicked();
    }
}

void MainWindow::on_pushButton_start_clicked()
{
    if(ui->spinBox_speed->value() > 0)
    {
        m_timer->start(720 - 20 * m_scrolling_speed);
    }
}

void MainWindow::on_pushButton_stop_clicked()
{
    m_timer->stop();
}

void MainWindow::on_textEdit_textChanged()
{
    m_changed = true;
}

void MainWindow::on_comboBox_encoder_currentIndexChanged(const QString &arg1)
{
    m_settings.setValue("editor/code", arg1);

    m_doc->clear();

    Load();
    PartitionText();
    ShowText();
}
