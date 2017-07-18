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

#ifndef UMAPCONTROL_H
#define UMAPCONTROL_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QWheelEvent>
#include <QGestureEvent>
#include <QMessageBox>
#include <QPinchGesture>
#include <QThread>
#include <QtMath>
#include <QList>
#include <QMutex>
#include <QDebug>
#include "umapcontrol_global.h"
#include "Map.h"
#include "Network.h"
#include "ILoveChina.h"
#include "SQLExcute.h"
#include "Manager.h"
#include "Layer.h"
#include "GeoRect.h"
#include "GeoPie.h"
#include "GeoCircle.h"
#include "GeoStar.h"
#include "GeoTri.h"
#include "GeoPolygon.h"

/*
 * 提供简单的跨平台的瓦片图层框架功能,反正不会C++,更不会面向对象,用来练activateWindow();手的!
 * */
namespace UM {
class UMapControl;
}
class Network;
class Manager;
class GeoPolygon;
class UMAPCONTROLSHARED_EXPORT UMapControl : public QGraphicsView
{
    Q_OBJECT
public:
    friend class Manager;
    friend class Layer;
    friend class Network;
    friend class ItemInfo;

    UMapControl(QWidget *parent=0);
    ~UMapControl();

    /*****************************************************************************
     *                                  API
     * ***************************************************************************/

    /*
     * 最大地图等级
     * */
    quint8 maxZoomLevel();
    /*
     * 最小地图等级
     * */
    quint8 minZoomLevel();
    /*
     * 当前地图等级
     * */
    quint8 zoomLevel();
    /*
     * 放大地图
     * */
    void zoomIn();
    /*
     * 缩小地图
     * */
    void zoomOut();
    /*
     * 设置默认的地图加载
     * @worldCoordinate 位置
     * @zoomLevel       地图等级
     * */
    void setDefaultLocation(QPointF worldCoordinate, quint8 zoomLevel);
    QPointF getDefaultLocation();
    /*
     * 返回图层
     * */
    QList<Layer *> getLayers() const;
    Layer * getlayer(QString name) const;
    Layer * getLayerByID(QString ID) const;
    /*
     * 新增图层
     * @name     图层名称
     * @typeList 图层数据结构
     * */
    Layer *addLayer(QString name, QList<UM::Format> *typeList) const;
    /*
     * 通过图层名称@name删除图层
     * */
    void removeLayer(QString name);
    void addTempGeo(QPointF world, UM::GeoType type = UM::iGeoCircle);
    /*
     * 世界坐标和场景坐标相与转换
     * */
    QPointF worldToScene(QPointF world);
    QPointF sceneToWorld(QPointF scene);
    /*
     * 设置或返回一个图层的最大图元个数@limit, 一个图层图元太多意义不大
     * */
    void setItemLimit(quint32 limit = DEFAULTITEMLIMITPERLAYER);
    quint32 getItemLimit();
    /*
     * 跳转到默认位置，可以通过setDefaultLocation设置位置，如果有GPS更新位置，默认位置为GPS当前位置
     * */
    void goToDefaultLocation();
    /*
     * 上下图层了，好像是可以用的，不过我现在用不到
     * */
    bool moveLayerTo(QString name, bool back = false);
    /*
     * 用来响应按键上下左右移动地图了。。。
     * */
    void setViewOffset(int deltaX = 10, int deltaY = -10);
    /*
     * 下载离线瓦片到数据库中，下载只会从当前地图等级开始下载，下到指定的地图等级
     * @dowloadMaxLevel >= 当前地图等级
     * */
    void DownloadTiles(quint8 dowloadMaxLevel = MAXZOOMLEVEL);
    /*
     * 取到数据库位置
     * */
    QString dbPath();
    /*
     * 查询瓦片数量
     * */
    int tilesSize();
    /*
     * 是不是GPS设备
     * */
    bool GPSUE();
    /*
     * 当前GPS位置
     * */
    QPointF currentGPS();
    /*
     * 提供从外部更新地图函数调用
     * */
    void updateMap();
protected:
    bool viewportEvent(QEvent *event);
    void drawBackground(QPainter *p, const QRectF &rect);
    void drawForeground(QPainter *painter, const QRectF &rect);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
private:
    /*
     * 角度和弧度相与转换,没上过高中,数学学得不好,真心不理解弧度,但不影响
     * */
    inline qreal degreeToRadian(qreal value);
    inline qreal radianToDegree(qreal value);
    /*
     * 计算当前地图等级@zoomLevel单行或单列共有多少张瓦片了
     * */
    int tilesOnZoomLevel(quint8 zoomLevel);
    /*
     * 计算当前场景内的所有瓦片,并在数据库里查找,如果没找到就下载,如果找到了直接打印到场景背景图片里
     * */
    void tilesUrlMatrix();
    /*
     * 缩放在指定的位置@world和指定的地图等级@zoomLevel
     * 因为默认是缩放在场景中心,所以加了个参数@underMouse限制缩放在鼠标下面
     * */
    void zoomTo(QPointF world, quint8 zoomLevel, bool underMouse = false);
    /*
     * 像拿着放大镜看地图一样在地图上移动场景,就当把场景当成是不能放大的放大镜就好
     * @topLeftPos 是场景的左上角位置
     * @updateItem 因为平移的时候不想刷新图元,刷新的话,压力比较大,所以设置这我参数
     * */
    void setSceneLocation(QPointF topLeftPos, bool updateItem = false);
    /*
     * 检查地图等级@zoomLevel是否有效
     * */
    inline bool checkZoomLevel(quint8 zoomLevel);
    /*
     * 检查世界坐标@world是否有效
     * */
    inline bool checkWorldCoordinate(QPointF world);
    /*
     * item缩放系数
     * */
    qreal itemScale;
    /*
     * 当前地图等级
     * */
    quint8 currentLevel;
    /*
     * 当前地图瓦片数量
     * */
    quint32 numberOfTiles;
    /*
     * 滚动鼠标缩放或鼠标平移地图时候 保存鼠标位置
     * */
    QPoint zoomOnPos;
    /*
     * 保存初始化位置的世界坐标
     * */
    QPointF defaultLocation;
    /*
     * 地图供应商
     * */
    Map map;
    /*
     * 场景背景图片
     * */
    QPixmap background;
    /*
     * 场景背景图片打印位置
     * */
    QPoint backgroundPos;
    /*
     * @middle  场景中心的那张瓦片的坐标
     * @leftTop 场景左上角的那张瓦片的坐标
     * 保存在这里是因为下载完瓦片后需要知道打印在哪
     * */
    QPoint middle, leftTop;
    /*
     * 管理网络服务
     * */
    Network * net;
    /*
     * 管理图层
     * */
    Manager * manager;
    /*
     * 瓦片path列表,下载的时候直接从列表里读取,刷新的时候直接保存在列表里
     * */
    QList <QString> list;
    /*
     * 下载瓦片线程
     * */
    QThread  networkThread;
    QThread  updateThread;
    /*
     * 数据库管理
     * */
    SQLExcute sqlExcute;
    /*
     * 下载瓦片时候都触发还剩下多少张瓦片没下,就保存在这里,刷新的时候直接打印到场景前景
     * */
    int tilesCount;
    /*
     * 保存当前鼠标所在的位置对应的世界WGS坐标,刷新的时候直接打印到场景前景
     * */
    QPointF currentPos;

