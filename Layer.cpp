#include "Layer.h"

Layer::Layer(UMapControl *parent, QString name, QList<UM::Format> *typeList) : uMap(parent),
    layerLabel(name),visible(true),selectable(true),sqlExcute(&parent->sqlExcute)
{
    /*
     * 这是直接创建新图层并写入数据库
     * */
    QUuid id = QUuid::createUuid();
    layerID =  QString("UISDO%1").arg(id.data1);
    UM::Format f;
    f.name = "UMID";
    f.type = UM::UMapN;
    headType.append(f);
    sqlExcute->initLayer(layerID,layerLabel,typeList, &headType);
    connect(this, SIGNAL(addGeoToScene(Geometry*)), uMap, SLOT(addGeoToScene(Geometry*)));
}

Layer::Layer(UMapControl *parent, QString id, QString name, bool visible, bool selectable):
    uMap(parent),layerLabel(name),layerID(id),sqlExcute(&parent->sqlExcute)
{
    /*
     * 这得从数据库里读取表加载成图层
     * */
    this->visible = visible;
    this->selectable = selectable;
    QSqlQuery * query = sqlExcute->checkType(layerID);
    while(query->next())
    {
        QString value = query->value(1).toString();
        QString type = query->value(2).toString();
        UM::Format t;
        t.name = value;
        t.type = type == "TEXT" ? UM::UMapT : UM::UMapN;
        headType.append(t);
    }
    delete query;
    query = 0;
    connect(this, SIGNAL(addGeoToScene(Geometry*)), uMap, SLOT(addGeoToScene(Geometry*)));
}

Layer::~Layer()
{
    sqlExcute->removeLayer(layerID);
}

QSqlQuery *Layer::searchInfo(QString field, QString text)
{
    UM::DataType t = UM::UMapT;
    for(int i=0; i<headType.size();i++)
    {
        if(headType.at(i).name == field)
        {
            t = headType.at(i).type;
            break;
        }
    }
    if(t == UM::UMapN)
    {
        bool ok;
        text.toDouble(&ok);
        if(!ok)
            return nullptr;
    }
    return sqlExcute->searchInfo(layerID,field,t,text);
}

void Layer::setViewToItem(QString itemID)
{
    QSqlQuery * query = sqlExcute->setViewToItem(getLayerID(),itemID);
    while(query->next())
    {
        bool ok;
        double x = query->value(0).toDouble(&ok);
        if(!ok)
            break;
        double y = query->value(1).toDouble(&ok);
        if(!ok)
            break;
        uMap->zoomTo(QPointF(x,y),uMap->zoomLevel());
        break;
    }
    delete query;
    query = 0;
}

void Layer::addGeo(Geometry::DataType data)
{
    QList<Geometry::DataType> l;
    l.append(data);
    addGeos(&l);
}

void Layer::addGeos(QList<Geometry::DataType> *dataList)
{
    sqlExcute->addItems(dataList,layerID, &headType);
}

QList<Geometry *> * Layer::getItems()
{
    return &list;
}


void Layer::updatLayer(bool *isUpdate)
{
    list.clear();
    if(!*isUpdate)
        return;
    QSqlQuery * query =sqlExcute->updateLayer(layerID);
    while(query->next() && *isUpdate)
    {
        int type = query->value(1).toInt();
        Geometry * g = nullptr;
        ILongInfo itemInfo = getInfo(query);
        switch (type) {
        case UM::iGeoCircle:
            g = new GeoCircle(itemInfo.center,itemInfo.size,itemInfo.pen,itemInfo.brush);
            break;
        case UM::iGeoRect:
            g = new GeoRect(itemInfo.center,itemInfo.size,itemInfo.pen,itemInfo.brush);
            break;
        case UM::iGeoPie:
            g = new GeoPie(itemInfo.center,itemInfo.size,itemInfo.flags,itemInfo.pen,itemInfo.brush);
            break;
        case UM::iGeoStar:
            g = new GeoStar(itemInfo.center,itemInfo.size,itemInfo.pen,itemInfo.brush);
            break;
        case UM::iGeoTri:
            g = new GeoTri(itemInfo.center,itemInfo.size,itemInfo.pen,itemInfo.brush);
            break;
        case UM::iGeoPolygon:
            g = new GeoPolygon(uMap,&itemInfo.list,itemInfo.flags,itemInfo.size,itemInfo.pen,itemInfo.brush);
            break;
        default:
            break;
        }
        if(g && *isUpdate)
        {
            g->setZValue(1);
            if(type == UM::iGeoPolygon)
            {
                g->setPos(uMap->worldToScene(QPointF(g->getRect().minX,g->getRect().maxY)));
                //g->setFlag(QGraphicsItem::ItemIsSelectable);
            }
            else
            {
                g->setPos(uMap->worldToScene(itemInfo.center));
                g->setScale(uMap->itemScale);
                g->rotate(itemInfo.flags);
            }
            if(itemInfo.label != "ILONGNULL")
                g->setLabel(itemInfo.label);
            g->setObjectName(QString("%1_%2").arg(layerID).arg(itemInfo.id));
            emit addGeoToScene(g);
            list.append(g);
        }
    }
    delete query;
    query = 0;
}

