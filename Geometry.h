/*
 * Copyright © 2017 UISDO All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce at minimum a disclaimer
 *    substantially similar to the "NO WARRANTY" disclaimer below
 *    ("Disclaimer") and any redistribution must be conditioned upon
 *    including a substantially similar Disclaimer requirement for further
 *    binary redistribution.
 * 3. Neither the names of the above-listed copyright holders nor the names
 *    of any contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 * NO WARRANTY
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 */

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QObject>
#include <QVariant>
#include <QGraphicsObject>
#include <QPainter>
#include <QLabel>
#include <QUuid>
#include <QDebug>

#include "umapcontrol_global.h"

/*
 * 图元基类,提供所有点类和面类的基本结构
 * */
namespace UM {
class Geometry;
}
class UMAPCONTROLSHARED_EXPORT Geometry : public QGraphicsObject
{
    Q_OBJECT
public:
    /*
     * 插入图元结构
     * */
    typedef struct
    {
        Geometry * geometry;    //图元
        QList<QVariant> data;   //一行数据放到data列表里
    } DataType;
    explicit Geometry(UM::GeoType gType,quint8 lWidth, QColor iPen = QColor(Qt::red), QColor iBrush=QColor(Qt::yellow));
    UM::UmapGeoRect getRect();
    UM::GeoType getGeoType();
    QPointF getCenter();
    QString getPen();
    QString getBrush();
    QString getPoints();
    quint8 getLineWidth();
    quint8 getSize();
    quint32 getID();
    int getDir();
    bool getCloseFlag();
    void rotate(int dir);
    void setLabel(QString lb);
    QString getLabel();
    int getLabelPixeSize();
    void setTempColor(QColor c);
protected:
    void checkRect();
    QList<QPointF> list;
    UM::GeoType geoType;
    QColor pen, brush;
    quint8 lineWidth;
    int size;
    UM::UmapGeoRect rect;
    quint32 itemID;
    QString label;
    int dir;
    bool closeFlag;
signals:

public slots:
};

#endif // GEOMETRY_H
