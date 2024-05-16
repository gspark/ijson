#include "jsonTreeModel.h"
#include "jsonTreeItem.h"
#include "common/icondb.h"
#include "component/findReplaceWidget.h"
#include "common/Logger.h"

#include <QRegularExpression>
#include <QFile>

#include <nlohmann/json.hpp>
#include <Stack>


const QString ROOT = "JSON";
const QString ARRAY_IDENTITY = "-";

JsonTreeModel::JsonTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
    , rootItem(new JsonTreeItem(nullptr))
{
    //addHeaders(QStringList{ tr("Key"), tr("Value") });
}

JsonTreeModel::~JsonTreeModel()
{
    delete rootItem;
}

bool JsonTreeModel::loadJson(ordered_json& json)
{
    //清空之前的model
    rootItem->deleteAllChild();

    if (json.is_object()) {
        rootItem->setType(JsonTreeItem::Object);
        parseObject(ROOT, json, rootItem);
    }
    else if (json.is_array()) {
        rootItem->setType(JsonTreeItem::Array);
        parseArray(ROOT, json, rootItem);
    }
    return true;
}

int JsonTreeModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return static_cast<JsonTreeItem*>(parent.internalPointer())->columnCount();
    }
    return rootItem->columnCount();
}

QVariant JsonTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DecorationRole && index.column() == 0)
    {
        JsonTreeItem* item = static_cast<JsonTreeItem*>(index.internalPointer());
        switch (item->type())
        {
        case JsonTreeItem::Object:
            return IconDb::instance()->getIcon(IconDb::Object);
        case JsonTreeItem::Array:
            return IconDb::instance()->getIcon(IconDb::Array);
        case JsonTreeItem::Null:
            return IconDb::instance()->getIcon(IconDb::Null);
        case JsonTreeItem::Bool:
            return IconDb::instance()->getIcon(IconDb::Bool);
        case JsonTreeItem::String:
            return IconDb::instance()->getIcon(IconDb::String);
        case JsonTreeItem::Number:
            return IconDb::instance()->getIcon(IconDb::Number);
        default:
            return QVariant();
        }
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole) {
        return QVariant();
    }

    JsonTreeItem* item = static_cast<JsonTreeItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags JsonTreeModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
        //return QAbstractItemModel::flags(index);
    }
    //if (index.column() == 0)
    //{
    //    if (!index.parent().isValid())
    //    {
    //        return Qt::ItemIsEnabled;
    //    }
    //    JsonTreeItem* item = static_cast<JsonTreeItem*>(index.internalPointer());
    //    if (QString::compare(ARRAY_IDENTITY, item->key()) == 0)
    //    {
    //        return Qt::ItemIsEnabled;
    //    }
    //}
    //else if (index.column() == 1)
    //{
    //    QString text = index.model()->data(index, Qt::EditRole).toString();
    //    if (text.isEmpty())
    //    {
    //        return Qt::NoItemFlags;
    //    }
    //}
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}


QVariant JsonTreeModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return rootItem->data(section);
    }
    return QVariant();
}

QModelIndex JsonTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    JsonTreeItem* parentItem;

    if (!parent.isValid()) {
        parentItem = rootItem;
    }
    else {
        parentItem = static_cast<JsonTreeItem*>(parent.internalPointer());
    }

    JsonTreeItem* childItem = parentItem->childItem(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    }
    return QModelIndex();
}

QModelIndex JsonTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    JsonTreeItem* childItem = static_cast<JsonTreeItem*>(index.internalPointer());
    JsonTreeItem* parentItem = childItem->parentItem();

    if (childItem == rootItem)
    {
        return QModelIndex();
    }
    return createIndex(parentItem->row(), 0, parentItem);
}


int JsonTreeModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0) {
        return 0;
    }

    JsonTreeItem* parentItem;
    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<JsonTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}


