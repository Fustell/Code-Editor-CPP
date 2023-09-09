#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include <QWidget>
#include <QDir>
#include <QTreeWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QStack>
#include <QLabel>
#include <QMap>

namespace Ui {
class FileExplorer;
}

class FileExplorer : public QWidget
{
    Q_OBJECT

public:
    QMap<QString, QString> icons;

    explicit FileExplorer(QWidget *parent = nullptr);
    void SetUpDir(QString absolutPath);
    void openFile(QTreeWidgetItem *item, int column);

    void SetSize(unsigned int width, unsigned int height)
    {
        this->resize(width, height);
    }

private slots:
    void updateFileTree();
    void openPreviousFileTree();
    void openNextFileTree();

private:
    unsigned int width;
    unsigned int height;

    Ui::FileExplorer *ui;
    QTreeWidget *fileTreeWidget;
    QDir currentDirectory;
    QPushButton *backspace;
    QPushButton *forwardspace;
    QStack<QDir> historyNav;
    QLabel *path;
};

#endif // FILEEXPLORER_H
