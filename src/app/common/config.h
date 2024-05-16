//
// Created by shrill on 2021-04-20.
//

#ifndef CRYPTOOL_CONFIG_H
#define CRYPTOOL_CONFIG_H

#include <QString>
#include <QVariant>

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE


class ConfigIni {
private:
    ConfigIni();

    ~ConfigIni();

    ConfigIni(const ConfigIni &);

    ConfigIni& operator=(const ConfigIni&);

    // ini文件路径
    QString pathFile;
    QSettings *settings;

    int fontSize(const QString& key) const;
public:
    static ConfigIni &getInstance();

    // 重设config.ini文件路径
    void setPathFile(const QString &path_file);

    // 检测配置存在
    bool iniContains(const QString &key);

    QVariant iniRead(const QString &key, const QVariant &defaultValue = QVariant()) const;

    // 写配置文件
    void iniWrite(const QString &key, const QVariant &value);

    /**
     * Returns the font that should be used in the editor.
     *
     * @return The font that should be used in the editor.
     */
    QFont font(const QString& key) const;

    QString foreground(const QString& theme) const;

    QString background(const QString& theme) const;

    QString caret(const QString& theme) const;

    QString caretLine(const QString& theme) const;

    QString selection(const QString& theme) const;

    QString whitespace(const QString& theme) const;

    QString number(const QString& theme) const;

    QString string(const QString& theme) const;

    QString propertyName(const QString& theme) const;

    QString keyword(const QString& theme) const;

};

#endif //CRYPTOOL_CONFIG_H
