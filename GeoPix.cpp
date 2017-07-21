#include "GeoPix.h"

GeoPix::GeoPix(QPointF world, QPixmap pix) :
    Geometry(UM::iGeoPix, pix.width()>pix.height()?pix.width():pix.height())
{
    background = pix;
    list.append(world);
    checkRect();
}

QRectF GeoPix::boundingRect() const
{
    return background.rect();
}

QPainterPath GeoPix::shape() const
{
    QPainterPath path;
    path.addRect(background.rect());
    return path;
}

void GeoPix::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->drawPixmap(0,0,background);
}
