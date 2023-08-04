#ifndef CUSTOMTEXTEDIT_H
#define CUSTOMTEXTEDIT_H

#include <QWidget>

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
    Ui::CustomTextEdit *ui;
    QString currentFile;
};

#endif // CUSTOMTEXTEDIT_H
