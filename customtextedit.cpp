#include "customtextedit.h"
#include "ui_customtextedit.h"

CustomTextEdit::CustomTextEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomTextEdit)
{
    ui->setupUi(this);
    setupEditor();
}

CustomTextEdit::~CustomTextEdit()
{
    delete ui;
}

void CustomTextEdit::setupEditor()
{/*
   this->ui->textEdit->setCurrentFont(serifFont);*/
    QFont serifFont("Times", 10, QFont::Bold);
    ui->textEdit->setFont(serifFont);

    this->highlighter = new Highlighter(ui->textEdit->document());
}