    quint32 itemLimit;
    /*
     * 保存鼠标按下后是否移动了,如果移动了就更新
     * */
    bool mouseMove;
    /*
     * 默认有一个图层,这个图层可以用来保存 GPS当前坐标和统计GPS数据用 或者是临时点之类的数据
     * */
    Layer * tempLayer;
    /*
     * 如果有GPS数据 用来保存卫星个数 没想那么多,对于有定位设备的设备而已
     * */
    int satellitesCount;
    /*
     * 如果有GPS数据 保存高度 和 方向， 速度现在暂时没有用
     * */
    qreal GPSAltitude;
    qreal GPSDir;
    /*
     * 保存比例
     * */
    QList<double> distanceList;
    /*
     * 保存视图中心点，有GPS时保存GPS位置坐标
     * */
    QPointF centerPos;
    /*
     * 保存有没有GPS，默认没有GPS，如果有GPS，打开程序默认跳转到GPS位置
     * */
    bool hasGps;
    QPointF GPSLocation;
    QMutex painMutex;
    double GPSSpeed;
signals:
    void viewChangedSignal(bool);
    void downloadImage();
    void sendLocationPos(QPointF);
    void doubleClicked(QPoint);
    void sendItemList(QList<QGraphicsItem *>);
    void updateLayer();
public slots:
    void viewChangedSlot(bool onlyBackground);
    void newImage();
    void updateTilesCount(int count);
    void updateLocationPos(QPointF world);
    void updateInfo(QPointF GPSPos , qreal speed, qreal dir, qreal altitude);//pos, speed, dir,altitude
    void updateSatellitesCount(int count);
    void addGeoToScene(Geometry * g);
};

#endif // UMAPCONTROL_H
