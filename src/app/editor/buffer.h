#ifndef BUFFER_H
#define BUFFER_H

#include "styleinfo.h"

#include <ScintillaEdit.h>

#include <QFileInfo>
#include <QList>
#include <QUrl>
#include <QWidget>


class Buffer : public ScintillaEdit {
    Q_OBJECT

public:
    /**
     * Enumeration that holds the various margins.
     */
    enum Margin {
        Line, Icon, Fold
    };

    /**
     * Creates the buffer.
     *
     * @param parent The parent window.
     */
    explicit Buffer(QWidget* parent = 0);

    /**
     * Destructor for the buffer.
     */
    virtual ~Buffer();

    /**
     * Clears the contents of the editor.
     */
    void clear();

    /**
     * Reads the contents of a file into the buffer.
     *
     * @param fileName The name of the file.
     * @return true, if the file has been opened successfully.
     */
    bool open(const QString& fileName);

    /**
     * Saves the contents of the buffer to a file.
     *
     * @param fileName The name of the file to save the contents.
     * @return true, if the file has been saved successfully.
     */
    bool save(const QString& fileName);

    /**
     * Returns the path of the file.
     *
     * @return The path of the file, or a null string if the buffer has not been saved yet.
     */
    QFileInfo fileInfo() const;

    /**
     * Applies the specified color scheme to the buffer.
     *
     * @param colorScheme The color scheme to apply.
     */
    void setColorScheme(const QString& theme);

    /**
     * Sets whether the indentation guides should be shown or not.
     *
     * @param viewIndentationGuides true if the indentation guides should be.
     * shown.
     */
    void setViewIndentationGuides(bool viewIndentationGuides);

    /**
     * Set the language for the buffer.
     *
     * @param lang The language.
     */
    void setLanguage(const QString& language);

    /**
     * Returns true if the line numbers are shown.
     *
     * @return true if the line numbers are shown.
     */
    bool showLineNumbers() const;

    /**
     * Shows or hides the line numbers.
     *
     * @param showLineNumbers if true, show the line numbers.
     */
    void setShowLineNumbers(bool showLineNumbers);

    /**
     * Sets the font for the specific style.
     *
     * @param style The style.
     * @param font The font.
     */
    void setStyleQFont(int style, const QFont& font);

    /**
     * Finds the occurance of the provided text and selects the match.
     *
     * @param findText The text to find.
     * @param flags The search flags.
     * @param forward If true, perform the search with forward direction.
     * @param wrap If true, wrap the search.
     * @param searchWrapped Input parameter, which is set to true if the search is wrapped.
     * @return true if a match was found.
     */
    bool find(const QString& findText, int flags, bool forward, bool wrap, bool* searchWrapped);

signals:
    /**
     * Emitted when the underlying file for this buffer has changed.
     *
     * @param fileInfo The new file information.
     */
    void fileInfoChanged(const QFileInfo& fileInfo);
    void cursorPositionChanged(intptr_t line, int column);
    void textLengthChanged(intptr_t length, intptr_t lines);
public slots:
    /**
     * Called when either the text or styling of the document has changed or the selection range or scroll position has
     * changed.
     */
    void onUpdateUi(Scintilla::Update updated);

    /**
     * Called when lines are added to the buffer.
     *
     * @param linesAdded The number of lines added.
     */
    void onLinesAdded(int linesAdded);
private:
    /**
     * Loads the editor preferences from the configuration.
     */
    void loadConfiguration();

    /**
     * Calculates the width of the line margin, in order to accomodate the largest line number.
     *
     * @return The line width.
     */
    int getLineMarginWidth();
    /**
     * Sets the file information of the underlying file.
     *
     * @param fileInfo The new file information.
     */
    void setFileInfo(const QFileInfo& fileInfo);

    /**
     * Check if a character is a brance.
     *
     * @param character The character.
     * @return 1 if the character is an opening brace, -1 if it is a closing brace, 0 if it is neither.
     */
    int isBrace(sptr_t character);

    /**
     * Applies the specified style to the buffer.
     *
     * @param styleNumber The style number, as defined in the lexer.
     * @param style The style information.
     */
    void applyStyle(int styleNumber, const StyleInfo& style);

    /** The underlying file for this buffer. */
    QFileInfo m_fileInfo;

    /** If the the line margin width will be changed automatically in order to accomodate the biggest line number */
    bool m_trackLineWidth;

    /** True if the matching brace should be highighted. */
    bool m_braceHighlight;

    int lastPos;
    intptr_t lastLine;
    intptr_t lastTextLength;
};

#endif // BUFFER_H
