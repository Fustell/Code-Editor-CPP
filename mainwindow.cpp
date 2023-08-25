#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "codeeditor.h"
#include "fileexplorer.h"

#define tabBar ui->tabWidget
#define programName "IDE C++"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(programName);

    QAction *saveAction = new QAction(this);
    saveAction->setShortcut(Qt::Key_S | Qt::CTRL);

    connect(saveAction, SIGNAL(triggered()), this, SLOT(on_actionSave_file_triggered()));
    this->addAction(saveAction);

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
    tabBar->addTab(newTab, filename);
    tabBar->setCurrentIndex(tabBar->count()-1);

    newTab->SetCurrentFile(fileName);

    QTextEdit* tabTextEdit =tabBar->currentWidget()->findChild<QTextEdit*>();
    tabTextEdit->setText(text);
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
    CodeEditor *currentTab = static_cast<CodeEditor*>(tabBar->currentWidget());

    if(currentTab == nullptr) return;

    QTextEdit *currentTextEdit =  tabBar->currentWidget()->findChild<QTextEdit*>();
    QString filePath = currentTab->GetCurrentFile();

    if(filePath.isEmpty())
    {
        filePath = QFileDialog::getSaveFileName(this, "Save",QDir::homePath());
        QFile file(filePath);
        QFileInfo fileInfo(file.fileName());
        QString filename(fileInfo.fileName());

        currentTab->SetCurrentFile(filePath);
        tabBar->setTabText(tabBar->currentIndex(), filename);
    }

    QFile file(filePath);

    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }

    QTextStream out(&file);
    QString text = currentTextEdit->toPlainText();

    out << text;
    file.close();
}


void MainWindow::on_actionSave_as_triggered()
{

    CodeEditor *currentTab = static_cast<CodeEditor*>(tabBar->currentWidget());

    QTextEdit *currentTextEdit = tabBar->findChild<QTextEdit*>();
    QString filePath = QFileDialog::getSaveFileName(this, "Save",QDir::homePath());
    QFile file(filePath);

    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }

    currentTab->SetCurrentFile(filePath);
    QTextStream out(&file);
    QString text = currentTextEdit->toPlainText();

    out << text;
    file.close();
}

void MainWindow::on_actionBuild_triggered()
{
    ui->commandPrompt->clear();
    CodeEditor *currentTab = static_cast<CodeEditor*>(tabBar->currentWidget());

    if(currentTab == nullptr) return;

    QFile file(currentTab->GetCurrentFile());
    QFileInfo fileInfo(file.fileName());

    QStringList arguments;
    QString outputFilePath = fileInfo.absolutePath() + "/" + fileInfo.baseName();
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

    QString output = process.readAllStandardOutput();
    QString errorOutput = process.readAllStandardError();

    if (process.exitCode() == 0) {
        ui->commandPrompt->appendPlainText("Compilation successful!\n");
    } else {
        ui->commandPrompt->appendPlainText("Compilation failed!\n");
    }
    ui->commandPrompt->moveCursor(QTextCursor::End);
}

