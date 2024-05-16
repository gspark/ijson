#ifndef jsonFormatter_h__
#define jsonFormatter_h__
#pragma once

#include <QWidget>
#include "jsonTree\orderedJson.h"

class Buffer;
class QTreeView;
class FindReplaceWidget;
struct FindFlags;
class QShortcut;

class JsonFormatter : public QWidget
{
    Q_OBJECT

public:
    explicit JsonFormatter(QWidget* parent = nullptr);
    ~JsonFormatter() override;

    void formatDoc();
    void clear();
    void compress();
    void searchForDoc();

    void loadFile(const QString& jsonFile);
    int lineCount();
signals:
    void message(QString message);
    void cursorPositionChanged(intptr_t line, int pos);
    void textLengthChanged(intptr_t length, intptr_t lines);
protected:
    virtual void resizeEvent(QResizeEvent* event);

    void setSearchGeometry(bool fromResize);
private slots:
    void find(const QString& findText, const FindFlags* searchFlags, bool forward, bool wrap);
    void slot_cursorPositionChanged(intptr_t line, int pos);
    void slot_textLengthChanged(intptr_t length, intptr_t lines);
    void key_escape();

    void splitterMoved(int pos, int index);
private:
    Buffer* editor;
    QTreeView* treeView;
    FindReplaceWidget* search;
    QShortcut* escapeStc;

    int treeFindCount;
    bool searchInRight;

    int searchFlags(const FindFlags* searchFlags);

    void itemSelectedManually(const QModelIndex& ar_index);

    void treeLoadJson(ordered_json& json);

    void loadJson(bool format);
};

#endif // jsonFormatter_h__
