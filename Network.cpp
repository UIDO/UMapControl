#include "Network.h"

Network::Network(UMapControl *iL, QObject *parent) : QObject(parent),list(&iL->list),
    manager(new QNetworkAccessManager(this)),isDownloading(false),uMap(iL),sqlExcute(&iL->sqlExcute)
{
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(requestFinished(QNetworkReply*)));
    connect(this,SIGNAL(startAgain()),this,SLOT(start()));
}

Network::~Network()
{
    if(manager)
    {
        delete manager;
        manager = 0;
    }
}

bool Network::getDownloadState()
{
    return isDownloading;
}

QString Network::getUrl(QString host, QString path)
{
    QString hostName = host;
    QString portNumber = QString("80");
    QRegExp r(".:.");

    if(r.indexIn(host) >= 0)
    {
        QStringList s = host.split(":");
        hostName = s.at(0);
        portNumber = s.at(1);
    }
    return QString("http://%1:%2%3").arg(hostName).arg(portNumber).arg(path);
}

UM::TPoint Network::getXYZFromUrl(QString Url)
{
    /*
     * 感觉这么写很Low,但先这么用着吧
     * */
    int xoffset = Url.indexOf("x=");
    int yoffset = Url.indexOf("y=");
    int zoffset = Url.indexOf("z=");

    int x = Url.mid(xoffset+2,yoffset-(xoffset+3)).toInt();
    int y = Url.mid(yoffset+2,zoffset-(yoffset+3)).toInt();
    int z = Url.mid(zoffset+2,Url.length()).toInt();
    UM::TPoint t;
    t.x = x;
    t.y = y;
    t.z = z;
    return t;
}

void Network::start()
{
    if(!list->isEmpty())
    {
        isDownloading = true;
        QString fullUrl = getUrl(uMap->map.getServer(),list->at(0));
        QNetworkRequest request = QNetworkRequest(QUrl(fullUrl));
        request.setRawHeader("User-Agent", "Mozilla/5.0 (PC; U; Intel; Linux; en) AppleWebKit/420+ (KHTML, like Gecko)");
        manager->get(request);
        list->removeFirst();
        emit sendTileCount(list->size());
        return;
    }
    isDownloading  = false;
    this->thread()->exit();
}

void Network::requestFinished(QNetworkReply *reply)
{
    emit startAgain();
    if (!reply)
    {
        qDebug() << "MapNetwork::requestFinished - reply no longer valid";
        return;
    }
    if (reply->error() != QNetworkReply::NoError)
    {
        //qDebug() << "QNetworkReply Error: " << reply->errorString();
        return;
    }
    QByteArray ax;
    if (reply->bytesAvailable()>0)
    {
        QPixmap pm;
        ax = reply->readAll();

        if (pm.loadFromData(ax) && pm.size().width() > 1 && pm.size().height() > 1)
        {
            UM::TPoint t = getXYZFromUrl(reply->url().toString());
            /*
             * 下载到的瓦片,看看是不是当前场景内,如果是就打印到场景背景图片里
             * */
            if(t.z == uMap->zoomLevel())
            {
                QPointF br = uMap->mapToScene(QPoint(uMap->width(),uMap->height()));
                int x = t.x*DEFAULTTILESIZE;
                int y = t.y*DEFAULTTILESIZE;
                if(t.x>=uMap->leftTop.x() && x<br.x() && y >=t.y && y < br.y())
                {
                    emit addPixGeo(uMap->sceneToWorld(QPointF(x,y)),pm,0);
                }
            }
            /*
             * 不管是不是场景里的瓦片,只要有瓦片下完都保存到数据库里,方便下次直接调用嘛是吧
             * */
            sqlExcute->insertImage(t.x, t.y, t.z, ax);
        }
    }
}

