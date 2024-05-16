#include "jsonPlainTextEdit.h"
#include "..\common\Logger.h"

#include <QTextBlock>
#include <QMenu>

JsonPlainTextEdit::JsonPlainTextEdit(QWidget* parent)
    : QPlainTextEdit(parent)
    , _height(-1)
{

}

JsonPlainTextEdit::JsonPlainTextEdit(const QString& text, QWidget* parent)
    : QPlainTextEdit(text, parent)
    , _height(-1)
{

}

JsonPlainTextEdit::~JsonPlainTextEdit()
{
    this->deleteLater();
}

void JsonPlainTextEdit::paintEvent(QPaintEvent* e)
{
    if (_height == -1)
    {
        QRectF r = blockBoundingRect(firstVisibleBlock());
        QFontMetricsF metric = fontMetrics();
        _height = r.height() + this->contentsMargins().top() + this->contentsMargins().bottom() + metric.lineSpacing();

        LOG_INFO << " paintEvent height: " << _height << "e height: " << e->rect().height();
        QSize s = this->size();
        s.setHeight(_height);
        this->resize(s);
        return this->selectAll();
    }
    LOG_INFO << " paintEvent height: " << _height << "e height111: " << e->rect().height() << " viewport height: " << viewport()->rect().height();
    QPlainTextEdit::paintEvent(e);
}

void JsonPlainTextEdit::mousePressEvent(QMouseEvent* e)
{
    if (e->type() == QEvent::MouseButtonPress && e->button() == Qt::RightButton)
    {
        QMenu* menu = this->createStandardContextMenu();
        return menu->popup(cursor().pos());
    }
    QPlainTextEdit::mousePressEvent(e);
}
