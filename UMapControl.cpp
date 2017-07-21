#include "UMapControl.h"


UMapControl::UMapControl(QWidget *parent) : QGraphicsView(parent),itemScale(1),
    currentLevel(DEFAULTZOOMLEVEL),numberOfTiles(tilesOnZoomLevel(currentLevel)),
    defaultLocation(DEFAULTLOCATION),net(new Network(this)),
    tilesCount(0),currentPos(DEFAULTLOCATION),
    satellitesCount(0),GPSAltitude(0),GPSDir(0),hasGps(false),GPSSpeed(0)
{
    QSqlQuery * query = sqlExcute.getDefaultLoaction();
    while (query->next())
    {
        QString fieldName = query->value(0).toString();
        if(fieldName == "X")
        {
            defaultLocation.setX(query->value(1).toDouble());
            currentPos.setX(query->value(1).toDouble());
        }
        if(fieldName == "Y")
        {
            defaultLocation.setY(query->value(1).toDouble());
            currentPos.setY(query->value(1).toDouble());
        }
        if(fieldName == "LEVEL")
        {
            currentLevel = query->value(1).toInt();
            numberOfTiles= tilesOnZoomLevel(currentLevel);
        }
    }
    delete query;
    //distanceList<<5000000<<2000000<<1000000<<1000000<<1000000<<100000<<100000<<50000<<50000<<10000<<10000<<10000<<1000<<1000<<500<<200<<100<<50<<20;
    distanceList<<5000000<<2000000<<1000000<<500000<<200000<<100000<<50000<<20000<<10000<<5000<<2000<<1000<<500<<200<<100<<50<<20<<10<<5;
    setStyleSheet("background-color:rgb(236,236,236)");
    setScene(new QGraphicsScene(this));
    /*
     * @manager得在QGraphicsScene初始化之后才能使用,所以在这里初始化
     * */
    manager = new Manager(this);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff );
    setViewportUpdateMode(FullViewportUpdate);
    centerOn(0,0);
    //grabGesture(Qt::PinchGesture);
    //viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
    //viewport()->grabGesture(Qt::PinchGesture);
    setSceneRect(viewport()->rect());
    /*
     * 处理刷新信号
     * */
    connect(this,SIGNAL(viewChangedSignal(bool)),this,SLOT(viewChangedSlot(bool)));
    net->moveToThread(&networkThread);
    /*
     * 处理下载信号
     * */
    connect(this,SIGNAL(downloadImage()),net,SLOT(start()));
    connect(net,SIGNAL(addPixGeo(QPointF,QPixmap,quint32)),this,SLOT(addPixGeo(QPointF,QPixmap,quint32)));
    connect(net,SIGNAL(sendTileCount(int)),this, SLOT(updateTilesCount(int)));

    manager->moveToThread(&updateThread);
    connect(this, SIGNAL(updateLayer()), manager, SLOT(updatLayer()));
    /*
     * 处理当前世界坐标位置信号
     * */
    connect(this,SIGNAL(sendLocationPos(QPointF)),this,SLOT(updateLocationPos(QPointF)));
//    resetMatrix();
//    scale(0.5,0.5);
    QList <UM::Format> fm;
    fm << UM::Format{"NAME",UM::UMapT} << UM::Format{"VALUE",UM::UMapN};
    tempLayer = manager->addLayer("UISDO", &fm);
    emit updateLayer();
//    QList<QPointF> l;
//    l.append(QPointF(99.7319,27.8306));
//    l.append(QPointF(99.0115,27.6178));
//    Geometry::ILongDataType tttt;
//    tttt.geometry = new GeoPolygon(this,&l);
//    tttt.data<<"xxx"<<0;
//    tempLayer->addGeo(tttt);
}

UMapControl::~UMapControl()
{
    networkThread.exit(0);
    while(networkThread.isRunning())
        this->thread()->usleep(100);
    updateThread.exit(0);
    while(updateThread.isRunning())
        this->thread()->usleep(100);
}

quint8 UMapControl::maxZoomLevel()
{
    return MAXZOOMLEVEL;
}

quint8 UMapControl::minZoomLevel()
{
    return MINZOOMLEVEL;
}

quint8 UMapControl::zoomLevel()
{
    return currentLevel;
}

