#ifndef CUSTOMTEXTEDIT_H
#define CUSTOMTEXTEDIT_H

#include <QWidget>
#include <QFont>

#include "highlighter.h"

namespace Ui {
class CustomTextEdit;
}

class CustomTextEdit : public QWidget
{
    Q_OBJECT

public:
    explicit CustomTextEdit(QWidget *parent = nullptr);
    ~CustomTextEdit();
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

    Ui::CustomTextEdit *ui;
    QString currentFile;
    Highlighter *highlighter;

};

#endif // CUSTOMTEXTEDIT_H
