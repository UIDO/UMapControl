#include "Geometry.h"

Geometry::Geometry(UM::GeoType gType, quint8 lWidth, QColor iPen, QColor iBrush)
{
    geoType = gType;
    if(gType == UM::iGeoPolygon)
        lineWidth = lWidth;
    else
        size = lWidth;
    pen = iPen;
    brush = iBrush;
    QUuid id = QUuid::createUuid();
    itemID = id.data1;
    label = "";
    dir = 0;
    closeFlag = false;
    rect.maxX = 0;
    rect.maxY = 0;
    rect.minX = 0;
    rect.minY = 0;
}

UM::UmapGeoRect Geometry::getRect()
{
    return rect;
}

UM::GeoType Geometry::getGeoType()
{
    return geoType;
}

QPointF Geometry::getCenter()
{
    return QPointF((rect.maxX + rect.minX)/2,(rect.maxY + rect.minY)/2);
}

QString Geometry::getPen()
{
    return QString("%1_%2_%3").arg(pen.red()).arg(pen.green()).arg(pen.blue());
}

QString Geometry::getBrush()
{
    return QString("%1_%2_%3").arg(brush.red()).arg(brush.green()).arg(brush.blue());
}

QString Geometry::getPoints()
{
    QString result = "";
    for(int i=0; i<list.size(); i++)
    {
        QPointF p = list.at(i);
        result += QString("%1,%2_").arg(QString::number(p.x(),'g',10))
                .arg(QString::number(p.y(),'g',10));
    }
    return result.left(result.length()-1);
}

quint8 Geometry::getLineWidth()
{
    return lineWidth;
}

quint8 Geometry::getSize()
{
    return size;
}

quint32 Geometry::getID()
{
    return itemID;
}

int Geometry::getDir()
{
    return dir;
}

bool Geometry::getCloseFlag()
{
    return closeFlag;
}

void Geometry::rotate(int dir)
{
    setTransform(QTransform().rotate(dir), true);
}

void Geometry::setLabel(QString lb)
{
    label = lb;
}

QString Geometry::getLabel()
{
    return label;
}

int Geometry::getLabelPixeSize()
{
    QLabel lb;
    return lb.fontMetrics().width(label);
}

void Geometry::checkRect()
{
    if(list.size() == 0)
    {
        list.append(QPointF(0,0));
        list.append(QPointF(0,0));
    }
    if(list.size() == 1)
    {
        if(geoType != UM::iGeoPolygon)
        {
            rect.minX = list.at(0).x();
            rect.minY = list.at(0).y();
            rect.maxX = list.at(0).x();
            rect.maxY = list.at(0).y();
            return;
        }
        list.append(list.at(0));
    }
    QPointF p1 = list.at(0);
    rect.minX = p1.x();
    rect.minY = p1.y();
    rect.maxX = p1.x();
    rect.maxY = p1.y();

    for(int i=1; i<list.size(); i++)
    {
        p1 = list.at(i);
        if(p1.x() >= rect.maxX) rect.maxX = p1.x();
        if(p1.x() <= rect.minX) rect.minX = p1.x();
        if(p1.y() >= rect.maxY) rect.maxY = p1.y();
        if(p1.y() <= rect.minY) rect.minY = p1.y();
    }
}
