#include "customtextedit.h"
#include "ui_customtextedit.h"

CustomTextEdit::CustomTextEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomTextEdit)
{
    ui->setupUi(this);
}

CustomTextEdit::~CustomTextEdit()
{
    delete ui;
}
