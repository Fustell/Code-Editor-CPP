#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QProcess>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    private slots:

    void on_actionExit_triggered();

    void on_actionOpen_file_triggered();

    void on_actionNew_file_triggered();

    void on_tabWidget_tabCloseRequested(int index);

    void on_actionSave_file_triggered();

    void on_actionSave_as_triggered();

    void on_actionBuild_triggered();

private:
    Ui::MainWindow *ui;
    QProcess compilerProcess;
};
#endif // MAINWINDOW_H
