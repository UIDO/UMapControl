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

#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QList>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include "umapcontrol_global.h"
#include "SQLExcute.h"
#include "UMapControl.h"

/*
 * 提供简单的网络下载能力,主要就是用来下载瓦片的
 * */
namespace UM {
class Network;
}
class UMapControl;
class UMAPCONTROLSHARED_EXPORT Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(UMapControl *iL , QObject * parent = 0);
    ~Network();
    /*
     * 判断当前是否有数据在下载了
     */
    bool getDownloadState();
private:
    /*
     * 从@host和@path里生成瓦片下载地址
     */
    QString getUrl(QString host, QString path);
    /*
     * 从@Url计算出x,y,z坐标
     */
    UM::TPoint getXYZFromUrl(QString Url);
    /*
     * 用来保存所有要下载的瓦片地址了,下完一个山一个
     */
    QList<QString>  *list;
    QNetworkAccessManager * manager;
    bool isDownloading;
    UMapControl *uMap;
    SQLExcute * sqlExcute;
signals:
    void startAgain();
    void sendTileCount(int);
    void addPixGeo(QPointF world, QPixmap pm, quint32 z);
public slots:
    void start();
    void requestFinished(QNetworkReply *reply);
};

#endif // NETWORK_H
