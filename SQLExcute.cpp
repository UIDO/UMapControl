#include "SQLExcute.h"

SQLExcute::SQLExcute(QObject *parent) : QObject(parent)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QDir dir;
    if(!dir.exists(CONFIGPATH))
    {
        dir.mkdir(CONFIGPATH);
    }
    db.setDatabaseName(CONFIGPATH + "uisdo.com");
    db.transaction();
    db.commit();
    if(!db.open())
    {
        qDebug() << "Init SQLITE Open " << db.lastError().text();
    }
    QString sql = "CREATE TABLE IF NOT EXISTS UISDO(X INTEGER, Y INTEGER, Z INTEGER, IMAGE LONGBLOB, primary key(X,Y,Z))";
    QSqlQuery query(db);
    if(!query.exec(sql))
    {
        qDebug() << "CREATE TABLE UISDO " << query.lastError().text();
    }
    sql = "CREATE TABLE IF NOT EXISTS UMCONF(NAME TEXT PRIMARY KEY, VALUE INTEGER DEFAULT 0)";
    if(!query.exec(sql))
    {
        qDebug() << "CREATE TABLE UMCONF " << query.lastError().text();
    }
}

void SQLExcute::addItems(QList<Geometry::DataType> *dataList,
                         QString id, QList<UM::Format> *headType)
{
    QSqlDatabase db;
    if(QSqlDatabase::contains("qt_sql_default_connection"))
      db = QSqlDatabase::database("qt_sql_default_connection");
    else
      db = QSqlDatabase::addDatabase("QSQLITE");
    if(!db.isOpen())
    {
        if(!db.open())
        {
            qDebug() << db.lastError().text();
            return;
        }
    }
    QSqlQuery query(db);
    db.transaction();
    for(int i=0; i<dataList->size(); i++)
    {
        Geometry::DataType data = dataList->at(i);
        if(data.data.size() + 1 < headType->size())
        {
            qDebug() << "data error";
            continue;
        }
        /*
         * 先把数据插入到数据表先
         * */
        QString sqlT = "";
        for(int j=1; j<headType->size(); j++)
        {
            if(headType->at(j).type == UM::UMapN)
            {
                /*
                 * 其实感觉没必要检查是不是能转换成数字了,但是我人好嘛,慢点就慢点了
                 * 等有好办法再说
                 * */
                bool ok;
                qreal result = data.data.at(j-1).toReal(&ok);
                if(!ok)
                    result = 0;
                sqlT += QString(" '%1', ").arg(result);
            }
            else
            {
                sqlT += " '" + data.data.at(j-1).toString() + "', ";
            }
        }
        sqlT = sqlT.left(sqlT.length()-2);
        QString sql = QString("INSERT INTO '%1' VALUES ( '%2', %3 )").arg(id).arg(data.geometry->getID()).arg(sqlT);
        if(!query.exec(sql))
        {
            /*
             * 如果插入数据表失败就没必要插入信息表了,直接跳过处理这个图元了
             * */
            qDebug() << query.lastError().text() << sql ;
            continue;
        }
        /*
         *创建信息表,专用保存图元的,应该可以直接保存图元,但是现在还不知道怎么弄,就先这样弄吧,以后再想办法改进(个人技术原因),主要信息有:
         * @UMID     与数据的ID关联;
         * @TYPE        ILongGeoType 枚举图元类型
         * @CenterX     图元wgs CenterX 坐标
         * @CenterY     图元wgs CenterX 坐标
         * @MINX        图元最小wgs X坐标 (点类图元写CenterX相同)
         * @MINY        图元最小wgs X坐标 (点类图元写CenterY相同)
         * @MAXX        图元最大wgs X坐标 (点类图元写CenterX相同)
         * @MAXY        图元最大wgs Y坐标 (点类图元写CenterY相同) 设计两个坐标点只为了非点类图元需要计算边界问题,比如线
         * @LABEL       用来显示图标注的, 如果设置显示标注,就从数据表里面把标注内容填充到该字段
         * @INFO        保存图元GIS信息
         *              格式: WGSx1,WGSy1_WGSx2,WGSy2_.o.._WGSxN,WGSyN
         * @FLAGS 点类旋转角度或面类图元闭环(FLAGS==0 线条， FLAGS!=0 多边形)
         * @SIZE 多边形或线条线宽或点类图元大小
         * ＠PEN 画笔(R_G_B)
         * ＠BRUSH 画刷(R_G_B)
         *
        */
        QPointF cen = data.geometry->getCenter();
        UM::UmapGeoRect rect = data.geometry->getRect();
        sql = QString("INSERT INTO '%1INFO' VALUES ( '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10', '%11', '%12', '%13', '%14', '%15')")
                .arg(id).arg(data.geometry->getID()).arg(data.geometry->getGeoType()).arg(cen.x())
                .arg(cen.y()).arg(rect.minX)
                .arg(rect.minY).arg(rect.maxX).arg(rect.maxY).arg("ILONGNULL")
                //@INFO
                .arg(data.geometry->getPoints())
                //@FLAGS
                .arg(data.geometry->getGeoType() == UM::iGeoPolygon ? data.geometry->getCloseFlag():data.geometry->getDir())
                //@SIZE
                .arg(data.geometry->getGeoType() == UM::iGeoPolygon ? data.geometry->getLineWidth():data.geometry->getSize())
                //@PEN @BRUSH
                .arg(data.geometry->getPen()).arg(data.geometry->getBrush());
        if(!query.exec(sql))
        {
            /*
             * 如果图元信息表插入失败,得删除数据表里对应的数据
             * */
            qDebug() << query.lastError().text() << sql ;
            sql = QString("DELETE FROM '%1' WHERE UMID = %2 ").arg(id).arg(data.geometry->getID());
            if(!query.exec(sql))
            {
                /*
                 * 如果删除失败,那就出现数据混乱了,可以把图层删除了再导入图层吧
                 * */
                qDebug() << query.lastError().text() << sql ;
            }
        }
    }
    db.commit();
    db.transaction();
}