void UMapControl::zoomIn()
{
    if(checkZoomLevel(currentLevel + 1))
    {
        currentLevel++;
        zoomOnPos = viewport()->rect().center();
        zoomTo(sceneToWorld(mapToScene(zoomOnPos)),currentLevel,true);
    }
}

void UMapControl::zoomOut()
{
    if(checkZoomLevel(currentLevel - 1))
    {
        currentLevel--;
        zoomOnPos = viewport()->rect().center();
        zoomTo(sceneToWorld(mapToScene(zoomOnPos)),currentLevel,true);
    }
}

QPointF UMapControl::getDefaultLocation()
{
    return defaultLocation;
}

QList<Layer *> UMapControl::getLayers() const
{
    return manager->getLayers();
}

Layer *UMapControl::getlayer(QString name) const
{
    return manager->getLayer(name);
}

Layer *UMapControl::getLayerByID(QString ID) const
{
    return manager->getLayerByID(ID);
}

Layer *UMapControl::addLayer(QString name, QList<UM::Format> *typeList) const
{
    if(!typeList->size() || name.isEmpty())
        return nullptr;
    return manager->addLayer(name,typeList);
}

void UMapControl::removeLayer(QString name)
{
    manager->removeLayer(name);
}

void UMapControl::addTempGeo(QPointF world, UM::GeoType type)
{

    Geometry * g = nullptr;
    QColor pen = QColor(qrand()%255,qrand()%255,qrand()%255);
    QColor brush = QColor(qrand()%255,qrand()%255,qrand()%255);
    switch (type) {
    case UM::iGeoCircle:
        g = new GeoCircle(world,80,pen,brush);
        break;
    case UM::iGeoRect:
        g = new GeoRect(world,80,pen,brush);
        break;
    case UM::iGeoPie:
        g = new GeoPie(world,80,0,pen,brush);
        break;
    case UM::iGeoStar:
        g = new GeoStar(world,80,pen,brush);
        break;
    case UM::iGeoTri:
        g = new GeoTri(world,80,pen,brush);
        break;
    default:
        break;
    }
    if(g)
    {
        Geometry::DataType t;
        t.geometry = g;
        t.data << "Uisdo" << 0;
        tempLayer->addGeo(t);
        zoomTo(world,zoomLevel());
        delete g;
        g = nullptr;
    }
}
QPointF UMapControl::worldToScene(QPointF world)
{
    world = ILoveChina::wgs84TOgcj02(world);
    return QPointF((world.x()+180) * (numberOfTiles*DEFAULTTILESIZE)/360.,
                  (1-(log(tan(PI/4.+degreeToRadian(world.y())/2.)) /PI)) /2.  * (numberOfTiles*DEFAULTTILESIZE));
}

QPointF UMapControl::sceneToWorld(QPointF scene)
{

    return ILoveChina::gcj02Towgs84(QPointF((scene.x()*(360./(numberOfTiles*DEFAULTTILESIZE)))-180,
                                            radianToDegree(atan(sinh((1-scene.y()*(2./(numberOfTiles*DEFAULTTILESIZE)))*PI)))));
}

void UMapControl::goToDefaultLocation()
{
    zoomTo(defaultLocation,zoomLevel());
}

bool UMapControl::moveLayerTo(QString name, bool back)
{
    return manager->moveLayer(name, back);
}

void UMapControl::setViewOffset(int deltaX, int deltaY)
{
    setSceneLocation(QPointF(sceneRect().x() + deltaX, sceneRect().y() + deltaY));
    emit viewChangedSignal(true);
}

