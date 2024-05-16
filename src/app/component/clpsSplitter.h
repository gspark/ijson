#ifndef collapsibleSplitter_h__
#define collapsibleSplitter_h__
#pragma once

#include <QSplitter>

class ClpsSplitter : public QSplitter
{
    Q_OBJECT
public:
    explicit ClpsSplitter(QWidget* parent = nullptr);
    explicit ClpsSplitter(Qt::Orientation orientation, QWidget* parent = nullptr);
    ~ClpsSplitter();

protected:
    QSplitterHandle* createHandle() override;

private:
    
};

class ClpsSplitterHandle : public QSplitterHandle
{
    Q_OBJECT
public:
    explicit ClpsSplitterHandle(Qt::Orientation o, QSplitter* parent);
    ~ClpsSplitterHandle() = default;
protected:
    void paintEvent(QPaintEvent*) override;

    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
private:
    bool collapsed;
    QByteArray state;
    bool button_contains(const QPoint& p, bool proper = false) const noexcept;
};


#endif // collapsibleSplitter_h__