void SQLExcute::removeItem(QString layerID, QString itemID)
{
    QString sql = QString("DELETE FROM '%1' WHERE UMID = '%2' ").arg(layerID).arg(itemID);
    nonResult(sql, "removeItem ");
    sql = QString("DELETE FROM '%1INFO' WHERE UMID = '%2' ").arg(layerID).arg(itemID);
    nonResult(sql, "removeItem at info");
}

QSqlQuery *SQLExcute::checkImage(int maxX, int minX, int maxY, int minY, int z)
{
    QString sql = QString("SELECT * FROM UISDO WHERE X >= %1 and X <= %2 and Y <= %3 and Y >= %4 and Z = %5")
            .arg(minX).arg(maxX).arg(maxY).arg(minY).arg(z);
    return getResult(sql, "checkImage");
}

void SQLExcute::insertImage(int x, int y, int z, QByteArray ax)
{
    QSqlDatabase db;
    if(QSqlDatabase::contains("qt_sql_default_connection"))
      db = QSqlDatabase::database("qt_sql_default_connection");
    else
      db = QSqlDatabase::addDatabase("QSQLITE");
    if(!db.isOpen())
    {
        if(!db.open())
        {
            qDebug() << "insertImage Open " << db.lastError().text();
            return;
        }
    }
    db.transaction();
    QSqlQuery query(db);
    query.prepare("REPLACE INTO UISDO VALUES (?,?,?,?)");
    query.addBindValue(x);
    query.addBindValue(y);
    query.addBindValue(z);
    query.addBindValue(ax);
    if(!query.exec())
    {
        qDebug() << "insertImage query.exec() " << query.lastError().text();
    }
    db.commit();
}

QSqlQuery *SQLExcute::initLayerManager()
{
    QString sql = "CREATE TABLE IF NOT EXISTS ILONGIOLAYER(ID TEXT, NAME TEXT, VISIBLE INTEGER, SELECTABLE INTEGER)";
    nonResult(sql, "initLayerManager");
    sql = "SELECT * FROM ILONGIOLAYER";
    return getResult(sql,"initLayerManager check data");
}

QSqlQuery *SQLExcute::checkType(QString id)
{
    QString sql = QString(" PRAGMA table_info('%1') ").arg(id);
    return getResult(sql,"checkType " + id);
}

QSqlQuery *SQLExcute::updateLayer(QString id, QPointF topLeft, QPointF rigthBottom, quint32 limit)
{
    QString t = QString("NOT ( MINX > %1 OR MAXX < %2 OR MINY > %3 OR MAXY < %4 )")
            .arg(rigthBottom.x()).arg(topLeft.x()).arg(topLeft.y()).arg(rigthBottom.y());
    QString sql = QString("SELECT * FROM '%1INFO' WHERE %2 GROUP BY RANDOM() LIMIT 0,%3")
            .arg(id).arg(t).arg(limit);
    return getResult(sql,"updat layer");
}

