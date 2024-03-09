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

    // Adds help menu
    helpMenu = this->ui->menubar->addMenu("Help");
    QAction *openHelp = new QAction("&Command", this);
    connect(openHelp, &QAction::triggered, this, &MainWindow::open_help);
    helpMenu->addAction(openHelp);


    // No edit
    this->ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::get_file_path_dialog() {
    return QFileDialog::getOpenFileName(this,tr("Open txt"), "/", tr("Text (*.txt .*)"));
}

void MainWindow::open_help() {
    QMessageBox msgbox;
    msgbox.setTextInteractionFlags(Qt::TextSelectableByMouse);
    msgbox.setText("valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes  --log-file=valgrind-out.txt <your executable> <your aguments>");
    msgbox.exec();
}

void MainWindow::open_file() {
    fileName = get_file_path_dialog();

    if(fileName.isEmpty()) {
        return;
    }

    // Create reader object
    reader rd(fileName);
    this->setWindowTitle(rd.get_command());

    QList<Leak> leaks = rd.get_leaks();
    model = new QStandardItemModel;

    // Label
    model->setHorizontalHeaderLabels(QStringList("Report"));
    QStandardItem *leakItem = new QStandardItem("Leaks");

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
            inFunctions->setForeground(Qt::blue);
            inFunctions->appendRow(func);
        }

        // Adds item in hierachical order
        item->appendRow(inFiles);
        item->appendRow(inFunctions);
        item->setForeground(Qt::darkRed);
        leakItem->appendRow(item);
    }
    model->appendRow(leakItem);

    // update UI
    this->ui->treeView->setModel(model);
    this->ui->summaryText->setPlainText(rd.get_summary());
    this->ui->outputEdit->setPlainText(rd.get_new_line_content());

}
