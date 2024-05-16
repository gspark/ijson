#include "findReplaceWidget.h"
#include "../common/icondb.h"

#include <QLineEdit>
#include <QLabel>
#include <QKeyEvent>

FindReplaceWidget::FindReplaceWidget(QWidget* parent /*= nullptr*/)
    :FindReplaceWidget("", parent)
{
}

FindReplaceWidget::FindReplaceWidget(const QString& searchFor, QWidget* parent /*= nullptr*/)
    : QToolBar(parent)
    , findFlags{ false,false,false }
{
    QPalette pl = parent->palette();
    setStyleSheet(tr("background-color:%1").arg(pl.base().color().name()));
    
    QAction* searchForAct = this->addAction(IconDb::instance()->getIcon(IconDb::search), tr("Search"));
    connect(searchForAct, &QAction::triggered, this, &FindReplaceWidget::search);

    findLindEdit = new QLineEdit(this);
    findLindEdit->setClearButtonEnabled(true);
    findLindEdit->setPlaceholderText(tr("Search for..."));
    this->setSearchFor(searchFor);
    connect(findLindEdit, &QLineEdit::returnPressed, this, &FindReplaceWidget::search);

    this->addWidget(findLindEdit);

    this->addSeparator();

    auto* caseAct = this->addAction(IconDb::instance()->getIcon(IconDb::casesensitively), tr("Match Case"));
    caseAct->setCheckable(true);
    connect(caseAct, &QAction::triggered, this, &FindReplaceWidget::caseToggled);

    auto* wordsAct = this->addAction(IconDb::instance()->getIcon(IconDb::wholewords), tr("Words"));
    wordsAct->setCheckable(true);
    connect(wordsAct, &QAction::triggered, this, &FindReplaceWidget::wordsToggled);

    auto* regexAct = this->addAction(IconDb::instance()->getIcon(IconDb::regexp), tr("Regex"));
    regexAct->setCheckable(true);
    connect(regexAct, &QAction::triggered, this, &FindReplaceWidget::wordsToggled);

    auto* results = new QLabel(this);
    results->setVisible(false);
    results->setText(tr("%1 results").arg(0));
    this->addWidget(results);

    prevAct = this->addAction(IconDb::instance()->getIcon(IconDb::prev), tr("Previous Occurrence"));
    connect(prevAct, &QAction::triggered, this, &FindReplaceWidget::prev);

    nextAct = this->addAction(IconDb::instance()->getIcon(IconDb::next), tr("Next Occurrence"));
    connect(nextAct, &QAction::triggered, this, &FindReplaceWidget::next);

    auto* spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->addWidget(spacer);

    auto* closeAct = this->addAction(IconDb::instance()->getIcon(IconDb::close), tr("Close"));
    connect(closeAct, &QAction::triggered, this, &FindReplaceWidget::hide);

    connect(this, &QToolBar::visibilityChanged, this, &FindReplaceWidget::visibilityChanged);
}

FindReplaceWidget::~FindReplaceWidget()
{
    //delete _searchFlags;
    this->deleteLater();
}

void FindReplaceWidget::setSearchFor(const QString& searchFor)
{
    if (!searchFor.isEmpty())
    {
        this->findLindEdit->setText(searchFor);
    }
}

void FindReplaceWidget::setOccurrence(bool show)
{
    if (show)
    {
        if (!prevAct->isVisible())
        {
            prevAct->setVisible(true);
        }
        if (!nextAct->isVisible())
        {
            nextAct->setVisible(true);
        }
    }
    else 
    {
        if (prevAct->isVisible())
        {
            prevAct->setVisible(false);
        }
        if (nextAct->isVisible())
        {
            nextAct->setVisible(false);
        }
    }
}

void FindReplaceWidget::caseToggled(bool enabled)
{
    findFlags.matchCase = enabled;
}

void FindReplaceWidget::regexToggled(bool enabled)
{
    findFlags.words = enabled;
}

void FindReplaceWidget::visibilityChanged(bool visible)
{
    if (visible)
    {
        this->findLindEdit->setFocus();
    }
}

void FindReplaceWidget::wordsToggled(bool enabled)
{
    findFlags.regex = enabled;
}

void FindReplaceWidget::search()
{
    emit find(findLindEdit->text().trimmed(), &findFlags, true, true);
}

void FindReplaceWidget::prev()
{
    emit find(findLindEdit->text().trimmed(), &findFlags, false, true);
}

void FindReplaceWidget::next()
{
    search();
}