void UMapControl::DownloadTiles(quint8 dowloadMaxLevel)
{
    if(dowloadMaxLevel < currentLevel)
        return;
    if(dowloadMaxLevel > MAXZOOMLEVEL)
        dowloadMaxLevel = MAXZOOMLEVEL;
    //进来得先保存 左上角和右下角的世界坐标位置先
    QPointF dowloadTilesTL = sceneToWorld(mapToScene(0,0));
    QPointF dowloadTilesBR = sceneToWorld(mapToScene(viewport()->width(), viewport()->height()));
    for(int level=currentLevel; level<=dowloadMaxLevel; level++)
    {
        QPointF sceneCenter = mapToScene(viewport()->rect().center());
        QPointF leftTopDelta = sceneCenter - worldToScene(dowloadTilesTL);
        QPointF rightBottomDelta = worldToScene(dowloadTilesBR) - sceneCenter;
        QPoint dMid = QPoint(sceneCenter.x() / DEFAULTTILESIZE,sceneCenter.y() / DEFAULTTILESIZE);
        QPoint dTL = QPoint(leftTopDelta.x() / DEFAULTTILESIZE + 1,leftTopDelta.y() / DEFAULTTILESIZE + 1);
        int rightTiles = rightBottomDelta.x() / DEFAULTTILESIZE + 1;
        int bottomTiles = rightBottomDelta.y() / DEFAULTTILESIZE + 1;
        for(int x=-dTL.x()+dMid.x(); x<=rightTiles+dMid.x(); x++)
        {
            for(int y=-dTL.y()+dMid.y(); y<=bottomTiles+dMid.y(); y++)
            {
                if(map.isTileValid(x,y,currentLevel))
                {
                    QString path = map.queryTile(x, y, currentLevel);
                    list.append(path);
                }

            }
        }
        zoomIn();
    }
}

QString UMapControl::dbPath()
{
    return sqlExcute.dbPath();
}

int UMapControl::tilesSize()
{
    QSqlQuery * query = sqlExcute.tilesCount();
    int result = 0;
    if(query->next())
        result = query->value(0).toInt();
    delete query;
    return result;
}

bool UMapControl::GPSUE()
{
    return hasGps;
}

QPointF UMapControl::currentGPS()
{
    return hasGps ? GPSLocation : centerPos;
}

void UMapControl::updateMap()
{
    emit viewChangedSignal(false);
}

void UMapControl::addPixGeo(QPointF world, QPixmap pm, quint32 z)
{
    GeoPix * gp = new GeoPix(world,pm);
    gp->setZValue(z);
    gp->setPos(worldToScene(world));
    scene()->addItem(gp);
}


bool UMapControl::viewportEvent(QEvent *event)
{
    switch(event->type())
    {
    case QEvent::Wheel:
    {
        QWheelEvent * wheelEvent = static_cast<QWheelEvent *>(event);;
        zoomOnPos = wheelEvent->pos();
        wheelEvent->delta() > 0 ? zoomIn() : zoomOut();
        wheelEvent->accept();
        return true;
        break;
    }
    case QEvent::MouseButtonDblClick:
    {
        QMouseEvent * doubleClickEvent = static_cast<QMouseEvent *>(event);
        emit doubleClicked(doubleClickEvent->pos());
        doubleClickEvent->accept();
        return true;
    }
    case QEvent::MouseButtonPress:
    {
        QMouseEvent * pressEvent = static_cast<QMouseEvent *>(event);
        pressEvent->accept();
        if(pressEvent->buttons() & Qt::LeftButton)
        {
            zoomOnPos = pressEvent->pos();
            mouseMove = false;

            QPointF point = mapToScene(pressEvent->pos());
            if (scene()->items(point).count() != 0)
            {
                QList<QGraphicsItem *> l = scene()->items(point);
                for(int i= l.size() - 1; i>=0; i--)
                {
                    Geometry * g = (Geometry *)l.at(i);
                    QStringList nameList = g->objectName().split('_');
                    if(g->objectName().isEmpty() || nameList.size() != 2)
                    {
                        l.removeOne(l.at(i));
                        continue;
                    }
                    Layer * layer = manager->getLayerByID(nameList.at(0));
                    if(!layer->isSelectable())
                        l.removeOne(l.at(i));

                }
                emit sendItemList(l);
            }
        }
        return true;
    }
    case QEvent::MouseMove:
    {
        QMouseEvent * moveEvent = static_cast<QMouseEvent *>(event);
        emit sendLocationPos(sceneToWorld(mapToScene(moveEvent->pos())));
        if(moveEvent->buttons() & Qt::LeftButton)
        {

            QPoint moveDelta = moveEvent->pos() - zoomOnPos;
            //scene()->clear();
            setSceneLocation(QPointF(sceneRect().x() - moveDelta.x(),sceneRect().y() - moveDelta.y()));
            backgroundPos = backgroundPos + moveDelta;
            zoomOnPos = moveEvent->pos();
            mouseMove = true;
            moveEvent->accept();
            /*
             * 这段码让托放变得很卡，理想是很美好的。。。。
            if(backgroundPos.x()>=0 || backgroundPos.y()>=0
                    || backgroundPos.x()+background.width()<=viewport()->width()
                    || backgroundPos.y()+background.height()<=viewport()->height() )
            {
                emit viewChangedSignal(true);
            }
            **/
        }
        return true;
    }
    case QEvent::MouseButtonRelease:
    {
        QMouseEvent * releaseEvent = static_cast<QMouseEvent *>(event);
        {
            if(releaseEvent->button() & Qt::LeftButton && zoomOnPos != QPoint(0,0) && mouseMove)
            {
                emit viewChangedSignal(true);
                zoomOnPos = QPoint(0,0);
                releaseEvent->accept();
            }
        }
        mouseMove = false;
        return true;
    }
    default:
        break;
    }
    return QGraphicsView::viewportEvent(event);
}

