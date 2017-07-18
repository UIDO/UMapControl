#include "Map.h"

Map::Map(QObject *parent) : QObject(parent)
{
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    server = "www.google.cn";
    path = "/maps/vt?lyrs=s@701,r@701&gl=cn&x=%2&y=%3&z=%1";
    param1 = path.indexOf("%1");
    param2 = path.indexOf("%2");
    param3 = path.indexOf("%3");
    int min = param1 < param2 ? param1 : param2;
    min = param3 < min ? param3 : min;
    int max = param1 > param2 ? param1 : param2;
    max = param3 > max ? param3 : max;
    int middle = param1+param2+param3-min-max;
    order[0][0] = min;
    if (min == param1)
        order[0][1] = 0;
    else if (min == param2)
        order[0][1] = 1;
    else
        order[0][1] = 2;
    order[1][0] = middle;
    if (middle == param1)
        order[1][1] = 0;
    else if (middle == param2)
        order[1][1] = 1;
    else
        order[1][1] = 2;
    order[2][0] = max;
    if (max == param1)
        order[2][1] = 0;
    else if(max == param2)
        order[2][1] = 1;
    else
        order[2][1] = 2;
}

QString Map::getServer()
{
    return server;
}

QString Map::getPath()
{
    return path;
}

QString Map::queryTile(int x, int y, int z)
{
    int a[3] = {z, x, y};
    return QString(getPath().replace(order[2][0],2, loc.toString(a[order[2][1]]))
            .replace(order[1][0],2, loc.toString(a[order[1][1]]))
            .replace(order[0][0],2, loc.toString(a[order[0][1]])));
}

bool Map::isTileValid(int x, int y, int z)
{
    return ( (x<0 || x > (1 << z)-1 || y<0 || y > (1 << z)-1) ? false : true);
}
