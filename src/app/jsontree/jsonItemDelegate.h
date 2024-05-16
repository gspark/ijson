#pragma once
#ifndef jsonItemDelegate_h__
#define jsonItemDelegate_h__

#include <QStyledItemDelegate>

class JsonItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit JsonItemDelegate(QObject* parent = nullptr);
    explicit JsonItemDelegate(const QString& theme, QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
private:
    QString theme;
};


#endif // jsonItemDelegate_h__