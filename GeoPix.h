#ifndef GEOPIX_H
#define GEOPIX_H

#include <QObject>
#include "Geometry.h"
namespace UM {
class GeoPix;
}
class UMAPCONTROLSHARED_EXPORT GeoPix : public Geometry
{
    Q_OBJECT
public:
    explicit GeoPix(QPointF world,QPixmap pix);
protected:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) Q_DECL_OVERRIDE;
signals:

public slots:
private:
    QPixmap background;
};

#endif // GEOPIX_H
