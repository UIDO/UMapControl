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
#ifndef LAYER_H
#define LAYER_H

#include <QObject>
#include <QGraphicsItemGroup>

#include "UMapControl.h"
#include "SQLExcute.h"
#include "Geometry.h"

/*
 * 提供简单的图元管理功能
 * */
namespace UM {
class Layer;
}
class UMapControl;
class UMAPCONTROLSHARED_EXPORT Layer : public QObject
{
    Q_OBJECT
public:

    typedef struct
    {
        quint32 id;
        QPointF center;
        QString label;
        QList<QPointF> list;
        int size;
        int flags;
        QColor pen;
        QColor brush;
    } ILongInfo;
    /*
     * 新增图层
     * @name 图层名称
     * @typeList 图层数据结构
     * */
    explicit Layer(UMapControl * parent,QString name, QList<UM::Format> * typeList);
    /*
     * 从数据库里加载图层,所有参数都在数据库管理表里得到
     * */
    Layer(UMapControl * parent, QString id, QString name, bool visible, bool selectable);
    ~Layer();
    QSqlQuery * searchInfo(QString field, QString text);
    void setViewToItem(QString itemID);
    void addGeo(Geometry::DataType data);
    void addGeos(QList<Geometry::DataType> *dataList);
    QList<Geometry *> *getItems();
    void updatLayer(bool * isUpdate);
    void setLabel(QString field = "ILONGNULL");
    void updateGeoPenColor(quint32 geoID, QColor c = QColor(Qt::red));
    void updateGeoBrushColor(quint32 geoID, QColor c = QColor(Qt::yellow));
    void removeGeo(QString itemID);
    /*
     * 返回图层名称
     * */
    QString getLayerName();
    /*
     * 返回图层ID,和数据库里的表关联
     * */
    QString getLayerID();
    /*
     * 设置和返回图层可视状态
     * */
    QList<UM::Format> *getLayerHead();
    void setVisible(bool b);
    bool isVisible();
    /*
     * 设置和返回图层可选状态
     * */
    void setSelectable(bool b);
    bool isSelectable();
    QPointF getItemPosByID(QString itemID);
private:
    ILongInfo getInfo(QSqlQuery * query);
    QList<QPointF> getGisList(QString gis);
    UMapControl * uMap;
    QString layerLabel;
    /*
     * 保存当前图层的图元指针
     * */
    //QList<QGraphicsItem *> list;
    QString layerID;
    bool visible;
    bool selectable;
    SQLExcute * sqlExcute;
    /*
     * 保存当前图层的字段类型,只为了方便导入数据时数据转换检查
     * */
    QList <UM::Format> headType;
    QList <Geometry *> list;

signals:
    void addGeoToScene(Geometry *);
public slots:
};

#endif // LAYER_H
