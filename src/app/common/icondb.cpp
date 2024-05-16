#include "icondb.h"
#include "magic_enum/magic_enum.hpp"


IconDb* IconDb::instance() {
    static IconDb instance;

    return &instance;
}

QIcon IconDb::getIcon(Icon ico, Theme theme/* = light*/) {
    if (iconMap.contains(ico))
    {
        return iconMap[ico];
    }
    return loadIcon(ico, theme);
}

QIcon IconDb::loadIcon(Icon ico, Theme theme)
{
    QIcon icon = QIcon();
    switch (ico)
    {
    case app:
        icon.addFile(":icons/icons/app.ico", QSize(16, 16));
        iconMap[app] = icon;
        break;
    case Array:
        icon.addFile(":icons/icons/json/array.png", QSize(16, 16));
        iconMap[Array] = icon;
        break;
    case Object:
        icon.addFile(":icons/icons/json/object.png", QSize(16, 16));
        iconMap[Object] = icon;
        break;
    case Null:
        icon.addFile(":icons/icons/json/j_null.png", QSize(16, 16));
        iconMap[Null] = icon;
        break;
    case Bool:
        icon.addFile(":icons/icons/json/j_bool.png", QSize(16, 16));
        iconMap[Bool] = icon;
        break;
    case String:
        icon.addFile(":icons/icons/json/string.png", QSize(16, 16));
        iconMap[String] = icon;
        break;
    case Number:
        icon.addFile(":icons/icons/json/number.png", QSize(16, 16));
        iconMap[Number] = icon;
        break;
    case h_left:
        icon.addFile(":icons/icons/splitter/h_left.png",QSize(
            static_cast<std::underlying_type<SplitterSize>::type>(SplitterSize::h_width), 
            static_cast<std::underlying_type<SplitterSize>::type>(SplitterSize::h_height)
        ));
        iconMap[h_left] = icon;
        break;
    case h_right:
        icon.addFile(":icons/icons/splitter/h_right.png", QSize(
            static_cast<std::underlying_type<SplitterSize>::type>(SplitterSize::h_width),
            static_cast<std::underlying_type<SplitterSize>::type>(SplitterSize::h_height)
        ));
        iconMap[h_right] = icon;
        break;
    case v_down:
        icon.addFile(":icons/icons/splitter/v_down.png", QSize(
            static_cast<std::underlying_type<SplitterSize>::type>(SplitterSize::h_height),
            static_cast<std::underlying_type<SplitterSize>::type>(SplitterSize::h_width)
        ));
        iconMap[v_down] = icon;
        break;
    case v_up:
        icon.addFile(":icons/icons/splitter/v_up.png", QSize(
            static_cast<std::underlying_type<SplitterSize>::type>(SplitterSize::h_height),
            static_cast<std::underlying_type<SplitterSize>::type>(SplitterSize::h_width)
        ));
        iconMap[v_up] = icon;
        break;
    case newTab:
        icon.addFile(":icons/icons/tool/newTab.png", QSize(16, 16));
        iconMap[newTab] = icon;
        break;
    case format:
        icon.addFile(":icons/icons/tool/format.png", QSize(16, 16));
        iconMap[format] = icon;
        break;
    case compress:
        icon.addFile(":icons/icons/tool/compress.png", QSize(16, 16));
        iconMap[compress] = icon;
        break;
    case clear:
        icon.addFile(":icons/icons/tool/clear.png", QSize(16, 16));
        iconMap[clear] = icon;
        break;
    case find:
        icon.addFile(":icons/icons/tool/find.png", QSize(16, 16));
        iconMap[find] = icon;
        break;
    default:
        QString path = ":icons/icons/";
        auto name = magic_enum::enum_name(ico);
        icon.addFile(path.append(name) + ".png", QSize(16, 16));
        iconMap[ico] = icon;
        break;
    }
    return icon;
}
