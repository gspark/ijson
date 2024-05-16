#ifndef ICONDB_H
#define ICONDB_H

#include <QIcon>
#include <QMap>


enum class SplitterSize : int {
    h_width = 8, h_height = 26
};

/**
 * A class that holds all the application icons.
 */
class IconDb {
public:
    /**
     * Enumeration of all the application icons.
     */
    enum Icon {
        app,
        Array, Object, Null, Bool, String, Number,
        h_left, h_right, v_down, v_up,
        newTab, format, compress, clear, find,
        search, casesensitively, wholewords, regexp, prev, next, close
    };

    enum Theme {
        light, dark
    };

    /**
     * Returns an instance of the icon database.
     *
     * @return An instance of the icon database.
     */
    static IconDb* instance();

    /**
     * Returns an icon.
     *
     * @param icon The icon to get.
     * @return The icon.
     */
    QIcon getIcon(Icon ico, Theme theme = light);

private:
    /**
     * Private constructor to prevent instantiation.
     */
    IconDb() = default;

    /**
     * Private copy constructor to prevent instantiation.
     */
    IconDb(const IconDb&);

    /**
     * Private equals operator to prevent instantiation.
     */
    IconDb& operator=(const IconDb&);

    /** The map that stores the various icons. */
    QMap<Icon, QIcon> iconMap;

    QIcon loadIcon(Icon ico, Theme theme);
};

#endif // ICONDB_H
