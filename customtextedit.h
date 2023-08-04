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

private:
    Ui::CustomTextEdit *ui;
};

#endif // CUSTOMTEXTEDIT_H
