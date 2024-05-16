#include "jsonItemDelegate.h"
#include "jsonTreeItem.h"
#include "jsonPlainTextEdit.h"
#include "common/config.h"
#include "editor/util.h"

#include <QtWidgets>

JsonItemDelegate::JsonItemDelegate(QObject* parent /*= nullptr*/)
    : JsonItemDelegate("", parent)
{

}

JsonItemDelegate::JsonItemDelegate(const QString& t, QObject* parent /*= nullptr*/)
    : QStyledItemDelegate(parent)
    , theme(t)
{

}

void JsonItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (!index.isValid()) {
        QStyledItemDelegate::paint(painter, option, index);
    }

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    auto* item = static_cast<JsonTreeItem*>(index.internalPointer());
    if (index.column() == 1) {
        if (item)
        {
            QString arg1 = "";
            switch (item->type())
            {
            case JsonTreeItem::Object:
                opt.palette.setBrush(QPalette::Text, QBrush(QColor("blue")));
                break;
            case JsonTreeItem::Array:
                opt.palette.setBrush(QPalette::Text, QBrush(QColor("red")));
                break;
            case JsonTreeItem::Null:
                opt.palette.setBrush(QPalette::Text, QBrush(QColor("grey")));
                break;
            case JsonTreeItem::Bool:
                opt.palette.setBrush(QPalette::Text, QBrush(QColor("lightcyan")));
                break;
            case JsonTreeItem::Number:
                opt.palette.setBrush(QPalette::Text, QBrush(QColor(ConfigIni::getInstance().number(theme))));
                break;
            case JsonTreeItem::String:
                opt.palette.setBrush(QPalette::Text, QBrush(QColor(ConfigIni::getInstance().string(theme))));
                break;
            default:
                break;
            }
        }
    }
    else if (index.column() == 0)
    {
        opt.palette.setBrush(QPalette::Text, QBrush(QColor(ConfigIni::getInstance().propertyName(theme))));
    }

    if (item && item->getFind() > 0)
    {
        if ((index.column() == 0 && ((item->getFind() & 0x0001) > 0)) || (index.column() == 1 && ((item->getFind() & 0x0010) > 0)))
        {
            painter->save();
            painter->setRenderHint(QPainter::Antialiasing, true);
            QLinearGradient ling(option.rect.x() + 1, option.rect.bottom() - 1, option.rect.right() - 1, option.rect.bottom() - 1);
            ling.setColorAt(0, Qt::green);
            ling.setColorAt(1, Qt::blue);
            QBrush bush(ling);
            //painter->fillRect(option.rect, QColor(255, 0, 0));
            painter->setBrush(bush);
            painter->drawRect(option.rect.x() + 1, option.rect.bottom() - 1, option.rect.right() - option.rect.x() - 2, 1);
            painter->restore();
        }
    }

    const QWidget* widget = option.widget;
    QStyle* style = widget ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
}

QSize JsonItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return QStyledItemDelegate::sizeHint(option, index);
    }
    QSize s = QStyledItemDelegate::sizeHint(option, index);
    s.setHeight(s.height() + 4);
    return s;
}

QWidget* JsonItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }

    QString text = index.model()->data(index, Qt::EditRole).toString();
    if (text.isEmpty())
    {
        return nullptr;
    }

    if ((index.column() == 0 && text.length() <= 30) || (index.column() == 1 && text.length() <= 64))
    {
        QLineEdit* edit = new QLineEdit(parent);
        edit->setFixedHeight(edit->sizeHint().height() + 1);
        return edit;
    }

    auto* edit = new JsonPlainTextEdit(parent);
    QPalette pl = parent->palette();
    edit->setStyleSheet(tr("background-color:%1").arg(pl.base().color().name()));
    edit->setReadOnly(true);
    edit->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    return edit;
}

void JsonItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    // readonly
}

