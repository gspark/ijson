#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "wxbar/wxwindow.h"

class QLabel;

class MainWindow : public WxWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    QTabWidget* tabWidget;

    QLabel* message;
    QLabel* docInfo;
    QLabel* position;

    //初始化状态栏
    void initStatusBar();

    void setupMenuBar();
    void setupWidgets();

    void loadWindowInfo();
    void saveWindowInfo();

    void setupToolBar();

private slots:
    void openFile();
    void about();

    void openNewTab();
    void formatDoc();
    void compressJson();
    void clearDoc();
    void searchForDoc();

    void onMessage(const QString message);

    void cursorPositionChanged(intptr_t line, int column);

    void tabCloseRequested(int index);

    void textLengthChanged(intptr_t length, intptr_t lines);
};
#endif // MAINWINDOW_H
