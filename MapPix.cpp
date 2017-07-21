#include "MapPix.h"

MapPix::MapPix(QPixmap pix)
{
    background = pix;
}

QRectF MapPix::boundingRect() const
{
    return background.rect();
}

QPainterPath MapPix::shape() const
{
    QPainterPath path;
    path.addRect(background.rect());
    return path;
}

void MapPix::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->drawPixmap(0,0,background);
}
