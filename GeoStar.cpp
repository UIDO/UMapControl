#include "GeoStar.h"

GeoStar::GeoStar(QPointF world, int size, QColor pen, QColor brush) :
    Geometry(UM::iGeoStar, size*0.8, pen, brush)
{
    list.append(world);
    checkRect();
}

QRectF GeoStar::boundingRect() const
{
    return QRectF(-size/2, -size/2, size, size);
}

QPainterPath GeoStar::shape() const
{
    QPainterPath path;
    QPolygonF polygon;
    polygon.append(QPointF(0,-size/2));polygon.append(QPointF(-size/8,-size/8));
    polygon.append(QPointF(-size/2,0));polygon.append(QPointF(-size/8,size/8));
    polygon.append(QPointF(0,size/2));polygon.append(QPointF(size/8,size/8));
    polygon.append(QPointF(size/2,0));polygon.append(QPointF(size/8,-size/8));
    path.addPolygon(polygon);
    path.closeSubpath();
    return path;
}

void GeoStar::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->setRenderHint(QPainter::Antialiasing);
    QPolygonF polygon;
    polygon.append(QPointF(0,-size/2));polygon.append(QPointF(-size/8,-size/8));
    polygon.append(QPointF(-size/2,0));polygon.append(QPointF(-size/8,size/8));
    polygon.append(QPointF(0,size/2));polygon.append(QPointF(size/8,size/8));
    polygon.append(QPointF(size/2,0));polygon.append(QPointF(size/8,-size/8));
    painter->drawPolygon(polygon);
    if(label.length())
    {
        QFont font = painter->font();
        font.setFamily("Microsoft YaHei");
        font.setBold(true);
        painter->setFont(font);
        painter->setPen(brush);
        painter->drawText(-getLabelPixeSize()/2,size,label);

    }
}
