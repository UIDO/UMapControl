#ifndef MAPPIX_H
#define MAPPIX_H

#include <QObject>
#include <QGraphicsObject>
#include <QPainter>
#include "umapcontrol_global.h"

namespace UM {
class MapPix;
}
class UMAPCONTROLSHARED_EXPORT MapPix : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit MapPix(QPixmap pix);
protected:
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) Q_DECL_OVERRIDE;
signals:

public slots:
private:
    QPixmap background;
};

#endif // MAPPIX_H
