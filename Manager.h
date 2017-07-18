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
#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QList>

#include "UMapControl.h"
#include "Layer.h"
#include "SQLExcute.h"

/*
 * 提供简单的图层管理功能
 * */
namespace UM {
class Manager;
}
class UMapControl;
class Layer;
class UMAPCONTROLSHARED_EXPORT Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(UMapControl *iL, QObject * parent = 0);
    /*
     * 返回所有图层的指针
     * */
    QList<Layer *> getLayers();
    /*
     * 新增图层,名称@name
     * @typeList图层数据结构
     * */
    Layer *addLayer(QString name, QList<UM::Format> *typeList);
    /*
     * 通过图层名称@name直接删除图层
     * */
    Layer *getLayer(QString name);
    Layer *getLayerByID(QString id);
    void removeLayer(QString name);
    void stopUpdateLayer();
    void addTempItem(QPointF world, UM::GeoType type = UM::iGeoCircle);
    bool moveLayer(QString name, bool up = true);
private:
    /*
     * 检查图层名称@name是否在图层管理表里,如果有,就自动在@name后面加*号,暂时这样处理导入多个同名图层
     * */
    QString checkLayerName(QString name);
    /*
     * 从管理表里加载数据库里的图层,所有参数都在管理表里
     * */
    void loadLayer(QString id, QString name, bool visible, bool selectable);
    UMapControl * uMap;
    /*
     * 所有图层列表,不用每次去图层管理表读取图层信息
     * */
    QList<Layer *> list;
    SQLExcute * sqlExcute;
    bool isUpdate;

    QPointF tempGeoWorldPos;
    UM::GeoType tempGeoType;
signals:
    void addGeoToScene(Geometry *);
public slots:
    void updatLayer();
};

#endif // MANAGER_H
