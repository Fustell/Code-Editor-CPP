#include "fileexplorer.h"
#include "codeeditor.h"
#include "ui_codeeditor.h"

void FileExplorer::updateFileTree() {
    fileTreeWidget->clear();
    path->setText(currentDirectory.absolutePath());

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
            QTabWidget *tabWidget = parentWidget()->parentWidget()->findChild<QTabWidget*>();

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

