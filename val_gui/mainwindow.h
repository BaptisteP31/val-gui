#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QStandardItemModel>
#include <QMenu>
#include <QFileDialog>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QString fileName = "";
    QStandardItemModel *model;
    QMenu *optionMenu;
    QMenu *helpMenu;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString get_file_path_dialog();
    void open_file();
    void open_help();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
