#ifndef JSONTREEITEM_H
#define JSONTREEITEM_H

#include <QList>
#include <QVariant>

class JsonTreeItem
{
public:
    enum JsonItemType {
        Null,
        Bool,
        Number,
        Object,
        Array,
        String,
        Table
    };
public:
    explicit JsonTreeItem(JsonTreeItem* parent = nullptr);
    explicit JsonTreeItem(const QHash<int, QVariant>& datas, JsonTreeItem::JsonItemType type, JsonTreeItem* parent = nullptr);
    virtual ~JsonTreeItem();

    bool insertChild(int row, JsonTreeItem* child);
    bool removeChild(int row);
    bool insertChildren(int row, int count, int columns);
    bool removeChildren(int row, int count);
    void appendChild(JsonTreeItem* child);
    void deleteAllChild();

    JsonTreeItem* childItem(int row);
    JsonTreeItem* parentItem();
    void setParentItem(JsonTreeItem* parent);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool setData(int column, const QVariant& val);
    int row() const;
    bool editable(int column) const;
    //QHash<int,QByteArray> roleNames() const;

    bool insertColumns(int pos, int count);
    bool removeColumns(int pos, int count);

    QString key() const;
    void setKey(const QString& key);
    QVariant value() const;
    void setValue(const QVariant& value);
    JsonTreeItem::JsonItemType type() const;
    void setType(JsonTreeItem::JsonItemType type);

    void setRowOfTree(int row);
    int getRowOfTree();

    void setFind(unsigned int find);
    unsigned int getFind() const;
private:
    JsonTreeItem* theParentItem;
    QList<JsonTreeItem*> theChildItems;
    JsonItemType theItemType;

    QHash<int, QVariant> theItemDatas;

    int rowOfTree;

    unsigned int find;
};

#endif // JSONTREEITEM_H
