#include "ILoveChina.h"

ILoveChina::ILoveChina()
{

}

QPointF ILoveChina::wgs84TOgcj02(QPointF wgs)
{
    if (outOfChina(wgs.x(), wgs.y()))
        return QPointF(wgs.x(), wgs.y());
    QPointF d = delta(wgs.x(), wgs.y());
    return QPointF(wgs.x() + d.x(), wgs.y() + d.y());
}

QPointF ILoveChina::gcj02Towgs84(QPointF gcj)
{
    if (outOfChina(gcj.x(), gcj.y()))
        return  QPointF(gcj.x(), gcj.y());
    QPointF d = delta(gcj.x(), gcj.y());
    return  QPointF(gcj.x() - d.x(),gcj.y() - d.y());
}

bool ILoveChina::DelDir(const QString &path)
{
    if (path.isEmpty()){
        return false;
    }
    QDir dir(path);
    if(!dir.exists()){
        return true;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
    QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
    foreach (QFileInfo file, fileList){ //遍历文件信息
        if (file.isFile()){ // 是文件，删除
            file.dir().remove(file.fileName());
        }else{ // 递归删除
            DelDir(file.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath()); // 删除文件夹
}

bool ILoveChina::outOfChina(double lon, double lat)
{
    if (lon < 72.004 || lon > 137.8347)
        return true;
    if (lat < 0.8293 || lat > 55.8271)
        return true;
    return false;
}

double ILoveChina::transformLat(double x, double y)
{
    double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * sqrt(fabs(x));
    ret += (20.0 * sin(6.0 * x * PI) + 20.0 * sin(2.0 * x * PI)) * 2.0 / 3.0;
    ret += (20.0 * sin(y * PI) + 40.0 * sin(y / 3.0 * PI)) * 2.0 / 3.0;
    ret += (160.0 * sin(y / 12.0 * PI) + 320 * sin(y * PI / 30.0)) * 2.0 / 3.0;
    return ret;
}

double ILoveChina::transformLon(double x, double y)
{
    double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(fabs(x));
    ret += (20.0 * sin(6.0 * x * PI) + 20.0 * sin(2.0 * x * PI)) * 2.0 / 3.0;
    ret += (20.0 * sin(x * PI) + 40.0 * sin(x / 3.0 * PI)) * 2.0 / 3.0;
    ret += (160.0 * sin(x / 12.0 * PI) + 300.0 * sin(x / 30.0 * PI)) * 2.0 / 3.0;
    return ret;
}

QPointF ILoveChina::delta(double lon, double lat)
{
    // Krasovsky 1940
    //
    // a = 6378245.0, 1/f = 298.3
    // b = a * (1 - f)
    // ee = (a^2 - b^2) / a^2;
    double a = 6378245.0; //  a: 卫星椭球坐标投影到平面地图坐标系的投影因子。
    double ee = 0.00669342162296594323; //  ee: 椭球的偏心率。
    double dLat = transformLat(lon - 105.0, lat - 35.0);
    double dLon = transformLon(lon - 105.0, lat - 35.0);
    double radLat = lat / 180.0 * PI;
    double magic = sin(radLat);
    magic = 1 - ee * magic * magic;
    double sqrtMagic = sqrt(magic);
    dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * PI);
    dLon = (dLon * 180.0) / (a / sqrtMagic * cos(radLat) * PI);
    return QPointF(dLon, dLat);
}