void UMapControl::drawBackground(QPainter *p, const QRectF &rect)
{
    Q_UNUSED(rect);
    p->save();
//    p->resetTransform();
//    p->setRenderHint(QPainter::Antialiasing);
//    p->drawPixmap(backgroundPos,background);
    p->restore();
}

void UMapControl::drawForeground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);
    painter->save();
    painter->resetTransform();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QColor(Qt::green));
    QLabel lb;
    QPoint p = viewport()->rect().center();
    int line = distanceList.at( zoomLevel()-1) / pow(2.0, MAXZOOMLEVEL-zoomLevel() -1) / 0.597164;
    int telta = ((viewport()->width() > viewport()->height() ? viewport()->width() : viewport()->height()) / 2) / line;
    for(int i=0; i<telta; i++)
    {
        painter->setPen(QColor( i%2 ? Qt::red : Qt::green));
        painter->drawLine(QPoint(p.x()+i*line,p.y()-5),QPoint(p.x()+i*line,p.y()+5));
        painter->drawLine(QPoint(p.x()-i*line,p.y()-5),QPoint(p.x()-i*line,p.y()+5));
        painter->drawLine(QPoint(p.x()-5,p.y()+i*line),QPoint(p.x()+5,p.y()+i*line));
        painter->drawLine(QPoint(p.x()-5,p.y()-i*line),QPoint(p.x()+5,p.y()-i*line));
    }
    QFont font = painter->font();
    font.setBold(true);
    painter->setFont(font);
    for(int i=1; i<=maxZoomLevel(); i++)
    {
        painter->setPen(QColor(i == zoomLevel() ? Qt::red : Qt::green));
        painter->drawLine(QPoint(width()/2 - ((maxZoomLevel()/2+1)*5) + i*5,height()-15),
                          QPoint(width()/2 - ((maxZoomLevel()/2+1)*5) + i*5,height()-25));
    }
    painter->setPen(QColor(Qt::green));
    QString distance = QVariant( distanceList.at(zoomLevel()-1) / 1000).toString();
    painter->drawText(QPoint(10,height()-15), QString("%1km").arg(distance));
    QString copyRight("U is do");
    painter->drawText(QPoint((width()-lb.fontMetrics().width(copyRight)-15),height()-15),copyRight);
    QString north = centerPos.x() >= 0 ? "N" : "S";
    QString east = centerPos.y() >= 0 ? "E" : "W";
    painter->resetTransform();
    painter->translate(width(),0);
    painter->rotate(90);
    painter->drawText(QPoint(15,10+lb.fontMetrics().height()),QString("%1%2 %3%4").arg(north)
                      .arg(fabs(currentPos.x()),0,'g',10).arg(east)
                      .arg(fabs(currentPos.y()),0,'g',10));
    painter->resetTransform();
    painter->translate(15+lb.fontMetrics().height()/2,15);
    painter->rotate(90);
    //painter->translate(0,width()-15-lb.fontMetrics().height());
    painter->drawText(QPoint(0,10),QString("A:%1 D:%2 T:%3 L:%4 S:%5")
                      .arg(GPSAltitude).arg(GPSDir).arg(tilesCount)
                      .arg(satellitesCount).arg(QString::number(GPSSpeed,'g',2)) );
    painter->restore();
}

