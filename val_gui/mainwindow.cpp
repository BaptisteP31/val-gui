#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "leak.h"
#include "reader.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Adds option menu
    optionMenu = this->ui->menubar->addMenu("File");
    QAction *openFile = new QAction("&Open file", this);
    connect(openFile, &QAction::triggered, this, &MainWindow::open_file);
    optionMenu->addAction(openFile);

    // No edit
    this->ui->lostBytesEdit->setReadOnly(true);
    this->ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::get_file_path_dialog() {
    return QFileDialog::getOpenFileName(this,tr("Open txt"), "/", tr("Text (*.txt .*)"));
}

void MainWindow::open_file() {
    fileName = get_file_path_dialog();

    if(fileName.isEmpty()) {
        return;
    }

    lost_bytes=0;

    // Create reader object
    reader rd(fileName);
    this->setWindowTitle(rd.get_command());

    QList<Leak> leaks = rd.get_leaks();
    model = new QStandardItemModel;

    // populate the treeview
    for (const auto& leak : leaks) {
        // Leak summaries
        QStandardItem *item = new QStandardItem(leak.to_string());

        // File list
        QStandardItem *inFiles = new QStandardItem("Files");
        for (const auto& filename : leak.get_in_files()) {
            QStandardItem *file = new QStandardItem(filename);
            inFiles->appendRow(file);
        }

        // List functions
        QStandardItem *inFunctions = new QStandardItem("Functions");
        for (const auto& function : leak.get_functions()) {
            QStandardItem *func = new QStandardItem(function);
            inFunctions->setForeground(Qt::darkBlue);
            inFunctions->appendRow(func);
        }

        // Adds item in hierachical order
        item->appendRow(inFiles);
        item->appendRow(inFunctions);
        item->setForeground(Qt::darkRed);
        model->appendRow(item);

        lost_bytes+=leak.get_lost_bytes();
    }


    // Label
    model->setHorizontalHeaderLabels(QStringList("Leaks"));

    // update UI
    this->ui->treeView->setModel(model);
    this->ui->summaryText->setPlainText(rd.get_summary());
    this->ui->lostBytesEdit->setText(QString::number(lost_bytes));
    this->ui->outputEdit->setPlainText(rd.get_new_line_content());

}
