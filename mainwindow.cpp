#include <QTextEdit>
#include <QDir>

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "./customtextedit.h"

#define tabBar ui->tabWidget

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    CustomTextEdit *customTextEdit = new CustomTextEdit();
    ui->tabWidget->removeTab(0);
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
    currentFile = fileName;

    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        return;
    }

    setWindowTitle(fileName);
    QTextStream in(&file);
    QString text = in.readAll();

    QFileInfo fileInfo(file.fileName());
    QString filename(fileInfo.fileName());

    tabBar->addTab(new CustomTextEdit(), filename);
    tabBar->setCurrentIndex(tabBar->count()-1);

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
    ui->tabWidget->removeTab(index);
    ui->tabWidget->setCurrentIndex(index-1);
}

