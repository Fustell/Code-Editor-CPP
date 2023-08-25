#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QWidget>
#include <QFont>
#include <QCompleter>
#include <QLineEdit>
#include <QStringListModel>

#include "highlighter.h"

namespace Ui {
class CodeEditor;
}

class CodeEditor : public QWidget
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);
    ~CodeEditor();
    QString GetCurrentFile() const
    {
        return this->currentFile;
    }
    void SetCurrentFile(QString file)
    {
        this->currentFile = file;
    }
private:
    void setupEditor();

    Ui::CodeEditor *ui;
    QString currentFile;
    Highlighter *highlighter;

};

#endif // CODEEDITOR_H
