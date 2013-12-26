#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>
#include <QFileDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QMessageBox>
#include <QDebug>
#include "chord.h"
//#include <magic.h> //GetMimeType

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_dir(""),
    m_doc(new QTextDocument),
    m_timer(new QTimer(this)),
    m_columns_count(3),
    m_scrolling_speed(8),
    m_changed(false),
    m_fullscreen(false)
{
    ui->setupUi(this);

    //------------Ustawienie identyfikatera html akordu------------
    m_doc->setDefaultStyleSheet(".akord{color: #ff0000}");


    //------------Ustawienie wielkości czcionki------------
    QFont font = m_doc->defaultFont();
    font.setPointSize(14);
    m_doc->setDefaultFont(font);
    ui->textEdit->setDocument(m_doc);


    //------------Wypełnianie boxa z kodowaniami------------
    ui->comboBox->addItem("UTF-8");
    ui->comboBox->addItem("UTF-16");
    ui->comboBox->addItem("Windows-1250");
    ui->comboBox->addItem("ISO 8859-1");
    ui->comboBox->addItem("ISO 8859-2");
    setWindowTitle("Guitar Text");


    //------------Połączenia------------
    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(textChange()));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(encodingChange(int)));
    connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(chordsChange(int)));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(moveScrollBar()));
    connect(ui->spinBox_2, SIGNAL(valueChanged(int)), this, SLOT(setScrollingSpeed(int)));

    //------------Nie zapętlanie------------
    ui->cB_loop->setChecked(false);

    //------------Liczba kolumn------------
    m_column_text.resize(m_columns_count);

    chordsChange(1);

    ui->textEdit->installEventFilter(this);

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
    m_files_history.setMaxSize(10);
    ui->menuLast_open->setEnabled(false);
    connect(ui->menuLast_open, SIGNAL(triggered(QAction*)), this, SLOT(historyFileOpen(QAction*)));

    //------------Drag and Drop------------
    setAcceptDrops(true); //pozwalamy na przechwytywanie elementów
}

MainWindow::~MainWindow()
{
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

            if(key_event->key() == Qt::Key_Space && ui->spinBox->value() > 0)
            {
                if(m_timer->isActive())
                {
                    m_timer->stop();
                }
                else if(ui->spinBox_2->value() > 0)
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

        ui->sb_shift->setValue(0);
    }
}

void MainWindow::textChange()
{
    m_changed = true;
}

void MainWindow::encodingChange(int index)
{
    Q_UNUSED(index)

    m_doc->clear();

    Load();
    PartitionText();
    ShowText();
}

void MainWindow::chordsChange(int chords)
{
    if(chords == 0)
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
    m_doc->clear();
    PartitionText();
    ShowText();
}

void MainWindow::moveScrollBar()
{
    QScrollBar *bar = ui->textEdit->verticalScrollBar();

    if(bar != NULL)
    {
        if(bar->value() == bar->maximum() && ui->cB_loop->isChecked())
        {
            bar->setValue(bar->minimum());
        }
        else
        {
            bar->setValue(bar->value() + 1 + m_scrolling_speed/10);
        }
    }
}

void MainWindow::setScrollingSpeed(int speed)
{
    m_scrolling_speed = speed;

    if(m_scrolling_speed == 0)
    {
        m_timer->stop();
    }
    else if(m_timer->isActive())
    {
        //timer start z nową szybkością
        on_pushButton_clicked();
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
    ui->spinBox->setValue(0); //wywołanie chordsChange(0);
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

        ui->sb_shift->setValue(0);
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
    ui->textEdit->setPalette(textPalette);
}

void MainWindow::on_actionBackground_Color_triggered()
{
    //background color
    QPalette textPalette = ui->textEdit->palette();
    QColor old = textPalette.color(QPalette::Base);
    QColor backgroundColor = QColorDialog::getColor(old, this);

    textPalette.setColor(QPalette::Base, (backgroundColor.isValid() ? backgroundColor : old));
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
    if(ui->spinBox->value() == 0)
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
        textStream.setCodec(ui->comboBox->currentText().toStdString().c_str());

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

    if(ui->spinBox->value() > 0)
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

                        int num = ui->spinBox->value(); //powinno być wieksze od 0

                        if(num <= middle.size() && num >= 1)
                        {
                            QString tmp = ShiftChordString(middle[num - 1].split(" "), ui->sb_shift->value());
                            text.append(tmp + "]");
                        }
                        else
                        {
                            QString tmp = ShiftChordString(middle[0].split(" "), ui->sb_shift->value());
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

    if(ui->spinBox->value() > 0)
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
/*
QString MainWindow::GetMimeType(const QString &fileName)
{
    QString result("application/octet-stream");
    magic_t magicMimePredictor;
    magicMimePredictor = magic_open(MAGIC_MIME_TYPE);

    if(!magicMimePredictor)
    {
        qDebug() << "libmagic: Unable to initialize magic library";
    }
    else
    {
        if(magic_load(magicMimePredictor, nullptr)) //jeżeli wartość różna od 0 to błąd
        {
            qDebug() << "libmagic: Can't load magic database - " + QString(magic_error(magicMimePredictor));
            magic_close(magicMimePredictor);
        }
        else
        {
            const char* file = fileName.toStdString().c_str();
            const char* mime;
            mime = magic_file(magicMimePredictor, file); //get mime-type
            result = QString(mime);
            magic_close(magicMimePredictor);
        }
    }

    qDebug() << "libmagic: result mime type - " + result + "for file: " + fileName;
    return result;
}
*/
void MainWindow::on_actionRefresh_triggered()
{
    if(ui->spinBox->value() == 0)
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
    if(ui->spinBox->value() > ui->spinBox->minimum())
        ui->spinBox->stepDown();
}

void MainWindow::on_actionNext_Chords_triggered()
{
    if(ui->spinBox->value() < ui->spinBox->maximum())
        ui->spinBox->stepUp();
}

void MainWindow::on_pushButton_clicked()
{
    if(ui->spinBox_2->value() > 0)
    {
        m_timer->start(720 - 20 * m_scrolling_speed);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    m_timer->stop();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_spinBox_column_valueChanged(int arg1)
{
    m_columns_count = arg1;

    m_column_text.resize(m_columns_count);

    PartitionText();
    ShowText();
}

void MainWindow::on_sb_shift_valueChanged(int arg1)
{
    Q_UNUSED(arg1)

    PartitionText();
    ShowText();
}

void MainWindow::on_actionNext_schift_chords_triggered()
{
    if(ui->sb_shift->value() < ui->sb_shift->maximum())
        ui->sb_shift->stepUp();
    else
        ui->sb_shift->setValue(0);
}

void MainWindow::on_actionPrevious_schift_chords_triggered()
{
    if(ui->sb_shift->value() > ui->sb_shift->minimum())
        ui->sb_shift->stepDown();
    else
        ui->sb_shift->setValue(11);
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
