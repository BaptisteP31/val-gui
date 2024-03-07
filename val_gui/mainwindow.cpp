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
    reader rd("./valgrind-out.txt");

    this->setWindowTitle(rd.get_command());

    QList<Leak> leaks = rd.get_leaks();

    for (auto &leak : leaks) {
        qDebug() << leak.to_string();
    }

    model = new QStandardItemModel;

    for (const auto& leak : leaks) {
        QStandardItem *item = new QStandardItem(leak.to_string());

        QStandardItem *inFiles = new QStandardItem("Files");
        for (const auto& filename : leak.get_in_files()) {
            QStandardItem *file = new QStandardItem(filename);
            inFiles->appendRow(file);
        }

        QStandardItem *inFunctions = new QStandardItem("Functions");
        for (const auto& function : leak.get_functions()) {
            QStandardItem *func = new QStandardItem(function);
            inFunctions->setForeground(Qt::darkBlue);
            inFunctions->appendRow(func);
        }


        item->appendRow(inFiles);
        item->appendRow(inFunctions);
        item->setForeground(Qt::darkRed);
        model->appendRow(item);
    }

    model->setHorizontalHeaderLabels(QStringList("Leaks"));

    this->ui->treeView->setModel(model);
    this->ui->summaryText->setPlainText(rd.get_summary());
}

MainWindow::~MainWindow()
{
    delete ui;
}
