#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QtWidgets>
#include <QWidget>
#include <QTextEdit>
#include <QFont>
#include <QCompleter>
#include <QPainter>
#include <QTextOption>
#include <QMouseEvent>

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

    explicit CodeEditor(QString filePath="",QWidget *parent = nullptr);
    ~CodeEditor();

    QString getCurrentFile() const;

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    void setCompleter(QCompleter *c);

    QCompleter *completer() const;

    QString GetCurrentFile() const
    {
        return this->currentFile;
    }
    void SetCurrentFile(const QString &filePath)
    {
        m_currentFilePath  = filePath;
    }

    int GetTabStopWidth() const
    {
        return this->tabStopWidth;
    }
    void SetTabStopWidth(int val)
    {
        this->tabStopWidth = val;
        this->textOption.setTabStopDistance(val);
    }

    QColor GetLineColor() const
    {
        return this->tabStopWidth;
    }
    void SetLineColor(QColor val)
    {
        this->lineColor = val;
    }


    int getFirstVisibleBlockId();

    QSet<int> breakpoints;

protected:
    void resizeEvent(QResizeEvent *event) override;

    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;

private slots:
    void highlightCurrentLine();

    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea();

    void insertCompletion(const QString &completion);

private:
    int tabStopWidth = 25;
    QString m_currentFilePath;

    QWidget *lineNumberArea;
    QString currentFile;
    QCompleter *c = nullptr;
    QTextOption textOption;
    QColor lineColor = QColor(50, 50, 50, 255).lighter(160);

    QAbstractItemModel *modelFromFile(const QString& fileName);
    QString textUnderCursor() const;


};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor)
    {
        QFont serifFont("Times", 10, QFont::Bold);
        setMouseTracking(true);
        codeEditor->setFont(serifFont);

        codeEditor->breakpoints.clear();

    }

    QSize sizeHint() const override
    {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }
    void addBreakpoint(int lineNumber)
    {
        codeEditor->breakpoints.insert(lineNumber);
        update();
    }

    void removeBreakpoint(int lineNumber)
    {
        codeEditor->breakpoints.remove(lineNumber);
        update();
    }
protected:
    CodeEditor *codeEditor;

    void paintEvent(QPaintEvent *event) override
    {
        codeEditor->lineNumberAreaPaintEvent(event);

        int blockNumber = codeEditor->getFirstVisibleBlockId();
        QTextBlock block = codeEditor->document()->findBlockByNumber(blockNumber);
        QTextBlock prev_block = (blockNumber > 0) ? codeEditor->document()->findBlockByNumber(blockNumber-1) : block;
        int translate_y = (blockNumber > 0) ? -codeEditor->verticalScrollBar()->sliderPosition() : 0;

        int top = codeEditor->viewport()->geometry().top();

        // Adjust text position according to the previous "non entirely visible" block
        // if applicable. Also takes in consideration the document's margin offset.
        int additional_margin;
        if (blockNumber == 0)
        {
            // Simply adjust to document's margin
            additional_margin = (int) codeEditor->document()->documentMargin() -1 - codeEditor->verticalScrollBar()->sliderPosition();
        }
        else
        {
            // Getting the height of the visible part of the previous "non entirely visible" block
            additional_margin = (int) codeEditor->document()->documentLayout()->blockBoundingRect(prev_block)
                                    .translated(0, translate_y).intersected(codeEditor->viewport()->geometry()).height();
        }

        // Shift the starting point
        top += additional_margin;

        int bottom = top + (int) codeEditor->document()->documentLayout()->blockBoundingRect(block).height();

        QPainter painter(this);

        while (block.isValid() && top <= event->rect().bottom()) {
            if (block.isVisible() && bottom >= event->rect().top() && codeEditor->breakpoints.contains(blockNumber)) {
                painter.setPen(Qt::black);
                painter.setBrush(Qt::red);
                painter.drawEllipse(0, top+3, 9, 9);
            }
            block = block.next();
            top = bottom;
            bottom = top + (int) codeEditor->document()->documentLayout()->blockBoundingRect(block).height();;
            ++blockNumber;

        }

    }
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            int lineNumber = codeEditor->cursorForPosition(event->pos()).blockNumber();

            if (codeEditor->breakpoints.contains(lineNumber)) {

                removeBreakpoint(lineNumber);
            } else {

                addBreakpoint(lineNumber);
            }
        }
    }
};


#endif // CODEEDITOR_H
