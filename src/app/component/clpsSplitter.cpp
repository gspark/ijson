#include "clpsSplitter.h"
#include "../common/icondb.h"

#include <QStylePainter>
#include <QStyleOptionButton>
#include <QMouseEvent>

ClpsSplitter::ClpsSplitter(QWidget* parent /*= nullptr*/)
    : ClpsSplitter(Qt::Horizontal, parent)
{
}

ClpsSplitter::ClpsSplitter(Qt::Orientation orientation, QWidget* parent /*= nullptr*/)
    : QSplitter(orientation, parent)
{
    setHandleWidth(static_cast<std::underlying_type<SplitterSize>::type>(SplitterSize::h_width));
    setContentsMargins(0, 0, 0, 0);
}

ClpsSplitter::~ClpsSplitter()
{
}

QSplitterHandle* ClpsSplitter::createHandle()
{
    return new ClpsSplitterHandle(this->orientation(), this);
}

ClpsSplitterHandle::ClpsSplitterHandle(Qt::Orientation o, QSplitter* parent)
    : QSplitterHandle(o, parent)
    , collapsed(false)
    , state(nullptr)
{
    this->setMouseTracking(true);
}

void ClpsSplitterHandle::paintEvent(QPaintEvent* event)
{
    QStylePainter painter(this);
    QStyleOptionButton opt;

    opt.rect = QRect(0, contentsRect().height() / 2, this->width(), static_cast<std::underlying_type<SplitterSize>::type>(SplitterSize::h_height));
    
    if (orientation() == Qt::Horizontal)
        opt.state = QStyle::State_Horizontal;
    else
        opt.state = QStyle::State_None;

    opt.state |= QStyle::State_Active;

    if (isEnabled())
        opt.state |= QStyle::State_Enabled;

    opt.iconSize = QSize(this->width(), static_cast<std::underlying_type<SplitterSize>::type>(SplitterSize::h_height));

    if (orientation() == Qt::Horizontal)
    {
        if (collapsed) {
            opt.icon = IconDb::instance()->getIcon(IconDb::h_left);
        }
        else {
            opt.icon = IconDb::instance()->getIcon(IconDb::h_right);
        }
    }
    else
    {
        if (collapsed) {
            opt.icon = IconDb::instance()->getIcon(IconDb::v_up);
        }
        else {
            opt.icon = IconDb::instance()->getIcon(IconDb::v_down);
        }
    }
   
    painter.fillRect(event->rect(), Qt::lightGray);
    painter.drawControl(QStyle::CE_PushButton, opt);
}

void ClpsSplitterHandle::mouseMoveEvent(QMouseEvent* event)
{
    QRect btnRect(0, contentsRect().height() / 2, this->width(), static_cast<std::underlying_type<SplitterSize>::type>(SplitterSize::h_height));
    QMouseEvent* mevent = static_cast<QMouseEvent*>(event);
    if (btnRect.contains(mevent->pos()))
    {
        setCursor(Qt::PointingHandCursor);
        //QPainter painter(this);
        //QPen pen(Qt::red);
        //pen.setWidth(5);
        //painter.setPen(pen);
        //painter.drawRect(btnRect.x() - 1, btnRect.y() + 1, btnRect.width() + 1, btnRect.height() + 1);
        //painter.fillRect(btnRect, QColor(255, 0, 0));
    }
    else if (this->orientation() == Qt::Horizontal)
    {
        setCursor(Qt::SplitHCursor);
    }
    else {
        setCursor(Qt::SplitVCursor);
    }
    QSplitterHandle::mouseMoveEvent(event);
}

bool ClpsSplitterHandle::button_contains(const QPoint& p, bool proper /*= false*/) const noexcept
{
    QRect btnRect(0, contentsRect().height() / 2, this->width(), static_cast<std::underlying_type<SplitterSize>::type>(SplitterSize::h_height));
    return btnRect.contains(p);
}

void ClpsSplitterHandle::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && button_contains(event->pos())) {
        auto* parent = qobject_cast<QSplitter*>(this->parent());
        if (nullptr == parent)
        {
            return;
        }
        if (collapsed) {
            collapsed = false;
            parent->restoreState(state);
        }
        else {
            collapsed = true;
            state = parent->saveState();
            if (this->orientation() == Qt::Horizontal)
            {
                parent->setSizes({ parent->width() - parent->handleWidth(), 0 });
            } else {
                parent->setSizes({ parent->height() - parent->handleWidth(), 0 });
            }
        }
        update();
        emit parent->splitterMoved(0, 1);
    }
    QSplitterHandle::mouseReleaseEvent(event);
}

void ClpsSplitterHandle::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && !button_contains(event->pos())) {
        auto* parent = qobject_cast<QSplitter*>(this->parent());
        parent->setSizes({ 1, 1 });
        update();
        emit parent->splitterMoved(0, 1);
    }
    QSplitterHandle::mouseDoubleClickEvent(event);
}
