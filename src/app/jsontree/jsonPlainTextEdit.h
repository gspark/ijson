#pragma once
#ifndef jsonPlainTextEdit_h__
#define jsonPlainTextEdit_h__

#include <QPlainTextEdit>

class JsonPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit JsonPlainTextEdit(QWidget* parent = nullptr);
    explicit JsonPlainTextEdit(const QString& text, QWidget* parent = nullptr);
    ~JsonPlainTextEdit() override;

protected:
    void paintEvent(QPaintEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
private:
    int _height;
};

#endif // jsonPlainTextEdit_h__
