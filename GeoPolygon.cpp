#include "GeoPolygon.h"

GeoPolygon::GeoPolygon(UMapControl *iL, QList<QPointF> * pointList, bool closePath, quint8 lineWidth, QColor pen, QColor brush) :
    Geometry(UM::iGeoPolygon,lineWidth, pen, brush),uMap(iL)
{
    closeFlag = closePath;
    for(int i=0; i<pointList->size(); i++)
        list.append(pointList->at(i));
    checkRect();
    QPointF tl = uMap->worldToScene(QPointF(rect.minX,rect.minY));
    QPointF br = uMap->worldToScene(QPointF(rect.maxX,rect.maxY));
    size = fabs(tl.x()-br.x());//polygonWidth.length();
    pHeight = fabs(tl.y()-br.y());//polygonHeight.length();
    QPointF telta = uMap->worldToScene(QPointF(rect.minX,rect.maxY));
    for(int i=0; i<list.size(); i++)
        polygon.append(uMap->worldToScene(list.at(i))-telta);
}

QRectF GeoPolygon::boundingRect() const
{
    return QRectF(0, 0, size, pHeight);
}

QPainterPath GeoPolygon::shape() const
{
    QPainterPath path;
    if(closeFlag)
        path.addPolygon(polygon);
    return path;
}

void GeoPolygon::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QPen xPen(QBrush(pen),lineWidth);
    painter->setPen(xPen);
    painter->setRenderHint(QPainter::Antialiasing);
    if(closeFlag)
        painter->setBrush(brush);
    QPainterPath path;
    path.addPolygon(polygon);
    if(closeFlag)
        path.closeSubpath();
    painter->drawPath(path);
    //多段线就不应该显示标注
    if((label.length() && closeFlag) ||
            (label.length() && polygon.size() == 2 && !closeFlag))
    {
        QFont font = painter->font();
        font.setFamily("Microsoft YaHei");
        font.setBold(true);
        painter->setFont(font);
        painter->setPen(pen);
        painter->drawText((size-getLabelPixeSize())/2,pHeight/2,label);

    }
}