void SQLExcute::initLayer(QString id, QString name, QList<UM::Format> *typeList, QList<UM::Format> *headType)
{
    /*
     * 在ILONGIOLAYER建立表索引
     * 一个图层分成两个表
     * 一个表保存数据(数据表)
     * 一个表保存图元信息(信息表)
    */
    QString sql = QString("INSERT INTO ILONGIOLAYER VALUES ( '%1', '%2', '%3', '%4' )").arg(id).arg(name).arg(1).arg(1);
    nonResult(sql, "initLayer1");
    /*
     * 读取表数据结构 并创建表 把结构保存在字段类型headType里,方便插入图元数据使用
     */
    QString saveSql = "";
    for(int i=0; i< typeList->size(); i++)
    {
        QString t = QString(" '%1' '%2' , ").arg(typeList->at(i).name).arg(typeList->at(i).type ? "TEXT" : "REAL");
        headType->append(typeList->at(i));
        saveSql += t;
    }
    saveSql = saveSql.left(saveSql.length() - 2);
    /*
     * 创建数据表,专用保存导入的数据
     */
    sql = QString("CREATE TABLE '%1' (UMID REAL, %2 )").arg(id).arg(saveSql);
    nonResult(sql, "initLayer create data table ");
    /*
     *创建信息表,专用保存图元的,应该可以直接保存图元,但是现在还不知道怎么弄,就先这样弄吧,以后再想办法改进(个人技术原因),主要信息有:
     * @UMID     与数据的ID关联;
     * @TYPE        ILongGeoType 枚举图元类型
     * @CenterX     图元wgs CenterX 坐标
     * @CenterY     图元wgs CenterX 坐标
     * @MINX        图元最小wgs X坐标 (点类图元写CenterX相同)
     * @MINY        图元最小wgs X坐标 (点类图元写CenterY相同)
     * @MAXX        图元最大wgs X坐标 (点类图元写CenterX相同)
     * @MAXY        图元最大wgs Y坐标 (点类图元写CenterY相同) 设计两个坐标点只为了非点类图元需要计算边界问题,比如线
     * @LABEL       用来显示图标注的, 如果设置显示标注,就从数据表里面把标注内容填充到该字段
     * @INFO        保存图元GIS信息
     *              格式: WGSx1,WGSy1_WGSx2,WGSy2_..._WGSxN,WGSyN
     * @FLAGS 点类旋转角度或面类图元闭环(FLAGS==0 线条， FLAGS!=0 多边形)
     * @SIZE 多边形或线条线宽或点类图元大小
     * ＠PEN 画笔(R_G_B)
     * ＠BRUSH 画刷(R_G_B)
     *
    */
    sql =QString("CREATE TABLE %1INFO (UMID REAL, TYPE REAL, CenterX REAL, CenterY REAL, "
                 "MINX REAL, MINY REAL, MAXX REAL, MAXY REAL, LABEL TEXT, INFO TEXT, FLAGS REAL,"
                 "SIZE REAL, PEN TEXT, BRUSH TEXT)").arg(id);
    nonResult(sql, "initLayer create info table ");
}

QSqlQuery *SQLExcute::getItemInfo(QString itemLayerID, QString itemID)
{
    QString sql = QString("SELECT * FROM '%1' WHERE UMID = '%2'").arg(itemLayerID).arg(itemID);
    return getResult(sql,"getItemInfo");
}

QSqlQuery *SQLExcute::searchInfo(QString itemLayerID, QString field, UM::DataType fieldType, QString text)
{
    QString sql;
    if(fieldType == UM::UMapN)
        sql = QString("SELECT UMID, %1 FROM '%2' WHERE %3 = %4").arg(field).arg(itemLayerID).arg(field).arg(text);
    else
        sql = QString("SELECT UMID, %1 FROM '%2' WHERE %3 like '%%4%'").arg(field).arg(itemLayerID).arg(field).arg(text);
    return getResult(sql,"searchInfo");
}

QSqlQuery *SQLExcute::setViewToItem(QString layerID, QString itemID)
{
    QString sql = QString("SELECT CenterX,CenterY FROM '%1INFO' WHERE UMID = '%2'").arg(layerID).arg(itemID);
    return getResult(sql,"setViewToItem");
}

QSqlQuery *SQLExcute::getDefaultLoaction()
{
    QString sql = "SELECT * FROM UMCONF";
    return getResult(sql,"getDefaultLoaction");
}

void SQLExcute::updateGeoColor(QString layerId, quint32 geoID, QString field, QColor color)
{
    if(!(field=="PEN" || field=="BRUSH"))
        return;
    QString c = QString("%1_%2_%3").arg(color.red()).arg(color.green()).arg(color.blue());
    QString sql = QString("UPDATE '%1INFO' SET '%2' = '%3' WHERE UMID = '%4'")
            .arg(layerId).arg(field).arg(c).arg(geoID);
    nonResult(sql,"updateGeoColor");
}

