#include "ui/mainwindow.h"
#include "ui_mainwindow.h"
#include "utils/fileexplorer.h"
#include <Windows.h>
#include <QMimeData>
#include <QFileInfo>
#include <QUrl>
#include <QDragEnterEvent>
#include <QDropEvent>

#define programName "Code Editor C/C++"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(programName);

    ui->OutputTabWidget->resize(16220, 200);

    QStringList envVars =  QProcessEnvironment::systemEnvironment().value("Path").split(";").filter("mingw");

    if(envVars.size() == 0)
    {
        QMessageBox::critical(this, "Error","You don't have mingw compiler.\nPlease install and launch again.");
        this->setCppCompiler(false);
        return;
    }
    else
    {
        this->setCppCompiler(true);
    }

    QAction *saveAction = new QAction(this);
    QAction *RunAction = new QAction(this);

    saveAction->setShortcut(Qt::Key_S | Qt::CTRL);
    RunAction->setShortcut(Qt::Key_F5);

    connect(saveAction, SIGNAL(triggered()), this, SLOT(on_actionSave_file_triggered()));
    connect(RunAction, SIGNAL(triggered()), this, SLOT(on_actionRun_triggered()));

    QList<QAction*> *actionsCollection = new QList<QAction*>{saveAction, RunAction};
    this->addActions(*actionsCollection);

    compilerProcess.setProcessChannelMode(QProcess::MergedChannels);

    qApp->setStyle(QStyleFactory::create("Fusion"));
    QMainWindow::showMaximized();

    outputPrompt = ui->OutputTabWidget->findChild<QWidget*>("build")->findChild<QPlainTextEdit*>();
    setAcceptDrops(true);
}
void MainWindow::dragEnterEvent(QDragEnterEvent *event) {

    if (event->mimeData()->hasUrls()) {
        bool can_accept = false;
        for (const QUrl &url : event->mimeData()->urls()) {
            if (url.isLocalFile()) {
                can_accept = true;
                break;
            }
        }
        if (can_accept) {
            event->acceptProposedAction();
        } else {
            event->ignore();
        }
    } else {
        event->ignore();
    }
}

void MainWindow::openFileInNewTab(const QString &filePath)
{
    if (filePath.isEmpty()) {
        qWarning() << "Attempted to open an empty file path.";
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        qWarning() << "Could not open file:" << filePath << " Error:" << file.errorString();
        return;
    }

    QTextStream in(&file);
    QString text = "";
    text = in.readAll();
    file.close();

    QFileInfo fileInfo(filePath);
    QString shortFilename = fileInfo.fileName();

    for (int i = 0; i < ui->tabWidget->count(); ++i) {
        CodeEditor* editor = qobject_cast<CodeEditor*>(ui->tabWidget->widget(i));
        if (editor && editor->getCurrentFile() == filePath) {
            ui->tabWidget->setCurrentIndex(i);
            qDebug() << "File already open, switching to tab:" << filePath;
            return;
        }
    }

    CodeEditor* newTab = new CodeEditor(filePath);
    newTab->setPlainText(text);

    int newIndex = ui->tabWidget->addTab(newTab, shortFilename);
    ui->tabWidget->setCurrentIndex(newIndex);

    ui->tabWidget->setTabToolTip(newIndex, filePath);

    qDebug() << "Successfully opened file in new tab:" << filePath;

}

void MainWindow::dropEvent(QDropEvent *event) {
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        bool accepted_at_least_one = false;

        for (const QUrl &url : urlList) {
            if (url.isLocalFile()) {
                QString filePath = url.toLocalFile();
                QFileInfo fileInfo(filePath);

                if (fileInfo.isFile()) {
                    qDebug() << "Attempting to open dropped file:" << filePath;
                    openFileInNewTab(filePath);

                    accepted_at_least_one = true;
                } else {
                    qWarning() << "Ignoring dropped directory:" << filePath;
                }
            }
        }

        if (accepted_at_least_one) {
            event->acceptProposedAction();
        } else {
            event->ignore();
        }
    } else {
        event->ignore();
    }
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
    QString fileName = QFileDialog::getOpenFileName(this, "Open file", QDir::homePath(), tr("C/C++ (*.c *.cpp *.h *hpp)"));

    if (!fileName.isEmpty()) {
        openFileInNewTab(fileName);
    }
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

void MainWindow::CompileCpp(CodeEditor *currentTab, QFileInfo fileInfo)
{
    QString outputFilePath = fileInfo.absolutePath() + "/" + fileInfo.baseName();
    QString fileExtention = fileInfo.suffix().toLower();

    QStringList arguments;
    exePath = outputFilePath;
    arguments <<"-g" << "-o" << outputFilePath << currentTab->GetCurrentFile();
    arguments << "-v";

    compilerProcess.setProcessChannelMode(QProcess::MergedChannels);

    connect(&compilerProcess, &QProcess::readyReadStandardOutput, [this]() {
        QString output = compilerProcess.readAllStandardOutput();
        outputPrompt->moveCursor(QTextCursor::End);
        outputPrompt->insertPlainText(output);
        outputPrompt->moveCursor(QTextCursor::End);
    });

    connect(&compilerProcess, &QProcess::readyReadStandardError, [this]() {
        QString errorOutput = compilerProcess.readAllStandardError();
        outputPrompt->moveCursor(QTextCursor::End);
        outputPrompt->insertPlainText(errorOutput);
        outputPrompt->moveCursor(QTextCursor::End);
    });

    compilerProcess.start("g++", arguments);
    compilerProcess.waitForStarted();

    while (compilerProcess.state() == QProcess::Running) {
        QCoreApplication::processEvents();
    }

    compilerProcess.waitForFinished();

    if (compilerProcess.exitCode() == 0) {
        outputPrompt->appendPlainText("Compilation successful!");
    } else {
        outputPrompt->appendPlainText("Compilation failed!");
    }
    outputPrompt->moveCursor(QTextCursor::End);
}

void MainWindow::on_actionBuild_triggered()
{
    CodeEditor *currentTab = static_cast<CodeEditor*>(ui->tabWidget->currentWidget());

    if(currentTab == nullptr || outputPrompt == nullptr) return;

    QFile file(currentTab->GetCurrentFile());
    QFileInfo fileInfo(file.fileName());
    QString fileExtention = fileInfo.suffix().toLower();

    outputPrompt->clear();

    QSet<QString> allowedExtentions = {"cpp", "c", "cxx"};
    if(allowedExtentions.contains(fileExtention))
    {
        this->CompileCpp(currentTab, fileInfo);
    }
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