QModelIndex JsonTreeModel::index(int row) const
{
    ////QModelIndex rootIndex = index(0, 0);
    ////if (!rootIndex.isValid())
    ////{
    ////    return rootIndex;
    ////}

    ////int count = 0;
    ////const QAbstractItemModel* model = rootIndex.model();

    ////if (model->rowCount() == 0) {
    ////    return rootIndex;
    ////}

    ////QModelIndex index = model->index(0, 0, rootIndex);
    ////while (index != rootIndex) {
    ////    count++;
    ////    if (model->hasChildren(index)) {
    ////        index = model->index(0, 0, index);
    ////    }
    ////    else {
    ////        if (model->rowCount(index.parent()) == index.row() + 1) {
    ////            index = index.parent();
    ////        }
    ////        if (index != rootIndex) {
    ////            index = model->index(index.row() + 1, 0, index.parent());
    ////        }
    ////    }
    ////    if (count >= row)
    ////    {
    ////        return createIndex(row, 0, getItem(index));
    ////    }
    ////}

    if (rootItem->childCount() == 0) {
        return QModelIndex();
    }

    if (row < jsonTreeItems.size())
    {
        JsonTreeItem* item = jsonTreeItems[row];
        if (item)
        {
            return createIndex(row, 0, item);
        }
    }
    return QModelIndex();
}

void JsonTreeModel::parseObject(const QString& key, ordered_json& obj, JsonTreeItem* item)
{
    if (QString::compare(ARRAY_IDENTITY, key) != 0 && obj.is_object())
    {
        addTreeItemEmpty(key, rootItem->columnCount(), item);
        item = item->childItem(item->childCount() - 1);
    }
    for (auto it = obj.begin(); it != obj.end(); ++it)
    {
        parseValue(QString::fromStdString(it.key()), it.value(), item);
    }
}

void JsonTreeModel::parseArray(const QString& key, ordered_json& arr, JsonTreeItem* item)
{
    addTreeItemEmpty(key, rootItem->columnCount(), item);

    for (auto& j : arr.items()) {
        parseValue(ARRAY_IDENTITY, j.value(), item->childItem(item->childCount() - 1));
    }
}

void JsonTreeModel::parseValue(const QString& key, ordered_json& val, JsonTreeItem* item)
{
    QVariant the_val;
    QHash<int, QVariant> datas;
    JsonTreeItem::JsonItemType type = JsonTreeItem::Table;

    switch (val.type()) {
    case nlohmann::detail::value_t::object:
        type = JsonTreeItem::Object;
        item->setType(type);
        parseObject(key, val, item);
        return;
    case nlohmann::detail::value_t::array:
        type = JsonTreeItem::Array;
        item->setType(type);
        parseArray(key, val, item);
        return;
    case nlohmann::detail::value_t::boolean:
        the_val = QVariant::fromValue(val.get<bool>());
        type = JsonTreeItem::Bool;
        datas = { {0,key}, {1,the_val} };
        break;
    case nlohmann::detail::value_t::number_float:
        the_val = QVariant::fromValue(val.get<double>());
        type = JsonTreeItem::Number;
        datas = { {0,key}, {1,the_val} };
        break;
    case nlohmann::detail::value_t::number_unsigned:
        the_val = QVariant::fromValue(val.get<unsigned int>());
        type = JsonTreeItem::Number;
        datas = { {0,key}, {1,the_val} };
        break;
    case nlohmann::detail::value_t::number_integer:
        the_val = QVariant::fromValue(val.get<int>());
        type = JsonTreeItem::Number;
        datas = { {0,key}, {1,the_val} };
        break;
    case nlohmann::detail::value_t::string:
        the_val = QString::fromStdString(val.get<std::string>());
        type = JsonTreeItem::String;
        datas = { {0,key}, {1,the_val} };
        break;
    case nlohmann::detail::value_t::null:
        the_val = "null";
        type = JsonTreeItem::Null;
        datas = { {0,key}, {1,the_val} };
        break;
    default: break;
    }

    JsonTreeItem* child = new JsonTreeItem(datas, type, item);
    child->setRowOfTree(item->getRowOfTree() + item->childCount() + 1);
    jsonTreeItems.push_back(child);
    item->appendChild(child);
}

void JsonTreeModel::addTreeItemEmpty(const QString key, const int count_сolumns, JsonTreeItem* parent)
{
    QHash<int, QVariant> root_data;
    root_data.insert({ {0, key} });
    for (int i = 1; i < count_сolumns; ++i) {
        root_data.insert(i, "");
    }

    JsonTreeItem* item = new JsonTreeItem(root_data, parent->type(), parent);
    jsonTreeItems.push_back(item);
    item->setRowOfTree(parent->getRowOfTree() + parent->childCount() + 1);
    parent->appendChild(item);
}

