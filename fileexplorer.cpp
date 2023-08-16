#include <QMessageBox>

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
            if(fileExtention == "hpp") item->setIcon(0, QIcon(":/icons/hpp.png"));
            if(fileExtention == "h") item->setIcon(0, QIcon(":/icons/h.png"));
            if(fileExtention == "c") item->setIcon(0, QIcon(":/icons/c.png"));
            if(fileExtention == "cpp") item->setIcon(0, QIcon(":/icons/cpp.png"));

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

}

