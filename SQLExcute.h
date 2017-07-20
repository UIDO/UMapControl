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

#ifndef SQLEXCUTE_H
#define SQLEXCUTE_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>
#include <QDebug>

#include "umapcontrol_global.h"
#include "Geometry.h"

/*
 * 第一次面向对象,也是第一次使用c++,所以呢,很多不会的了,反正现在是把所有数据库操作的都放到这个类里了!
 * 有用没用都先这么活着,总比不活强多了!~_~,发现我没事我还以自己乐了!
 */
namespace UM {
class SQLExcute;
}
class UMAPCONTROLSHARED_EXPORT SQLExcute : public QObject
{
    Q_OBJECT
public:
    explicit SQLExcute(QObject *parent = 0);
    void addItems(QList<Geometry::DataType> *dataList,
                  QString id, QList<UM::Format> *headType);
    void removeItem(QString layerID, QString itemID);
    /*
     * 获取当前场景范围内的所有瓦片并返回QSqlQuery,用完了需要自己删除指针
     */
    QSqlQuery * checkImage(int maxX, int minX, int maxY, int minY, int z);
    /*
     * 把已经下载的瓦片插入数据库中
     */
    void insertImage(int x, int y, int z, QByteArray ax);
    /*
     * 创建图层管理
     * 图层表里就只有包含图层的ID,NAME,VISIBLE(图层可视),SELECTABLE(图层可选)
     */
    QSqlQuery * initLayerManager();
    /*
     * 读取id这个数据表的字段类型,并返回QSqlQuery,这个读取字段类型就是为了,如果数据库里已经有表了,那就得把表的字段类型读取出来,
     * 保存到自己的图层里,方便插入图元数据的时候判断数据类型而已,没想到更好的办法,用完了需要自己删除指针
     */
    QSqlQuery *checkType(QString id);
    QSqlQuery *updateLayer(QString id, QPointF topLeft, QPointF rigthBottom, quint32 limit = 1000);
    /*
     * 创建图层,@id是图层的id,用来做数据库里的表名,在class Layer里自动生成,
     * @name 图层名称,创建图层时需要自己指定一个名称,
     * @typeList 就是表的数据结构了,有字段名和字段类型
     * @headType 就是在图层里用来保存表里所有字段的数据类型 只为了插入图元数据的字段类型选择,为了安全,还是每个数据都做个转换,
     *           如果转换失败,基本都是文本转数字失败,就填0,可能影响效率.
     */
    void initLayer(QString id, QString name, QList<UM::Format> *typeList, QList<UM::Format> *headType);
    QSqlQuery * getItemInfo(QString itemLayerID, QString itemID);
    QSqlQuery * searchInfo(QString itemLayerID, QString field, UM::DataType fieldType,QString text);
    QSqlQuery * setViewToItem(QString layerID,QString itemID);
    QSqlQuery * getDefaultLoaction();
    void updateGeoColor(QString layerId, quint32 geoID, QString field, QColor color);
    void updateDefaultLoaction(QPointF world = DEFAULTLOCATION, quint8 level = DEFAULTZOOMLEVEL);
    void updateItemLimit(quint32 limit);
    QString dbPath();
    QSqlQuery * tilesCount();
    /*
     * 删除图层,@id可以通过图层获得
     */
    void removeLayer(QString id);
    void clearLayer(QString id);
    /*
     * 设置图层是否可视
     */
    void setLayerVisible(QString id,bool b);
    /*
     * 设置图层是否可选
     */
    void setLayerSelectable(QString id,bool b);
    void setLabel(QString id,QString field);
    QSqlQuery * getPosByItemID(QString layerID, QString itemID);
private:
    /*
     * 通用执行有返回结果的@sql语句
     * @position 自定义一个信息 如果出错就打印这个信息,还是感觉有点蛋疼,但是现在还没有使用log系统,选这样处理了
     */
    QSqlQuery *getResult(QString sql, QString position);
    /*
     * 通用执行有返回结果的@sql语句
     * @position 和getResult一样
     */
    void nonResult(QString sql, QString position);
signals:

public slots:
};

#endif // SQLEXCUTE_H