void UMapControl::resizeEvent(QResizeEvent *event)
{
    event->accept();
    zoomTo(defaultLocation,currentLevel);
}

void UMapControl::keyPressEvent(QKeyEvent *event)
{
    //QMessageBox::information(this,"x",QString::number( event->key()));
    switch (event->key()) {
    case Qt::Key_J:
    case Qt::Key_Z:
    case Qt::Key_VolumeUp:
        zoomIn();
        break;
    case Qt::Key_K:
    case Qt::Key_X:
    case Qt::Key_VolumeDown:
        zoomOut();
        break;
    case Qt::Key_Up:
    case Qt::Key_W:
        setViewOffset(0,-10);
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        setViewOffset(0,10);
        break;
    case Qt::Key_Left:
    case Qt::Key_A:
        setViewOffset(-10,0);
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        setViewOffset(10,0);
        break;
    default:
        break;
    }
}


qreal UMapControl::degreeToRadian(qreal value)
{
    return value * (PI/180.);
}

qreal UMapControl::radianToDegree(qreal value)
{
    return value * (180./PI);
}

int UMapControl::tilesOnZoomLevel(quint8 zoomLevel)
{
    return int(pow(2.0, zoomLevel));
}

void UMapControl::tilesUrlMatrix(bool onlyBackground)
{
    quint8 offset = 1;
    QPointF sceneCenter = mapToScene(viewport()->rect().center());
    QPointF leftTopDelta = sceneCenter - mapToScene(QPoint(0,0));
    QPointF rightBottomDelta = mapToScene(QPoint(viewport()->width(),viewport()->height())) - sceneCenter;
    middle = QPoint(sceneCenter.x() / DEFAULTTILESIZE,sceneCenter.y() / DEFAULTTILESIZE);
    leftTop = QPoint(leftTopDelta.x() / DEFAULTTILESIZE + offset,leftTopDelta.y() / DEFAULTTILESIZE + offset);
    int rightTiles = rightBottomDelta.x() / DEFAULTTILESIZE + offset;
    int bottomTiles = rightBottomDelta.y() / DEFAULTTILESIZE + offset;
    backgroundPos = mapFromScene((-leftTop.x()+middle.x())*DEFAULTTILESIZE,
                                 (-leftTop.y()+middle.y())*DEFAULTTILESIZE);
    /*
     * 先把所有有效的瓦片坐标保存到tList里
     * */
    QList<QPoint> tList;
    for(int x=-leftTop.x()+middle.x(); x<=rightTiles+middle.x(); x++)
    {
        for(int y=-leftTop.y()+middle.y(); y<=bottomTiles+middle.y(); y++)
        {
            if(map.isTileValid(x,y,currentLevel))
            {
                tList.append(QPoint(x,y));
            }

        }
    }
    /*
     * 删除背景图层
     * */
    if(onlyBackground)
    {
        QList <QGraphicsItem *> l =scene()->items(scene()->sceneRect());
        for(int i=0;i<l.size();i++)
        {
            if(l.at(i)->zValue() == 0)
            {
                scene()->removeItem(l.at(i));
            }
        }
    }
    else
    {
        scene()->clear();
    }
    /*
     * 再从数据库里读取当前场景有效的瓦片,如果读取失败,直接把@tList里的所有坐标生成path保存到@list里,
     * 然后跳到最后启动下载线程
     * */
    QSqlQuery * query = sqlExcute.checkImage(leftTop.x()+middle.x(), -rightTiles+middle.x(),
                                             leftTop.y()+middle.y(), -bottomTiles+middle.y(), currentLevel);
    bool checkImageError = false;
    if(query == nullptr)
    {
        while(!tList.isEmpty())
        {
            QPoint p = tList.first();
            QString path = map.queryTile(p.x(),p.y(),currentLevel);
            list.contains(path) ? list.move(list.indexOf(path),0) : list.insert(0,path);
            tList.removeFirst();
        }
        checkImageError = true;
    }
    /*
     * 如果读取成功,得把瓦片打印到场景的背景图里,并删除@tList里的对就瓦片的坐标
     * */
    while (query->next() && !checkImageError)
    {
        int x = query->value(0).toInt();
        int y = query->value(1).toInt();
        //int z = query->value(2).toInt();
        QPixmap pm;
        pm.loadFromData(query->value(3).toByteArray());
        addPixGeo(sceneToWorld(QPointF(x*DEFAULTTILESIZE,y*DEFAULTTILESIZE)),pm,0);
        tList.removeOne(QPoint(x,y));
    }
    if(query)
    {
        delete query;
        query = 0;
    }
    /*
     * 到了这里,如果tList还没空,那就说明有此瓦片是在数据库里没有的,需要从@tList里把坐标转成path保存到@list里然后启动下载线程
     * 之所以用随机数,就是为了影响瓦片的下载顺序,让人看着感觉不是只能从一个方向刷新场景背景的了,没多大用处
     * */
    qsrand(QDateTime::currentDateTime().time().second());
    while(!tList.isEmpty() && !checkImageError)
    {
        int index = qrand() % tList.size();
        QPoint value = tList.at(index);
        QString path = map.queryTile(value.x(), value.y(), currentLevel);
        list.contains(path) ? list.move(list.indexOf(path),0) : list.insert(0,path);
        tList.removeOne(value);
    }
    if(!networkThread.isRunning())
        networkThread.start();
    if(!net->getDownloadState())
        emit downloadImage();
}

