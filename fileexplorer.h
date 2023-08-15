#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include <QWidget>
#include <QDir>
#include <QTreeWidget>

namespace Ui {
class FileExplorer;
}

class FileExplorer : public QWidget
{
    Q_OBJECT

public:
    explicit FileExplorer(QWidget *parent = nullptr);
    void SetUpDir(QString absolutPath);

private slots:
    void updateFileTree();

private:
    Ui::FileExplorer *ui;
    QTreeWidget *fileTreeWidget;
    QDir currentDirectory;
};

#endif // FILEEXPLORER_H
