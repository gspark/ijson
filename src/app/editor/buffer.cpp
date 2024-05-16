#include "buffer.h"
#include "util.h"
#include "common/config.h"

#include "ILexer.h"
#include "Lexilla.h"
#include "SciLexer.h"

#include <QBuffer>
#include <QDebug>
#include <QDropEvent>
#include <QFontDatabase>
#include <QTextStream>
#include <QUrl>

#include <algorithm>
#include <cmath>

Buffer::Buffer(QWidget* parent)
    : ScintillaEdit(parent)
    , lastPos(-1)
    , lastLine(-1)
    , lastTextLength(0)
{
    setCodePage(SC_CP_UTF8);

    setMarginWidthN(1, 0);

    // Load the initial state from the configuration
    loadConfiguration();

    setMarginSensitiveN(Icon, false);
    setMarginSensitiveN(Fold, false);

    connect(this, &Buffer::updateUi, this, &Buffer::onUpdateUi);
    connect(this, &Buffer::linesAdded, this, &Buffer::onLinesAdded);
}

Buffer::~Buffer() {
}

void Buffer::loadConfiguration() {
    m_trackLineWidth = true;
    m_braceHighlight = true;
    setViewIndentationGuides(true);
    setShowLineNumbers(false);
    setScrollWidth(1);
    setScrollWidthTracking(true);

    setStyleQFont(STYLE_DEFAULT, ConfigIni::getInstance().font(QStringLiteral("Viewer/FontName")));
   
    //setColorScheme(ConfigIni::getInstance().iniRead(QStringLiteral("Configuration/theme"), "default").toString());
    setLanguage("json");
}

void Buffer::clear() {
    // Clear the file name and the editor
    clearAll();
    setFileInfo(QFileInfo(""));
    setSavePoint();
    setShowLineNumbers(false);
}

bool Buffer::open(const QString &fileName) {
    // Open the file
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    char* ptr = reinterpret_cast<char*>(file.map(0, file.size(), QFile::MapPrivateOption));
    if (ptr) {
        // worked
        //QByteArray data = QByteArray::fromRawData(ptr, file.size());
        setText(ptr);
    }
    else {
        QTextStream input(&file);
        //input.setCodec(m_encoding->name());
        QString content = input.readAll();
        setText(content.toUtf8());
        //clearDocumentStyle();
    }
    file.close();

    // File opened succesfully
    setFileInfo(QFileInfo(fileName));
    //emptyUndoBuffer();
    //setSavePoint();
    setShowLineNumbers(true);
    return true;
}

bool Buffer::save(const QString &fileName) {
    // Save the file
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    // Save the text to a file.
    QTextStream output(&file);
    QByteArray content = getText(textLength() + 1);
    //output.setCodec(m_encoding->name());
    output << QString::fromUtf8(content);
    output.flush();
    file.close();

    // File saved
    setFileInfo(QFileInfo(fileName));
    setSavePoint();

    return true;
}

QFileInfo Buffer::fileInfo() const {
    return m_fileInfo;
}

void Buffer::setColorScheme(const QString& theme) {
    // Set the common features of all styles.
    styleSetFore(STYLE_DEFAULT, convertColor(ConfigIni::getInstance().foreground(theme)));

    styleSetBack(STYLE_DEFAULT, convertColor(ConfigIni::getInstance().background(theme)));

    setCaretFore(convertColor(ConfigIni::getInstance().caret(theme)));

    setCaretLineBack(convertColor(ConfigIni::getInstance().caretLine(theme)));

    int selection = convertColor(ConfigIni::getInstance().selection(theme));
    if (selection == -1) {
        setSelBack(false, 0);
    }
    else {
        setSelBack(true, selection);
    }

    // Copy common features of all styles.
    styleClearAll();

    // Set the foreground of whitespace when there is no style defined by the lexer.
    setWhitespaceFore(false, 0);
    styleSetFore(0, convertColor(ConfigIni::getInstance().whitespace(theme)));
    // Set up the lexer styles
    StyleInfo styleInfo;
    styleInfo.setForegroundColor(convertColor(ConfigIni::getInstance().number(theme)));
    styleInfo.setItalic(false);
    applyStyle(SCE_JSON_NUMBER, styleInfo);
   
    styleInfo.setForegroundColor(convertColor(ConfigIni::getInstance().string(theme)));
    styleInfo.setItalic(false);
    applyStyle(SCE_JSON_STRING, styleInfo);

    styleInfo.setForegroundColor(convertColor(ConfigIni::getInstance().propertyName(theme)));
    styleInfo.setItalic(false);
    applyStyle(SCE_JSON_PROPERTYNAME, styleInfo);

    styleInfo.setForegroundColor(convertColor(ConfigIni::getInstance().keyword(theme)));
    styleInfo.setItalic(false);
    applyStyle(SCE_JSON_KEYWORD, styleInfo);
}

void Buffer::setViewIndentationGuides(bool viewIndentationGuides) {
    if (viewIndentationGuides) {
        setIndentationGuides(3);
    }
    else {
        setIndentationGuides(SC_IV_NONE);
    }
}

bool Buffer::showLineNumbers() const {
    return marginWidthN(Line) != 0;
}

