#include "jsonFormatter.h"
#include "editor/buffer.h"
#include "jsontree/JsonTreeModel.h"
#include "jsontree/jsonItemDelegate.h"
#include <nlohmann/json.hpp>
#include "common/IconDb.h"
#include "component/findReplaceWidget.h"
#include "common/Logger.h"
#include "common/config.h"
#include "component/clpsSplitter.h"

#include <QHBoxLayout>
#include <QTreeView>
#include <QHeaderView>
#include <QStyleFactory>
#include <QAbstractItemView>
#include <QKeyEvent>
#include <QScrollBar>
#include <QShortcut>

JsonFormatter::JsonFormatter(QWidget* parent)
    : QWidget(parent)
    , search(nullptr)
    , treeFindCount(0)
    , searchInRight(false)
{
    auto* splitter = new ClpsSplitter(this);

    auto* hLayout = new QHBoxLayout(this);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->addWidget(splitter);

    editor = new Buffer(splitter);
    connect(editor, &Buffer::cursorPositionChanged, this, &JsonFormatter::slot_cursorPositionChanged, Qt::QueuedConnection);
    connect(editor, &Buffer::textLengthChanged, this, &JsonFormatter::slot_textLengthChanged, Qt::QueuedConnection);

    treeView = new QTreeView(splitter);
    treeView->setModel(new JsonTreeModel);
    treeView->setItemDelegate(new JsonItemDelegate(ConfigIni::getInstance().iniRead(QStringLiteral("Configuration/theme"), "").toString()));

    // 去掉边框
    treeView->setFrameStyle(QFrame::NoFrame);
    // 去掉虚线边框
    treeView->setRootIsDecorated(false);
    // 去掉子节点的虚框
    //treeView->setStyleSheet("QTreeView::branch {image:none;}");
    // 显示节点连线
    treeView->setStyle(QStyleFactory::create("windows"));

    //QFont font("Arial", 9, 100);
    //font.setBold(true);
    //treeView->header()->setFont(font);
    // 最后一列自适应宽度
    treeView->header()->setStretchLastSection(true);
    //treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    treeView->setHeaderHidden(true);
    //选中单元格
    //treeView->setSelectionBehavior(QAbstractItemView::SelectItems);
    //支持shift, ctrl, 鼠标框框等方式多选，QAbstractItemView::SingleSelection表示一次只能选中一行
    //treeView->setSelectionMode(QAbstractItemView::NoSelection);
    treeView->setFont(ConfigIni::getInstance().font(QStringLiteral("Viewer/FontName")));

    splitter->addWidget(editor);
    splitter->addWidget(treeView);

    escapeStc = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    QObject::connect(escapeStc, &QShortcut::activated, this, &JsonFormatter::key_escape);

    connect(splitter, &QSplitter::splitterMoved, this, &JsonFormatter::splitterMoved);
}

JsonFormatter::~JsonFormatter()
{
    delete escapeStc;
    if (search)
    {
        delete search;
    }
    this->deleteLater();
}

void JsonFormatter::loadFile(const QString& jsonFile)
{
    if (editor->open(jsonFile))
    {
        loadJson(false);
    }
}

int JsonFormatter::lineCount()
{
    return this->editor->lineCount();
}

void JsonFormatter::loadJson(bool format)
{
    QByteArray data = editor->getText(editor->length());
    short tryCount = 0;
    do {
        try
        {
            std::string txt = data.toStdString();
            if (tryCount == 1)
            {
                if (txt.find('<') == std::string::npos)
                {
                    txt.erase(std::remove_if(txt.begin(), txt.end(), [](int c) { return c == '\\' | c == '\n'; }), txt.end());
                }
                else
                {
                    qsizetype p = data.indexOf("\\\"");
                    if (p > 0)
                    {
                        data.remove(p, 1);
                        txt = data.toStdString();
                        editor->setText(data);
                    }
                }
            }
            auto json = ordered_json::parse(txt, nullptr, true);
            if (format)
            {
                editor->setText(json.dump(4, ' ', false).c_str());
            }
            return treeLoadJson(json);
        }
        catch (nlohmann::detail::exception& e)
        {
            if (tryCount == 1)
            {
                QString what = e.what();
                int p1 = what.indexOf("]");
                p1 = p1 > 0 ? p1 + 2 : 0;

                qsizetype p2 = what.indexOf(";");
                if (p2 == -1)
                {
                    p2 = what.length() - 1;
                }
                emit message(what.mid(p1, p2 - p1));
                return;
            }
        }
        tryCount++;
    } while (tryCount < 2);
}

void JsonFormatter::formatDoc()
{
    loadJson(true);
}

void JsonFormatter::treeLoadJson(ordered_json& json)
{
    auto* jsonModel = static_cast<JsonTreeModel*>(treeView->model());
    jsonModel->loadJson(json);
    treeView->setColumnWidth(0, 256);
    treeView->expandAll();
}

void JsonFormatter::clear()
{
    editor->clear();
    auto* jsonModel = static_cast<JsonTreeModel*>(treeView->model());
    jsonModel->clear();
}

