#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "codeeditor.h"
#include "fileexplorer.h"

#include <Windows.h>

#define programName "IDE C++"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QMainWindow::showMaximized();

    setWindowTitle(programName);
    hasCompiler = true;

    ui->commandPrompt->resize(16220, 200);

    QStringList envVars =  QProcessEnvironment::systemEnvironment().value("Path").split(";").filter("mingw");

    if(envVars.size() == 0)
    {
        QMessageBox::critical(this, "Error","You don't have mingw compiler.\nPlease install and launch again.");
        hasCompiler = false;
        return;
    }

    QAction *saveAction = new QAction(this);
    saveAction->setShortcut(Qt::Key_S | Qt::CTRL);

    QAction *RunAction = new QAction(this);
    RunAction->setShortcut(Qt::Key_F5);

    connect(saveAction, SIGNAL(triggered()), this, SLOT(on_actionSave_file_triggered()));
    connect(RunAction, SIGNAL(triggered()), this, SLOT(on_actionRun_triggered()));

    QList<QAction*> *actionsCollection = new QList<QAction*>{saveAction, RunAction};
    this->addActions(*actionsCollection);

    compilerProcess.setProcessChannelMode(QProcess::MergedChannels);

    qApp->setStyle(QStyleFactory::create("Fusion"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_actionOpen_file_triggered()
{

    QString fileName = QFileDialog::getOpenFileName(this, "Open file",QDir::homePath());
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        return;
    }

    QTextStream in(&file);
    QString text = in.readAll();

    QFileInfo fileInfo(file.fileName());
    QString filename(fileInfo.fileName());

    CodeEditor* newTab = new CodeEditor();
    newTab->setPlainText(text);

    ui->tabWidget->addTab(newTab, filename);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);

    newTab->SetCurrentFile(fileName);
    file.close();


    QString absolutPath = fileInfo.absoluteDir().absolutePath();

    auto layout = new QVBoxLayout();
    FileExplorer* fileExplorer = new FileExplorer();
    fileExplorer->SetUpDir(absolutPath);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(fileExplorer);
    ui->fileExplorerFrame->setLayout(layout);

}

void MainWindow::on_actionNew_file_triggered()
{
    ui->tabWidget->addTab(new CodeEditor(), tr("New tab"));
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
}


void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    ui->tabWidget->widget(index)->deleteLater();
    ui->tabWidget->removeTab(index);
    ui->tabWidget->setCurrentIndex(index-1);
}


void MainWindow::on_actionSave_file_triggered()
{
    CodeEditor *currentEditorTab = static_cast<CodeEditor*>(ui->tabWidget->currentWidget());

    if(currentEditorTab == nullptr) return;

    QString filePath = currentEditorTab->GetCurrentFile();

    if(filePath.isEmpty())
    {
        filePath = QFileDialog::getSaveFileName(this, "Save",QDir::homePath());

        if(filePath.isEmpty()) return;

        QFile file(filePath);
        QFileInfo fileInfo(file.fileName());

        currentEditorTab->SetCurrentFile(filePath);
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), fileInfo.fileName());
    }

    QFile file(filePath);

    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }

    QTextStream out(&file);
    QString text = currentEditorTab->toPlainText();

    out << text;
    file.close();
}


void MainWindow::on_actionSave_as_triggered()
{

    CodeEditor *currentEditorTab = static_cast<CodeEditor*>(ui->tabWidget->currentWidget());


    QString filePath = QFileDialog::getSaveFileName(this, "Save",QDir::homePath());
    QFile file(filePath);

    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }

    currentEditorTab->SetCurrentFile(filePath);
    QTextStream out(&file);
    QString text = currentEditorTab->toPlainText();

    out << text;
    file.close();
}

void MainWindow::on_actionBuild_triggered()
{
    ui->commandPrompt->clear();
    CodeEditor *currentTab = static_cast<CodeEditor*>(ui->tabWidget->currentWidget());

    if(currentTab == nullptr) return;

    QFile file(currentTab->GetCurrentFile());
    QFileInfo fileInfo(file.fileName());
    QString outputFilePath = fileInfo.absolutePath() + "/" + fileInfo.baseName();

    QStringList arguments;
    exePath = outputFilePath;
    arguments << "-o" << outputFilePath << currentTab->GetCurrentFile();
    arguments << "-v";

    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);

    connect(&process, &QProcess::readyReadStandardOutput, [this, &process]() {
        QString output = process.readAllStandardOutput();
        ui->commandPrompt->moveCursor(QTextCursor::End);
        ui->commandPrompt->insertPlainText(output);
        ui->commandPrompt->moveCursor(QTextCursor::End);
    });

    connect(&process, &QProcess::readyReadStandardError, [this, &process]() {
        QString errorOutput = process.readAllStandardError();
        ui->commandPrompt->moveCursor(QTextCursor::End);
        ui->commandPrompt->insertPlainText(errorOutput);
        ui->commandPrompt->moveCursor(QTextCursor::End);
    });

    process.start("g++", arguments);
    process.waitForStarted();

    while (process.state() == QProcess::Running) {
        QCoreApplication::processEvents(); // Process pending events to update the UI
    }

    process.waitForFinished();

    if (process.exitCode() == 0) {
        ui->commandPrompt->appendPlainText("Compilation successful!");
    } else {
        ui->commandPrompt->appendPlainText("Compilation failed!");
    }
    ui->commandPrompt->moveCursor(QTextCursor::End);
}

void MainWindow::on_actionRun_triggered()
{
    CodeEditor *currentTab = static_cast<CodeEditor*>(ui->tabWidget->currentWidget());

    if(currentTab == nullptr) return;

    QFile file(currentTab->GetCurrentFile());
    QFileInfo fileInfo(file.fileName());
    QFile outputFilePath = static_cast<QFile>(fileInfo.absolutePath() + "/" + fileInfo.baseName() + ".exe");

    this->on_actionSave_file_triggered();
    this->on_actionBuild_triggered();

    if(outputFilePath.exists())
    {
        LPCWSTR wideFilePath = reinterpret_cast<LPCWSTR>(outputFilePath.fileName().utf16());

        // Execute the file using ShellExecute
        HINSTANCE result = ShellExecute(nullptr, L"open", wideFilePath, nullptr, nullptr, SW_SHOWNORMAL);

        if (reinterpret_cast<int>(result) <= 32)
        {
            // Handle error if ShellExecute fails (result <= 32 indicates an error)
            qDebug() << "Failed to execute the file";
        }

    }
}
