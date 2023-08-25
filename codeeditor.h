#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QFont>
#include <QCompleter>
#include <QLineEdit>
#include <QStringListModel>
#include <QPainter>

#include "highlighter.h"

namespace Ui {
class CodeEditor;
}

class LineNumberArea;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    Highlighter *highlighter;

    explicit CodeEditor(QWidget *parent = nullptr);
    ~CodeEditor();

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    QString GetCurrentFile() const
    {
        return this->currentFile;
    }
    void SetCurrentFile(QString file)
    {
        this->currentFile = file;
    }

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *lineNumberArea;
    QString currentFile;

};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor)
    {
        QFont serifFont("Times", 10, QFont::Bold);

        codeEditor->setFont(serifFont);

        codeEditor->highlighter = new Highlighter(codeEditor->document());
    }

    QSize sizeHint() const override
    {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    CodeEditor *codeEditor;

    void paintEvent(QPaintEvent *event) override
    {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

};


#endif // CODEEDITOR_H
