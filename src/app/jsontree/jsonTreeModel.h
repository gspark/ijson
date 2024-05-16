#ifndef JSONTREEMODEL_H
#define JSONTREEMODEL_H

#include <QAbstractItemModel>
#include "orderedJson.h"

class JsonTreeItem;
struct FindFlags;

class JsonTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit JsonTreeModel(QObject *parent = nullptr);
    ~JsonTreeModel() override;

    bool loadJson(ordered_json& json);

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex& index) const override;
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    void addTreeItemEmpty(const QString key, const int count_сolumns, JsonTreeItem* parent);
    
    void clear();

    QModelIndex index(int row) const;

    bool find(const QString& findText, const FindFlags* searchFlags, bool forward, bool wrap, bool* searchWrapped, QModelIndex* currentIndex);

    int find(const QString& findText, const FindFlags* searchFlags);

    void findClear();
private:

    void parseObject(const QString& key, ordered_json& obj, JsonTreeItem* item);
    void parseArray(const QString& key, ordered_json& arr, JsonTreeItem* item);
    void parseValue(const QString& key, ordered_json& val, JsonTreeItem* item);

    bool stringCompare(const QString& findText, const QString& source, const FindFlags* searchFlags);

private:
    JsonTreeItem* rootItem;
    std::vector<JsonTreeItem*> jsonTreeItems;
};

#endif // JSONTREEMODEL_H
