#include <QTextEdit>
#include <QDir>
#include <QMessageBox>
#include <QProcess>

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "./customtextedit.h"

#define tabBar ui->tabWidget
#define programName "IDE C++"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->tabWidget);
    setWindowTitle(programName);
    ui->tabWidget->removeTab(0);

    QAction *saveAction = new QAction(this);
    saveAction->setShortcut(Qt::Key_S | Qt::CTRL);

    connect(saveAction, SIGNAL(triggered()), this, SLOT(on_actionSave_file_triggered()));
    this->addAction(saveAction);

    compilerProcess.setProcessChannelMode(QProcess::MergedChannels);
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

    CustomTextEdit* newTab = new CustomTextEdit();
    tabBar->addTab(newTab, filename);
    tabBar->setCurrentIndex(tabBar->count()-1);

    newTab->SetCurrentFile(fileName);

    QTextEdit* tabTextEdit =tabBar->currentWidget()->findChild<QTextEdit*>();
    tabTextEdit->setText(text);
    file.close();
}


void MainWindow::on_actionNew_file_triggered()
{
    ui->tabWidget->addTab(new CustomTextEdit(), tr("New tab"));
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
    CustomTextEdit *currentTab = static_cast<CustomTextEdit*>(tabBar->currentWidget());

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

    CustomTextEdit *currentTab = static_cast<CustomTextEdit*>(tabBar->currentWidget());

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
    CustomTextEdit *currentTab = static_cast<CustomTextEdit*>(tabBar->currentWidget());

    QFile file(currentTab->GetCurrentFile());
    QFileInfo fileInfo(file.fileName());

    QStringList arguments;
    QString outputFilePath = fileInfo.absolutePath() + "\\"+fileInfo.baseName()+".exe";
    arguments << "-o" << outputFilePath << currentTab->GetCurrentFile();

    QProcess process;
    process.start("g++", arguments);
    process.waitForStarted();
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    QString errorOutput = process.readAllStandardError();

    qDebug() << output;

    if (process.exitCode() == 0) {
        qDebug() << "Compilation successful!";
    } else {
        qWarning() << "Compilation failed!";
        qDebug() << "Error output:\n" << errorOutput;
    }
}