void Layer::setLabel(QString field)
{
    sqlExcute->setLabel(layerID, field);
}

void Layer::updateGeoPenColor(quint32 geoID, QColor c)
{
    sqlExcute->updateGeoColor(this->getLayerID(),geoID,"PEN",c);
}

void Layer::updateGeoBrushColor(quint32 geoID, QColor c)
{
    sqlExcute->updateGeoColor(this->getLayerID(),geoID,"BRUSH",c);
}

void Layer::removeGeo(QString itemID)
{
    sqlExcute->removeItem(getLayerID(), itemID);
}


QString Layer::getLayerName()
{
    return layerLabel;
}

QString Layer::getLayerID()
{
    return layerID;
}

QList<UM::Format> *Layer::getLayerHead()
{
    return &headType;
}

void Layer::setVisible(bool b)
{
    visible = b;
    sqlExcute->setLayerVisible(layerID, b);
}

bool Layer::isVisible()
{
    return visible;
}

void Layer::setSelectable(bool b)
{
    selectable = b;
    sqlExcute->setLayerSelectable(layerID, b);
}

bool Layer::isSelectable()
{
    return selectable;
}

QPointF Layer::getItemPosByID(QString itemID)
{
    QSqlQuery * query  = sqlExcute->getPosByItemID(getLayerID(),itemID);
    if(query->next())
    {
        return QPointF(query->value(0).toDouble(),query->value(1).toDouble());
    }
    return QPointF(0,0);
    delete query;
}

Layer::ILongInfo Layer::getInfo(QSqlQuery *query)
{
    /*
     * 主要信息有:
     * @UMID     与数据的ID关联;
     * @TYPE        ILongGeoType 枚举图元类型
     * @CenterX     图元wgs CenterX 坐标
     * @CenterY     图元wgs CenterX 坐标
     * @MINX        图元最小wgs X坐标 (点类图元写CenterX相同)
     * @MINY        图元最小wgs X坐标 (点类图元写CenterY相同)
     * @MAXX        图元最大wgs X坐标 (点类图元写CenterX相同)
     * @MAXY        图元最大wgs Y坐标 (点类图元写CenterY相同) 设计两个坐标点只为了非点类图元需要计算边界问题,比如线
     * @LABEL       用来显示图标注的, 如果设置显示标注,就从数据表里面把标注内容填充到该字段  默认 ILONGNULL
     * @INFO        保存图元GIS信息
     *              格式: WGSx1,WGSy1_WGSx2,WGSy2_..._WGSxN,WGSyN
     * @FLAGS 点类旋转角度或面类图元闭环(FLAGS==0 线条， FLAGS!=0 多边形)
     * @SIZE 多边形或线条线宽或点类图元大小
     * ＠PEN 画笔(R_G_B)
     * ＠BRUSH 画刷(R_G_B)
     *
    */
    ILongInfo info;
    info.id = query->value(0).toDouble();
    info.center = QPointF(query->value(2).toDouble(),query->value(3).toDouble());
    info.label =  query->value(8).toString();
    info.list = getGisList(query->value(9).toString());
    info.flags = query->value(10).toInt();
    info.size = query->value(11).toInt();
    QStringList iPen = query->value(12).toString().split('_');
    info.pen = QColor(iPen.at(0).toInt(),iPen.at(1).toInt(),iPen.at(2).toInt());
    iPen = query->value(13).toString().split('_');
    info.brush = QColor(iPen.at(0).toInt(),iPen.at(1).toInt(),iPen.at(2).toInt());
    return info;
}

QList<QPointF> Layer::getGisList(QString gis)
{
    QList<QPointF> l;
    QStringList tl = gis.split('_');
    while (!tl.isEmpty())
    {
        QString str = tl.first();
        QStringList tmp = str.split(',');
        l.append(QPointF(tmp.at(0).toDouble(),tmp.at(1).toDouble()));
        tl.removeFirst();
    }
    return l;
}

