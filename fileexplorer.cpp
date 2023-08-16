#include <QMessageBox>
#include <QTextEdit>

#include "fileexplorer.h"
#include "ui_fileexplorer.h"

void FileExplorer::updateFileTree() {
    fileTreeWidget->clear();
    QFileInfoList fileInfoList = currentDirectory.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QFileInfo &fileInfo : fileInfoList) {
        QTreeWidgetItem *item = new QTreeWidgetItem(fileTreeWidget);
        item->setText(0, fileInfo.fileName());
        item->setData(0, Qt::UserRole, fileInfo.absoluteFilePath());
        if (fileInfo.isFile()) {
            QString fileExtention = fileInfo.suffix().toLower();

            if(fileExtention == "txt") item->setIcon(0, QIcon(":/icons/txt.png"));
            else if(fileExtention == "cpp") item->setIcon(0, QIcon(":/icons/cpp.png"));
            else if(fileExtention == "c") item->setIcon(0, QIcon(":/icons/c.png"));
            else if(fileExtention == "hpp") item->setIcon(0, QIcon(":/icons/hpp.png"));
            else if(fileExtention == "h") item->setIcon(0, QIcon(":/icons/h.png"));
            else item->setIcon(0, QIcon(":/icons/empty.png"));

        }
        if (fileInfo.isDir()) {
            item->setIcon(0, QIcon(":/icons/folder.png"));
        }
    }
}

FileExplorer::FileExplorer(QWidget *parent) : QWidget(parent) {
    };

void FileExplorer::SetUpDir(QString absolutPath)
{
    this->currentDirectory = QDir(absolutPath);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    fileTreeWidget = new QTreeWidget(this);
    layout->addWidget(fileTreeWidget);

    fileTreeWidget->setHeaderLabels(QStringList() << "File Explorer");

    updateFileTree();

    connect(fileTreeWidget, &QTreeWidget::itemDoubleClicked, this, &FileExplorer::openFile);
}

void FileExplorer::openFile(QTreeWidgetItem *item, int column) {
    QString filePath = item->data(0, Qt::UserRole).toString();
    QFileInfo fileInfo(filePath);

    if (fileInfo.isFile()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString fileContent = QString::fromUtf8(file.readAll());
            file.close();
            QTabWidget *tabWidget = parentWidget()->parentWidget()->findChild<QTabWidget*>();
            if (tabWidget) {
                QTextEdit *textEdit = new QTextEdit;
                textEdit->setPlainText(fileContent);
                tabWidget->addTab(textEdit, fileInfo.fileName());
                tabWidget->setCurrentWidget(textEdit);
            }
        }
    } else if (fileInfo.isDir()) {
        currentDirectory.cd(fileInfo.fileName());
        updateFileTree();
    }
}

