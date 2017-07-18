#include "GeoRect.h"

GeoRect::GeoRect(QPointF world, int size, QColor pen, QColor brush) :
  Geometry(UM::iGeoRect,  size*0.6, pen, brush)
{
    list.append(world);
    checkRect();
}

QRectF GeoRect::boundingRect() const
{
    return QRectF(-size/2, -size/2, size, size);
}

QPainterPath GeoRect::shape() const
{
    QPainterPath path;
    path.addRect(-size/2, -size/2, size, size);
    return path;
}

void GeoRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawRect(-size/2,-size/2,size,size);
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
