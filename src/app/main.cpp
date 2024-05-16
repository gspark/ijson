#include "mainwindow.h"

#ifdef _DEBUG
#include "common/Logger.h"
#endif

#include <QApplication>
#include <QStyleFactory>
#include <QFile>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
#if _DEBUG
    Logger::initLog();
#endif

    //QFile qssFile(":/qdarkstyle/light/lightstyle.qss");
    //QFile qssFile(":/qdarkstyle/dark/darkstyle.qss");
    //qssFile.open(QFile::ReadOnly);
    //if (qssFile.isOpen())
    //{
    //    a.setStyleSheet(qssFile.readAll());
    //    qssFile.close();
    //}
    //// 设置兼容的样式
    //QApplication::setStyle(QStyleFactory::create("Fusion"));
    //// 创建并应用暗色调色板
    //QPalette darkPalette;
    //darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    //darkPalette.setColor(QPalette::WindowText, Qt::white);
    //darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    //darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    //darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    //darkPalette.setColor(QPalette::ToolTipText, QColor(162, 162, 162));
    //darkPalette.setColor(QPalette::Text, Qt::white);
    //darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    //darkPalette.setColor(QPalette::ButtonText, Qt::white);
    //darkPalette.setColor(QPalette::BrightText, Qt::red);
    //darkPalette.setColor(QPalette::Link, QColor(255, 183, 47));
    //darkPalette.setColor(QPalette::Highlight, QColor(255, 183, 47));
    //darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    //// 设置禁用状态下的窗口文本颜色
    //darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, Qt::gray);
    //// 设置禁用状态下的按钮文本颜色
    //darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::gray);
    //a.setPalette(darkPalette);
    MainWindow w;
    w.show();
    return a.exec();
}
