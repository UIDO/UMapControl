#include "GeoTri.h"

GeoTri::GeoTri(QPointF world, int size, QColor pen, QColor brush) :
    Geometry(UM::iGeoTri, size*0.6, pen, brush)
{
    list.append(world);
    checkRect();
}

QRectF GeoTri::boundingRect() const
{
    return QRectF(-size/2, -size/2, size, size);
}

QPainterPath GeoTri::shape() const
{
    QPainterPath path;
    QPolygonF polygon;
    polygon.append(QPointF(0,-size/2));
    polygon.append(QPointF(-size/2,size/2));
    polygon.append(QPointF(size/2,size/2));
    path.addPolygon(polygon);
    return path;
}

void GeoTri::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->setRenderHint(QPainter::Antialiasing);
    QPolygonF polygon;
    polygon.append(QPointF(0,-size/2));
    polygon.append(QPointF(-size/2,size/2));
    polygon.append(QPointF(size/2,size/2));
    painter->drawPolygon(polygon);
    if(label.length())
    {
        QFont font = painter->font();
        font.setFamily("Microsoft YaHei");
        font.setBold(true);
        painter->setFont(font);
        painter->setPen(brush);
        painter->drawText(-getLabelPixeSize()/2,size+5,label);
    }

}
