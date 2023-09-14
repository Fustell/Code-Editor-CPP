#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QProcess>
#include <QTextEdit>
#include <QDir>
#include <QMessageBox>
#include <QProcess>
#include <QStyleFactory>
#include <QFileSystemModel>
#include <QCompleter>

#include "codeeditor.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    bool hasCompiler;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void CompileCpp(CodeEditor *currentTab, QFileInfo fileInfo);
    void CompileAsm(CodeEditor *currentTab, QFileInfo fileInfo);
private slots:

    void on_actionExit_triggered();

    void on_actionOpen_file_triggered();

    void on_actionNew_file_triggered();

    void on_tabWidget_tabCloseRequested(int index);

    void on_actionSave_file_triggered();

    void on_actionSave_as_triggered();

    void on_actionBuild_triggered();
    void on_actionRun_triggered();
    void on_actionDebug_triggered();

private:
    Ui::MainWindow *ui;
    QProcess compilerProcess;
    QString exePath;
    QFileSystemModel *fileModel;
    QPlainTextEdit* outputPrompt;
};
#endif // MAINWINDOW_H
