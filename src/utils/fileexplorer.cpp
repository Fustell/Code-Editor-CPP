#include "utils/fileexplorer.h"
#include "core/codeeditor.h"

void FileExplorer::updateFileTree() {
    fileSystemWatcher->removePath(currentDirectory.absolutePath());\

    fileTreeWidget->clear();

    path->setText(currentDirectory.absolutePath());

    fileSystemWatcher->addPath(currentDirectory.absolutePath());

    QFileInfoList fileInfoList = currentDirectory.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QFileInfo &fileInfo : fileInfoList) {
        QTreeWidgetItem *item = new QTreeWidgetItem(fileTreeWidget);
        item->setText(0, fileInfo.fileName());
        item->setData(0, Qt::UserRole, fileInfo.absoluteFilePath());
        if (fileInfo.isFile()) {
            QString fileExtention = fileInfo.suffix().toLower();

            if(icons.contains(fileExtention)) item->setIcon(0, QIcon(icons.value(fileExtention)));
            else item->setIcon(0, QIcon(":/icons/empty.png"));

        }
        if (fileInfo.isDir()) {
            item->setIcon(0, QIcon(":/icons/folder.png"));
        }
    }
}

void FileExplorer::showContextMenu(const QPoint& pos)
{
    QMenu contextMenu(this);

    QAction* createFolderAction = contextMenu.addAction("Create Folder");
    QAction* createFileAction = contextMenu.addAction("Create File");
    QAction* renameAction = contextMenu.addAction("Rename");
    QAction* deleteAction = contextMenu.addAction("Delete");


    QAction* selectedItem = contextMenu.exec(fileTreeWidget->mapToGlobal(pos));
     QList<QTreeWidgetItem*> selectedItems = fileTreeWidget->selectedItems();
    if (selectedItem == createFolderAction) {
        bool ok;
        QString text = QInputDialog::getText(this, tr("Enter the name of folder"),
                                             nullptr, QLineEdit::Normal,
                                             nullptr, &ok);
        if (ok && !text.isEmpty())
        {

            QDir dir(currentDirectory.absolutePath() +QDir::separator()+ text);
            if(!dir.exists())
            {
                dir.mkdir(currentDirectory.absolutePath() +QDir::separator()+ text);
            }
        }
    } else if (selectedItem == createFileAction) {
        bool ok;
        QString text = QInputDialog::getText(this, tr("Enter the name of file"),
                                             nullptr, QLineEdit::Normal,
                                             nullptr, &ok);

        if (ok && !text.isEmpty())
        {
            QFile file(currentDirectory.absolutePath() +QDir::separator()+ text);
            if(file.open(QIODevice::WriteOnly|QIODevice::Text)){}
            file.close();
        }
    } else if (selectedItem == deleteAction) {
        for (QTreeWidgetItem* item : selectedItems) {
            QString filePath = item->data(0, Qt::UserRole).toString();
            QFile file(filePath);
            QDir directory(filePath);

            if(directory.exists())
            {
             if (directory.removeRecursively()) delete item;
            }else if(file.exists())
            {
             if (file.remove()) delete item;
            }

        }
    }
    else if (selectedItem == renameAction) {
        for (QTreeWidgetItem* item : selectedItems) {
            QString filePath = item->data(0, Qt::UserRole).toString();
            QFile file(filePath);
            QDir directory(filePath);

            bool ok;
            QString text = QInputDialog::getText(this, tr("Enter the name of folder"),
                                                 nullptr, QLineEdit::Normal,
                                                 nullptr, &ok);
            if (ok && !text.isEmpty())
            {
                 if(directory.exists())
                 {
                    directory.rename(filePath, currentDirectory.absolutePath() +QDir::separator()+ text);
                     delete item;
                 }else if(file.exists())
                 {
                     QFile::rename(filePath, currentDirectory.absolutePath() +QDir::separator()+ text);
                     delete item;
                 }
            }


        }
    }

}


FileExplorer::FileExplorer(QWidget *parent) : QWidget(parent) {

    icons["txt"] = ":/icons/txt.png";
    icons["cpp"] = ":/icons/cpp.png";
    icons["c"] = ":/icons/c.png";
    icons["hpp"] = ":/icons/hpp.png";
    icons["h"] = ":/icons/h.png";
    this->SetSize(70, 16227);
    };

void FileExplorer::openPreviousFileTree()
{
    historyNav.push(currentDirectory);
    currentDirectory.cd("../");
    updateFileTree();
}
void FileExplorer::openNextFileTree()
{
    if(!historyNav.empty())
    {
        currentDirectory.cd(historyNav.top().absolutePath());
        historyNav.pop();
        updateFileTree();
    }
}

void FileExplorer::SetUpDir(QString absolutPath)
{
    this->currentDirectory = QDir(absolutPath);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QHBoxLayout *buttonsLayout = new QHBoxLayout(this);

    backspace = new QPushButton(this);
    backspace->setText("<-");
    buttonsLayout->addWidget((backspace));

    connect(backspace, &QPushButton::clicked, this, &FileExplorer::openPreviousFileTree);

    forwardspace = new QPushButton(this);
    forwardspace->setText("->");
    buttonsLayout->addWidget((forwardspace));

    path = new QLabel(this);
    layout->addWidget(path);

    connect(forwardspace, &QPushButton::clicked, this, &FileExplorer::openNextFileTree);

    layout->addLayout(buttonsLayout);
    fileTreeWidget = new QTreeWidget(this);
    layout->addWidget(fileTreeWidget);
    fileTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(fileTreeWidget, &QTreeWidget::customContextMenuRequested, this, &FileExplorer::showContextMenu);

    fileSystemWatcher = new QFileSystemWatcher(this);
    connect(fileSystemWatcher, &QFileSystemWatcher::directoryChanged, this, &FileExplorer::updateFileTree);


    fileTreeWidget->setHeaderLabels(QStringList() << "File Explorer");

    updateFileTree();

    connect(fileTreeWidget, &QTreeWidget::itemDoubleClicked, this, &FileExplorer::openFile);
}

void FileExplorer::openFile(QTreeWidgetItem *item, int column) {
    QString filePath = item->data(0, Qt::UserRole).toString();
    QFileInfo fileInfo(filePath);
    QFile file(filePath);
    QString filename(fileInfo.fileName());

    if (fileInfo.isFile()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString fileContent = QString::fromUtf8(file.readAll());
            file.close();
            QTabWidget *tabWidget = parentWidget()->parentWidget()->findChild<QTabWidget*>("tabWidget");

            if (tabWidget == nullptr) return;

            QList<CodeEditor*> codeEditors = tabWidget->findChildren<CodeEditor*>();
            for(auto& editor: codeEditors)
            {
                if (editor->GetCurrentFile() == filePath) return;
            }

            CodeEditor* newTab = new CodeEditor(filePath);
            newTab->setPlainText(fileContent);
            tabWidget->addTab(newTab, filename);
            tabWidget->setCurrentIndex(tabWidget->count()-1);

            newTab->SetCurrentFile(filePath);

        }
    } else if (fileInfo.isDir()) {
        currentDirectory.cd(fileInfo.fileName());
        updateFileTree();
    }
}