void JsonTreeModel::clear()
{
    beginResetModel();
    rootItem->deleteAllChild();
    endResetModel();
}

bool JsonTreeModel::find(const QString& findText, const FindFlags* searchFlags, bool forward, bool wrap, bool* searchWrapped, QModelIndex* currentIndex)
{
    if (nullptr == currentIndex || !currentIndex->isValid())
    {
        auto i = index(0, 0);
        currentIndex = &i;
    }
    if (!currentIndex->isValid())
    {
        return 0;
    }

    std::stack<QModelIndex*> nodes;
    nodes.push(currentIndex);

    while (!nodes.empty())
    {
        auto* index = nodes.top();
        nodes.pop();

        auto item = static_cast<JsonTreeItem*>(index->internalPointer());
        if (stringCompare(findText, item->key(), searchFlags) || stringCompare(findText, item->data(1).toString(), searchFlags))
        {
            currentIndex = index;
            return true;
        }

        int rowCount = this->rowCount(*index);
        if (rowCount > 0)
        {
            for (int row = 0; row < rowCount; ++row)
            {
                QModelIndex index = this->index(row, 1, index);
                nodes.push(&index);
            }
        }
        else
        {
            QModelIndex parent = index->parent();
            while (nodes.empty())
            {
                if (parent.isValid())
                {
                    auto item = static_cast<JsonTreeItem*>(parent.internalPointer());
                    int row = item->row() + 1;
                    if (row < item->childCount())
                    {
                        QModelIndex index = this->index(row, 1, parent);
                        nodes.push(&index);
                    }
                }
                if (nodes.empty())
                {
                    parent = parent.parent();
                }
            }
        }
    }
    return false;
}


int JsonTreeModel::find(const QString& findText, const FindFlags* searchFlags)
{
    auto currentIndex = index(0, 0);
    if (!currentIndex.isValid())
    {
        return 0;
    }

    auto item = static_cast<JsonTreeItem*>(currentIndex.internalPointer());
    std::stack<JsonTreeItem*> nodes;
    nodes.push(item);

    int count = 0;
    while (!nodes.empty())
    {
        auto* item = nodes.top();
        nodes.pop();
        if (item == nullptr)
        {
            continue;
        }

        int find = 0;
        LOG_INFO << "item->key(): " << item->key() << " value: " << item->data(1).toString();

        if (stringCompare(findText, item->key(), searchFlags))
        {
            find = 0x0001;
        }
        if(stringCompare(findText, item->data(1).toString(), searchFlags)) 
        {
            find = 0x0010 | find;
        }
        item->setFind(find);
        if (find > 0)
        {
            count++;
        }

        int rowCount = item->childCount();
        for (int row = 0; row < rowCount; ++row)
        {
            nodes.push(item->childItem(row));
        }
    }
    return count;
}

void JsonTreeModel::findClear()
{
    std::stack<JsonTreeItem*> nodes;
    nodes.push(rootItem);

    while (!nodes.empty())
    {
        auto* item = nodes.top();
        nodes.pop();
        if (item == nullptr)
        {
            continue;
        }
        item->setFind(0);
        int rowCount = item->childCount();
        for (int row = 0; row < rowCount; ++row)
        {
            nodes.push(item->childItem(row));
        }
    }
}

bool JsonTreeModel::stringCompare(const QString& findText, const QString& source, const FindFlags* searchFlags)
{
    if (searchFlags->regex)
    {
        if (searchFlags->matchCase)
        {
            QRegularExpression re(findText);
            return re.match(source).hasMatch();
        }
        QRegularExpression re(findText, QRegularExpression::CaseInsensitiveOption);
        return re.match(source).hasMatch();
    }

    if (searchFlags->matchCase && searchFlags->words)
    {
        return QString::compare(findText, source, Qt::CaseSensitive) == 0;
    }
    else if (searchFlags->words)
    {
        return QString::compare(findText, source, Qt::CaseInsensitive) == 0;
    }
    else if (searchFlags->matchCase)
    {
        return source.contains(findText, Qt::CaseSensitive);
    }

    return source.contains(findText, Qt::CaseInsensitive);
}


