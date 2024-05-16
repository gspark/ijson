//
// Created by shrill on 2021-04-20.
//

#include "config.h"

#include <QSettings>
#include <QFont>
#include <QFontDatabase>

ConfigIni::ConfigIni() : pathFile("ijson.ini"), settings(new QSettings(pathFile, QSettings::IniFormat)) {
//    Qt5
//    settings->setIniCodec("UTF-8");
}

ConfigIni::~ConfigIni() {
    if (nullptr != settings) {
        // 销毁前同步
        settings->sync();
        settings->deleteLater();
        settings = nullptr;
    }
}

ConfigIni &ConfigIni::getInstance() {
//    1. 变量在代码第一次执行到变量声明的地方时初始化。
//    2. 初始化过程中发生异常的话视为未完成初始化，未完成初始化的话，需要下次有代码执行到相同位置时再次初始化。
//    3. 在当前线程执行到需要初始化变量的地方时，如果有其他线程正在初始化该变量，则阻塞当前线程，直到初始化完成为止。
//    4. 如果初始化过程中发生了对初始化的递归调用，则视为未定义行为。

    static ConfigIni instance;
    return instance;
}

void ConfigIni::setPathFile(const QString &path_file) {
    if (path_file != this->pathFile) {
        this->pathFile = path_file;
        // 及时同步之前的
        settings->sync();
        // 改变路径
        QSettings::setPath(settings->format(), settings->scope(), path_file);
//        Qt5
//        settings->setIniCodec("UTF-8");
    }
}

void ConfigIni::iniWrite(const QString &key, const QVariant &value) {
    settings->setValue(key, value);
}

bool ConfigIni::iniContains(const QString &key) {
    return settings->contains(key);
}

QVariant ConfigIni::iniRead(const QString &key, const QVariant &defaultValue) const
{
    if (!settings->contains(key)) {
        // 不存在
        // 使用默认值创建
        settings->setValue(key, defaultValue);
        return defaultValue;
    }
    return settings->value(key);
}

QFont ConfigIni::font(const QString& key) const
{
    QString family;
    if (settings->contains(key)) {
        family = settings->value(key).toString();
    }
    else {
#ifdef Q_OS_WIN
        QFontDatabase fontDb;
        if (fontDb.families(QFontDatabase::Any).contains("Consolas")) {
            family = "Consolas";
        }
        else {
            family = "Courier New";
        }
#elif Q_OS_MAC
        QFontDatabase fontDb;
        if (fontDb.families(QFontDatabase::Any).contains("Menlo")) {
            family = "Menlo";
        }
        else {
            family = "Monaco";
        }
#else
        family = "Monospace";
#endif
        settings->setValue(key, family);
    }
    int size = 10;
    int p = key.lastIndexOf("Name");
    if (p > 0)
    {
        size = fontSize(key.mid(0, p) + "Size");
    }

    return QFont(family, size);
}

int ConfigIni::fontSize(const QString& key) const
{
    if (settings->contains(key)) {
        return settings->value(key).toInt();
    }
    else {
        settings->setValue(key, 10);
        return 10;
    }
}

QString ConfigIni::foreground(const QString& theme) const
{
    return iniRead(QStringLiteral("Colors%1/foreground").arg(theme), theme.isEmpty() ? "#000000" : "#F8F8F2").toString();
}

QString ConfigIni::background(const QString& theme) const
{
    return iniRead(QStringLiteral("Colors%1/background").arg(theme), theme.isEmpty() ? "#FFFFFF" : "#272822").toString();
}

QString ConfigIni::caret(const QString& theme) const
{
    return iniRead(QStringLiteral("Colors%1/caret").arg(theme), theme.isEmpty() ? "#000000" : "#F8F8F0").toString();
}

QString ConfigIni::caretLine(const QString& theme) const
{
    return iniRead(QStringLiteral("Colors%1/caretLine").arg(theme), theme.isEmpty() ? "#FFFFC0" : "#49483E").toString();
}

QString ConfigIni::selection(const QString& theme) const
{
    return iniRead(QStringLiteral("Colors%1/selection").arg(theme), theme.isEmpty() ? "#80C0FF" : "#49483E").toString();
}

QString ConfigIni::whitespace(const QString& theme) const
{
    return iniRead(QStringLiteral("Colors%1/whitespace").arg(theme), theme.isEmpty() ? "#C0C0C0" : "#75715E").toString();
}

QString ConfigIni::number(const QString& theme) const
{
    return iniRead(QStringLiteral("Colors%1/Number").arg(theme), theme.isEmpty() ? "#8080FF" : "#AE81FF").toString();
}

QString ConfigIni::string(const QString& theme) const
{
    return iniRead(QStringLiteral("Colors%1/String").arg(theme), theme.isEmpty() ? "#F08080" : "#E6DB74").toString();
}

QString ConfigIni::propertyName(const QString& theme) const
{
    return iniRead(QStringLiteral("Colors%1/PropertyName").arg(theme), theme.isEmpty() ? "#800080" : "#F92672").toString();
}

QString ConfigIni::keyword(const QString& theme) const
{
    return iniRead(QStringLiteral("Colors%1/Keyword").arg(theme), theme.isEmpty() ? "#000080" : "#F92672").toString();
}
