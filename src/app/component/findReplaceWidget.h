#pragma once
#ifndef findReplaceWidget_h__
#define findReplaceWidget_h__

#include <QToolBar>

class QLineEdit;

struct FindFlags
{
    bool matchCase;
    bool words;
    bool regex;
};

class FindReplaceWidget : public QToolBar
{
    Q_OBJECT
public:
    /**
    * Enumeration for the dialog type, either a find or a find/replace
    * dialog.
    */
    enum Type {
        Find = 0,
        FindReplace = 1
    };

    explicit FindReplaceWidget(QWidget* parent = nullptr);
    explicit FindReplaceWidget(const QString&, QWidget* parent = nullptr);
    ~FindReplaceWidget() override;

    void setSearchFor(const QString&);
    void setOccurrence(bool show);
signals:
    void find(const QString& findText, const FindFlags* searchFlags, bool forward, bool wrap);

private slots:
    void caseToggled(bool enabled);
    void wordsToggled(bool enabled);
    void regexToggled(bool enabled);

    void visibilityChanged(bool visible);

    void prev();
    void next();
    void search();
private:
    FindFlags findFlags;
    QLineEdit* findLindEdit;

    QAction* prevAct;
    QAction* nextAct;
};

#endif // findReplaceWidget_h__

