#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include <QWidget>
#include <QDir>
#include <QTreeWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QStack>

namespace Ui {
class FileExplorer;
}

class FileExplorer : public QWidget
{
    Q_OBJECT

public:
    explicit FileExplorer(QWidget *parent = nullptr);
    void SetUpDir(QString absolutPath);
    void openFile(QTreeWidgetItem *item, int column);

private slots:
    void updateFileTree();
    void openPreviousFileTree();
    void openNextFileTree();

private:
    Ui::FileExplorer *ui;
    QTreeWidget *fileTreeWidget;
    QDir currentDirectory;
    QPushButton *backspace;
    QPushButton *forwardspace;
    QStack<QDir> historyNav;
};

#endif // FILEEXPLORER_H
