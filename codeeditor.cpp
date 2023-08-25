#include "codeeditor.h"
#include "ui_codeeditor.h""

CodeEditor::CodeEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CodeEditor)
{
    ui->setupUi(this);
    setupEditor();
}

CodeEditor::~CodeEditor()
{
    delete ui;
}

void CodeEditor::setupEditor()
{
    QFont serifFont("Times", 10, QFont::Bold);
    ui->textEdit->setFont(serifFont);

    this->highlighter = new Highlighter(ui->textEdit->document());
}
