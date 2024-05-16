#include "jsonTreeItem.h"

JsonTreeItem::JsonTreeItem(JsonTreeItem* parent)
    //:theParentItem(parent)
    //, theItemType(JsonTreeItem::Table)
    //, theItemDatas({ {0,"Key"},{1,"Value"} })
    //, rowOfTree(-1)
    : JsonTreeItem({ {0,"Key"},{1,"Value"} }, JsonTreeItem::Table,parent)
{
}

JsonTreeItem::JsonTreeItem(const QHash<int, QVariant>& datas, JsonTreeItem::JsonItemType type, JsonTreeItem* parent)
    :theParentItem(parent)
    , theItemType(type)
    , theItemDatas(datas)
    , rowOfTree(-1)
    , find(0)
{
}

JsonTreeItem::~JsonTreeItem()
{
    deleteAllChild();
}

bool JsonTreeItem::insertChild(int row, JsonTreeItem* child)
{
    if (row < 0 || row > theChildItems.count()) {
        return false;
    }
    theChildItems.insert(row, child);
    child->setParentItem(this);
    return true;
}

bool JsonTreeItem::removeChild(int row)
{
    if ((row < 0) || ((qsizetype(row) + 1) > theChildItems.count())) {
        return false;
    }
    delete theChildItems.takeAt(row);
    return true;
}

bool JsonTreeItem::insertChildren(int row, int count, int columns)
{
    if (row < 0 || row > theChildItems.count()) {
        return false;
    }
    for (int i = 0; i < count; ++i) {
        theChildItems.insert(row, new JsonTreeItem(this));
    }
    return true;
}

bool JsonTreeItem::removeChildren(int row, int count)
{
    if ((row < 0) || ((qsizetype(row) + count) > theChildItems.count()))
        return false;
    for (int i = 0; i < count; i++) {
        delete theChildItems.takeAt(row + i);
    }
    return true;
}

void JsonTreeItem::appendChild(JsonTreeItem* child)
{
    theChildItems.append(child);
    child->setParentItem(this);
}

void JsonTreeItem::deleteAllChild()
{
    qDeleteAll(theChildItems);
    theChildItems.clear();
}

JsonTreeItem* JsonTreeItem::childItem(int row)
{
    return theChildItems.value(row);
}

JsonTreeItem* JsonTreeItem::parentItem()
{
    return theParentItem;
}

void JsonTreeItem::setParentItem(JsonTreeItem* parent)
{
    theParentItem = parent;
}

int JsonTreeItem::childCount() const
{
    return theChildItems.count();
}

int JsonTreeItem::columnCount() const
{
    return theItemDatas.count();
}

QVariant JsonTreeItem::data(int column) const
{
    return theItemDatas.value(column, QVariant());
}

bool JsonTreeItem::setData(int column, const QVariant& value)
{
    if (column < 0 || column >= theItemDatas.size())
        return false;
    theItemDatas.insert(column, value);
    return true;
}

int JsonTreeItem::row() const
{
    if (theParentItem)
        return theParentItem->theChildItems.indexOf(const_cast<JsonTreeItem*>(this));
    return 0;
}

bool JsonTreeItem::editable(int column) const
{
    if ((!theParentItem || !theParentItem->parentItem()) ||
        ((0 == column) && (theParentItem->type() == JsonTreeItem::Array)) ||
        ((1 == column) && ((type() == JsonTreeItem::Array) || (type() == JsonTreeItem::Object))))
        return false;
    return true;
}

bool JsonTreeItem::insertColumns(int position, int columns)
{
    if (position < 0 || position > theItemDatas.size())
        return false;
    for (int column = 0; column < theItemDatas.size() - position; column++)
        theItemDatas.insert(theItemDatas.size() - 1 + columns - column,
            theItemDatas[theItemDatas.size() - 1 - column]);
    for (int column = 0; column < columns; column++)
        theItemDatas.insert(position + column, QVariant(""));
    foreach(JsonTreeItem * child, theChildItems)
        child->insertColumns(position, columns);
    return true;
}

bool JsonTreeItem::removeColumns(int position, int columns)
{
    if ((position < 0) || ((qsizetype(position) + columns) > theItemDatas.size()))
        return false;
    for (int column = 0; column < columns; ++column) {
        int inx = position + column;
        if (inx > theItemDatas.size()) {
            break;
        }
        theItemDatas.remove(inx);
    }
    for (int column = 0; column < theItemDatas.size() - position - columns; column++)
        theItemDatas.insert(position + column, theItemDatas[position + columns + column]);
    foreach(JsonTreeItem * child, theChildItems)
        child->removeColumns(position, columns);
    return true;
}

QString JsonTreeItem::key() const
{
    return theItemDatas.value(0, "").toString();
}

void JsonTreeItem::setKey(const QString& key)
{
    theItemDatas[0] = key;
}

QVariant JsonTreeItem::value() const
{
    return theItemDatas.value(1, 0);
}

void JsonTreeItem::setValue(const QVariant& value)
{
    theItemDatas[1] = value;
}

JsonTreeItem::JsonItemType JsonTreeItem::type() const
{
    return theItemType;
}

void JsonTreeItem::setType(JsonTreeItem::JsonItemType type)
{
    theItemType = type;
}

void JsonTreeItem::setRowOfTree(int row)
{
    this->rowOfTree = row;
}

int JsonTreeItem::getRowOfTree()
{
    return this->rowOfTree;
}

void JsonTreeItem::setFind(unsigned int find)
{
    this->find = find;
}

unsigned int JsonTreeItem::getFind() const
{
    return this->find;
}