void UMapControl::zoomTo(QPointF world, quint8 zoomLevel, bool underMouse)
{
    currentLevel = zoomLevel;
    numberOfTiles = tilesOnZoomLevel(currentLevel);
    itemScale = currentLevel * 1. / MAXZOOMLEVEL ;
    setSceneLocation(worldToScene(world) -
                     (underMouse ? zoomOnPos : viewport()->rect().center()),
                     true);

}

void UMapControl::setSceneLocation(QPointF topLeftPos, bool updateItem)
{
    setSceneRect(topLeftPos.x(),topLeftPos.y(),viewport()->width(), viewport()->height());
    centerPos = sceneToWorld(mapToScene(viewport()->rect().center()));
    if(updateItem)
        emit viewChangedSignal(false);
}

bool UMapControl::checkZoomLevel(quint8 zoomLevel)
{
    return (zoomLevel>=MINZOOMLEVEL && zoomLevel<=MAXZOOMLEVEL);
}

bool UMapControl::checkWorldCoordinate(QPointF world)
{
    return (world.x() >= -180 && world.x() <= 180 && world.y() >= -85 && world.y() <= 85);
}

void UMapControl::addGeoToScene(Geometry *g)
{
    if(g)
        scene()->addItem(g);
}

void UMapControl::viewChangedSlot(bool onlyBackground)
{
    manager->stopUpdateLayer();
    tilesUrlMatrix(onlyBackground);
    if(!updateThread.isRunning())
        updateThread.start();
    if(onlyBackground)
        return;
    emit updateLayer();
    /*
     * 更新完视图，保存当前视图到数据库
     * 本来要退出的时候再保存的，但是发在手机上没用
     * */
     sqlExcute.updateDefaultLoaction(centerPos,currentLevel);
}

void UMapControl::updateTilesCount(int count)
{
    tilesCount = count;
    viewport()->update();
}

void UMapControl::updateLocationPos(QPointF world)
{
    currentPos = world;
    viewport()->update();
}

void UMapControl::updateInfo(QPointF GPSPos, qreal speed, qreal dir, qreal altitude)
{
    GPSSpeed = speed;
    /*
     * 如果有GPS,第一次更新应该要自己跳到GPS位置,不管地图是啥等级
     * */
    if(!hasGps)
    {
        hasGps = true;
        zoomTo(GPSPos,zoomLevel());
    }
    GPSDir = dir;
    currentPos = GPSPos;
    GPSLocation = GPSPos;
    /*
     * 因为如果有GPS的话界面打印出来的应该是GPS位置坐标，没有GPS就打印界面中心点位置坐标
     * GPS位置要覆盖中心坐标，也覆盖默认位置坐标，在没有GPS设备上跳转到设置好的默认位置，有GPS就跳到GPS位置
     * */
    defaultLocation = GPSPos;
    centerPos = GPSPos;

    GPSAltitude = altitude;
    manager->addTempItem(GPSPos);
}

void UMapControl::updateSatellitesCount(int count)
{
    satellitesCount = count;
}

