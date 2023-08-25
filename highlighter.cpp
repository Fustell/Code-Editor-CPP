#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(QColor(0,145,255,255));
    keywordFormat.setFontWeight(QFont::Bold);
    const QString keywordPatterns[] = {
        QStringLiteral("\\bchar\\b"), QStringLiteral("\\bclass\\b"), QStringLiteral("\\bconst\\b"),
        QStringLiteral("\\bdouble\\b"), QStringLiteral("\\benum\\b"), QStringLiteral("\\bexplicit\\b"),
        QStringLiteral("\\bfriend\\b"), QStringLiteral("\\binline\\b"), QStringLiteral("\\bint\\b"),
        QStringLiteral("\\blong\\b"), QStringLiteral("\\bnamespace\\b"), QStringLiteral("\\boperator\\b"),
        QStringLiteral("\\bprivate\\b"), QStringLiteral("\\bprotected\\b"), QStringLiteral("\\bpublic\\b"),
        QStringLiteral("\\bshort\\b"), QStringLiteral("\\bsignals\\b"), QStringLiteral("\\bsigned\\b"),
        QStringLiteral("\\bslots\\b"), QStringLiteral("\\bstatic\\b"), QStringLiteral("\\bstruct\\b"),
        QStringLiteral("\\btemplate\\b"), QStringLiteral("\\btypedef\\b"), QStringLiteral("\\btypename\\b"),
        QStringLiteral("\\bunion\\b"), QStringLiteral("\\bunsigned\\b"), QStringLiteral("\\bvirtual\\b"),
        QStringLiteral("\\bvoid\\b"), QStringLiteral("\\bvolatile\\b"), QStringLiteral("\\bbool\\b")
    };

    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Z][A-Za-z0-9_]*(?=\\s*\\(|=)"));
    rule.format = classFormat;
    highlightingRules.append(rule);

    quotationFormat.setForeground(QColor(255,145,0,255));
    rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    preprocessorFormat.setForeground(QColor(123, 47, 148, 255));
    rule.pattern = QRegularExpression("#[^\n]*");
    rule.format = preprocessorFormat;
    highlightingRules.append(rule);

    numberFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression("\\b[0-9]+\\b");
    rule.format = numberFormat;
    highlightingRules.append(rule);

    namespaceFormat.setForeground(Qt::darkCyan);  // Set your desired text color
    rule.pattern = QRegularExpression("\\b\\w+::\\w+\\b");
    rule.format = namespaceFormat;
    highlightingRules.append(rule);

//    functionFormat.setFontItalic(true);
    functionFormat.setForeground(QColor(255,0,191));
    rule.pattern = QRegularExpression(QStringLiteral("\\b\\w+(?=\\s*\\()"));
    rule.format = functionFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    returnFormat.setForeground(QColor(0,145,255,255)); // Set your desired text color
    rule.pattern = QRegularExpression("\\breturn\\b");
    rule.format = returnFormat;
    highlightingRules.append(rule);


    thisAndArrowFormat.setForeground(Qt::darkMagenta); // Set your desired text color for 'this' and the subsequent word
    rule.pattern = QRegularExpression("\\bthis\\s*->\\s*\\w+\\b");
    rule.format = thisAndArrowFormat;
    highlightingRules.append(rule);

    // Rule for highlighting '->'
    arrowOperatorFormat.setForeground(Qt::green); // Set your desired text color for the '->' operator
    rule.pattern = QRegularExpression("->");
    rule.format = arrowOperatorFormat;
    highlightingRules.append(rule);

    newDeleteOperatorFormat.setForeground(QColor(0,145,255,255)); // Set your desired text color for 'new'
    rule.pattern = QRegularExpression("\\b(new|delete)\\b");
    rule.format = newDeleteOperatorFormat;
    highlightingRules.append(rule);


    classNameFormat.setForeground(Qt::green); // Set your desired text color for class names
    rule.pattern = QRegularExpression("\\b[A-Z][A-Za-z0-9_]*(?=\\s*(?:\\*\\s*)?[a-zA-Z0-9_]*\\s*=\\s*new\\s+[A-Z][A-Za-z0-9_]*\\s*\\()");
    rule.format = classNameFormat;
    highlightingRules.append(rule);

    rule.pattern = QRegularExpression("\\b[A-Z][A-Za-z0-9_]*(?=\\s*\\*)");
    rule.format = classNameFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);

    commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
}

void Highlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : std::as_const(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}