void SQLExcute::updateDefaultLoaction(QPointF world, quint8 level)
{
    QString sql = QString("REPLACE INTO UMCONF VALUES ('X', %1)").arg(world.x());
    nonResult(sql, "update x ");
    sql = QString("REPLACE INTO UMCONF VALUES ('Y', %1)").arg(world.y());
    nonResult(sql, "update y ");
    sql = QString("REPLACE INTO UMCONF VALUES ('LEVEL', %1)").arg(level);
    nonResult(sql, "update level ");
}

void SQLExcute::updateItemLimit(quint32 limit)
{
    QString sql = QString("REPLACE INTO UMCONF VALUES ('LIMIT', %1)").arg(limit);
    nonResult(sql, "update limit ");
}

QString SQLExcute::dbPath()
{
    return CONFIGPATH + "uisdo.com";
}

QSqlQuery *SQLExcute::tilesCount()
{
    QString sql = "SELECT COUNT(*) FROM UISDO";
    return getResult(sql,"tilesCount");
}

void SQLExcute::removeLayer(QString id)
{
    QString sql = QString("DELETE FROM ILONGIOLAYER WHERE ID = '%1' ").arg(id);
    nonResult(sql, "removeLayer on ILONGIOLAYER " + id);
    sql = QString("DROP TABLE '%1' ").arg(id);
    nonResult(sql, "DROP TABLE " + id);
    sql = QString("DROP TABLE '%1INFO' ").arg(id);
    nonResult(sql, "DROP INFO TABLE " + id);
}

void SQLExcute::clearLayer(QString id)
{
    QString sql = QString("DELETE FROM '%1' ").arg(id);
    nonResult(sql, "CLEAR " + id);
    sql = QString("DELETE FROM '%1INFO' ").arg(id);
    nonResult(sql, "CLEAR INFO TABLE " + id);
}

void SQLExcute::setLayerVisible(QString id, bool b)
{
    QString sql = QString("UPDATE ILONGIOLAYER SET VISIBLE = '%1' WHERE ID = '%2' ").arg(b).arg(id);
    nonResult(sql, "setLayerVisible " + id);
}

void SQLExcute::setLayerSelectable(QString id, bool b)
{
    QString sql = QString("UPDATE ILONGIOLAYER SET SELECTABLE = '%1' WHERE ID = '%2' ").arg(b).arg(id);
    nonResult(sql, "setLayerVisible " + id);
}

void SQLExcute::setLabel(QString id, QString field)
{
    //ILONGNULL 清除标注
    QString sql;
    if(field == "ILONGNULL")
        sql = QString("UPDATE '%1INFO' SET LABEL = 'ILONGNULL' ").arg(id);
    else
        sql = QString("UPDATE '%1INFO' SET LABEL = (SELECT %2 FROM '%1' WHERE UMID = '%1INFO'.UMID) ")
            .arg(id).arg(field);
    nonResult(sql, "setLabel " + id);
}

QSqlQuery *SQLExcute::getPosByItemID(QString layerID, QString itemID)
{
    QString sql = QString("SELECT CenterX,CenterY FROM '%1INFO' WHERE UMID = '%2'").arg(layerID).arg(itemID);
    return getResult(sql,"getPosByItemID");
}

QSqlQuery *SQLExcute::getResult(QString sql, QString position)
{
    QSqlDatabase db;
    if(QSqlDatabase::contains("qt_sql_default_connection"))
      db = QSqlDatabase::database("qt_sql_default_connection");
    else
      db = QSqlDatabase::addDatabase("QSQLITE");
    if(!db.isOpen())
    {
        if(!db.open())
        {
            qDebug() << position << db.lastError().text();
            return nullptr;
        }
    }
    QSqlQuery * query = new QSqlQuery(db);
    if(!query->exec(sql))
    {
        qDebug() << position << query->lastError().text();
        return nullptr;
    }
    return query;
}

void SQLExcute::nonResult(QString sql, QString position)
{
    QSqlDatabase db;
    if(QSqlDatabase::contains("qt_sql_default_connection"))
      db = QSqlDatabase::database("qt_sql_default_connection");
    else
      db = QSqlDatabase::addDatabase("QSQLITE");
    if(!db.isOpen())
    {
        if(!db.open())
        {
            qDebug() << position << db.lastError().text();
            return;
        }
    }
    db.transaction();
    QSqlQuery query(db);
    if(!query.exec(sql))
    {
        qDebug() << position << query.lastError().text();
        qDebug() << sql;
    }
    db.commit();
}