void Buffer::setShowLineNumbers(bool showLineNumbers) {
    setMarginWidthN(Line, showLineNumbers ? getLineMarginWidth() : 0);
}

void Buffer::setStyleQFont(int style, const QFont& font) {
    styleSetFont(style, font.family().toLatin1());
    styleSetSize(style, font.pointSize());
    styleSetBold(style, font.bold());
    styleSetItalic(style, font.italic());
}

bool Buffer::find(const QString& findText, int flags, bool forward, bool wrap, bool *searchWrapped) {
    if (findText.isEmpty()) {
        return false;
    }
    if (searchWrapped) {
        *searchWrapped = false;
    }
    // Perform the search
    setSearchFlags(flags);
    setTargetStart(forward ? currentPos() : currentPos() - 1);
    setTargetEnd(forward ? length() : 0);
    QByteArray findArray = findText.toUtf8();
    int findPos = searchInTarget(findArray.length(), findArray);
    // If the search should wrap, perform the search again.
    if (findPos == -1 && wrap) {
        setTargetStart(forward ? 0 : length());
        setTargetEnd(forward ? currentPos() : currentPos() - 1);
        findPos = searchInTarget(findArray.length(), findArray);
        if (searchWrapped) {
            *searchWrapped = true;
        }
    }
    if (findPos != -1)  {
        setSel(targetStart(), targetEnd());
        scrollRange(targetStart(), targetEnd());
    }

    return findPos != -1;
}

void Buffer::onUpdateUi(Scintilla::Update updated) {
    if (m_braceHighlight && selectionEmpty()) {
        sptr_t position = currentPos();
        sptr_t braceStart = -1;
        // Check previous character first.
        if (position != 0 && isBrace(charAt(position - 1))) {
            braceStart = position - 1;
        } else if (isBrace(charAt(position))) { // Check next character.
            braceStart = position;
        }
        if (braceStart != -1) {
            // Brace found
            sptr_t braceEnd = braceMatch(braceStart, 0);
            if (braceEnd > 0) {
                braceHighlight(braceStart, braceEnd);
            } else {
                // Missing matching brace.
                braceBadLight(braceStart);
            }
        } else {
            // Not a brace, clear indicators.
            braceBadLight(-1);
        }

        if (position != lastPos)
        {
            lastPos = position;
            intptr_t line = lineFromPosition(position);
            int col = column(position);
            emit cursorPositionChanged(line, col);
        }
        intptr_t textLength = this->textLength();
        if (lastTextLength != textLength)
        {
            if (!showLineNumbers() && textLength > 0)
            {
                setShowLineNumbers(true);
            }
            lastTextLength = textLength;
            emit textLengthChanged(textLength, this->lineCount());
        }
    }
}

void Buffer::onLinesAdded(int linesAdded)
{
    if (showLineNumbers() && m_trackLineWidth) {
        setMarginWidthN(Line, getLineMarginWidth());
    }
}

void Buffer::setFileInfo(const QFileInfo& fileInfo) {
    if (m_fileInfo != fileInfo) {
        m_fileInfo = fileInfo;
        // Set up the lexer for the buffer
        setLanguage("json");
        emit fileInfoChanged(fileInfo);
    }
}


void Buffer::setLanguage(const QString& language) {

    if (!language.isEmpty()) {
        //setLexerLanguage(language->lexer().toLocal8Bit());
        ILexer5* pLexer = CreateLexer(language.toLocal8Bit());
        setILexer((sptr_t)pLexer);

        //setKeyWords(0, "{");
        //setKeyWords(1, "}");
        //setKeyWords(2, "[");
        //setKeyWords(3, "]");
        //setKeyWords(4, ":");
        //setKeyWords(5, "\"");

        setProperty("fold", "1");
        setProperty("fold.compact", "0");
    }
    else {
        //setLexer(SCLEX_NULL);
        setILexer(NULL);
        //for (int i = 0; i < 6; ++i)
        //{
        //    setKeyWords(i, "");
        //}
        setProperty("fold", "0");
    }

    setColorScheme(ConfigIni::getInstance().iniRead(QStringLiteral("Configuration/theme"), "").toString());
}

int Buffer::getLineMarginWidth() {
    int lineWidth = m_trackLineWidth ? lineCount() : 1;
    int width = ((int)std::log10(lineWidth)) + 1;
    QString text;
    text.fill('9', width).prepend('_');

    return textWidth(STYLE_LINENUMBER, text.toLatin1());
}

int Buffer::isBrace(sptr_t c) {
    if (c == '{' || c == '(' || c == '[') {
        return 1;
    }
    else if (c == '}' || c == ')' || c == ']') {
        return -1;
    }

    return 0;
}

void Buffer::applyStyle(int styleNumber, const StyleInfo& style) {
    if (style.foregroundColor() >= 0) {
        styleSetFore(styleNumber, style.foregroundColor());
    }
    if (style.backgroundColor() >= 0) {
        styleSetBack(styleNumber, style.backgroundColor());
    }
    styleSetBold(styleNumber, style.bold());
    styleSetItalic(styleNumber, style.italic());
    styleSetUnderline(styleNumber, style.underline());
    styleSetEOLFilled(styleNumber, style.eolFilled());
}
