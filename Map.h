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

#ifndef MAP_H
#define MAP_H

#include <QObject>
#include <QLocale>

#include "umapcontrol_global.h"

/*
 * 地图供应商管理
 * */
namespace UM {
class Map;
}
class Map : public QObject
{
    Q_OBJECT
public:
    explicit UMAPCONTROLSHARED_EXPORT Map(QObject *parent = 0);
    /*
     * 取到地图供应商服务地址
     * */
    QString getServer();
    QString getPath();
    /*
     * 得到坐标点的瓦片下载地址的path部分,如:/maps/vt?lyrs=s@701,r@701&gl=cn&x=%2&y=%3&z=%1
     * */
    QString queryTile(int x, int y, int z);
    /*
     * 判断瓦片是否有效
     * */
    bool isTileValid(int x, int y, int z);
private:
    int param1;
    int param2;
    int param3;
    int order[3][2];

    QString server;
    QString path;

    QLocale loc;
signals:

public slots:
};

#endif // MAP_H
