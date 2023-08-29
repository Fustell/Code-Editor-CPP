#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QtWidgets>
#include <QWidget>
#include <QTextEdit>
#include <QFont>
#include <QCompleter>
#include <QPainter>
#include <QTextOption>

#include "highlighter.h"

namespace Ui {
class CodeEditor;
}

class LineNumberArea;

class CodeEditor : public QTextEdit
{
    Q_OBJECT

public:
    Highlighter *highlighter;

    explicit CodeEditor(QWidget *parent = nullptr);
    ~CodeEditor();

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    void setCompleter(QCompleter *c);
    QCompleter *completer() const;

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

    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;

private slots:
    void highlightCurrentLine();
    int getFirstVisibleBlockId();

    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea();

    void insertCompletion(const QString &completion);

private:
    QWidget *lineNumberArea;
    QString currentFile;
    QCompleter *c = nullptr;

    QAbstractItemModel *modelFromFile(const QString& fileName);

    QString textUnderCursor() const;



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