void JsonFormatter::compress()
{
    QByteArray data = editor->getText(editor->length());
    auto json_doc = ordered_json::parse(data.toStdString(), nullptr, false);
    if (json_doc.is_discarded())
    {
        return;
    }
    editor->setText(json_doc.dump(-1, ' ', false, nlohmann::detail::error_handler_t::ignore).c_str());
}

void JsonFormatter::searchForDoc()
{
    if (nullptr == this->search)
    {
        if (treeView->hasFocus())
        {
            search = new FindReplaceWidget("", this);
        }
        else {
            search = new FindReplaceWidget(editor->getSelText(), this);
        }
        connect(search, &FindReplaceWidget::find, this, &JsonFormatter::find);
    }

    searchInRight = treeView->hasFocus();

    setSearchGeometry(false);
    search->show();
}

void JsonFormatter::resizeEvent(QResizeEvent* event)
{
    if (search && search->isVisible())
    {
        setSearchGeometry(true);
    }
    QWidget::resizeEvent(event);
}

void JsonFormatter::find(const QString& findText, const FindFlags* searchFlags, bool forward, bool wrap)
{
    bool searchWrapped;
    bool found = false;

    if (search->x() == treeView->x())
    {
        auto* jsonModel = static_cast<JsonTreeModel*>(treeView->model());
        treeFindCount = jsonModel->find(findText, searchFlags);
        if (treeFindCount > 0)
        {
            treeView->viewport()->update();
        }
    }
    else {
        found = this->editor->find(findText, this->searchFlags(searchFlags), forward, wrap, &searchWrapped);
    }
    
    if (found) {
        emit message(searchWrapped ? tr("Search wrapped.") : tr(""));
    }
    else {
        emit message(tr("The text was not found."));
    }
}

int JsonFormatter::searchFlags(const FindFlags* searchFlags)
{
    int flags = 0;
    if (searchFlags->matchCase) {
        flags |= SCFIND_MATCHCASE;
    }
    if (searchFlags->words) {
        flags |= SCFIND_WHOLEWORD;
    }
    if (searchFlags->regex) {
        flags |= SCFIND_REGEXP;
    }
    return flags;

}

void JsonFormatter::itemSelectedManually(const QModelIndex& ar_index)
{
    std::vector<std::pair<int, int> > indexes;

    // first of all, I save all item "offsets" relative to its parent 

    QModelIndex indexAbobe = ar_index.parent();
    while (indexAbobe.isValid())
    {
        indexes.push_back(std::make_pair(indexAbobe.row(), indexAbobe.column()));
        indexAbobe = indexAbobe.parent();
    }

    // now, select actual selection model 

    auto model = treeView->selectionModel()->model();

    // get root item 

    QModelIndex index = model->index(0, 0, QModelIndex());
    if (index.isValid())
    {
        // now, expand all items below 

        for (auto it = indexes.rbegin(); it != indexes.rend() && index.isValid(); ++it)
        {
            auto row = (*it).first;
            auto colum = (*it).second;

            treeView->setExpanded(index, true);

            // and get a new item relative to parent 
            index = model->index(row, colum, index);
        }
    }

    // finally, scroll to real item, after expanding everything above. 
    treeView->scrollTo(ar_index);
}

void JsonFormatter::slot_cursorPositionChanged(intptr_t line, int pos)
{
    //auto* model = static_cast<JsonTreeModel*>(treeView->model());
    //QModelIndex index = model->index(line, 0);
    //if (index.isValid())
    //{
    //    LOG_INFO << "model->index: " << index.data(Qt::DisplayRole);
    //    //itemSelectedManually(index);
    //    //treeView->setCurrentIndex(index);
    //    treeView->scrollTo(index);
    //    //QRect rect = treeView->visualRect(index);
    //    //treeView->horizontalScrollBar()->setValue(rect.x());

    //}
    emit cursorPositionChanged(line, pos);
}

void JsonFormatter::slot_textLengthChanged(intptr_t length, intptr_t lines)
{
    emit textLengthChanged(length, lines);
}

void JsonFormatter::key_escape()
{
    if (search->isHidden())
    {
        if (treeFindCount > 0)
        {
            auto* jsonModel = static_cast<JsonTreeModel*>(treeView->model());
            jsonModel->findClear();
            treeFindCount = 0;
            treeView->viewport()->update();
        }
    }
    else
    {
        search->hide();
    }
}

void JsonFormatter::splitterMoved(int pos, int index)
{
    if (search && search->isVisible())
    {
        setSearchGeometry(true);
    }
}

void JsonFormatter::setSearchGeometry(bool fromResize)
{
    LOG_INFO << "searchInRight: " << searchInRight;
    int w = 0;
    if ((searchInRight && fromResize) || treeView->hasFocus())
    {
        if (treeView->verticalScrollBar()->isVisible())
        {
            w = treeView->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
        }
        search->setOccurrence(false);
        search->setFixedWidth(this->treeView->width() - w);
        search->move(treeView->x(), treeView->y());
    }
    else
    {
        if (editor->verticalScrollBar()-isVisible())
        {
            w = editor->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
        }
        search->setOccurrence(true);
        search->setFixedWidth(this->editor->width() - w);
        search->move(editor->x(), editor->y());
    }
}

