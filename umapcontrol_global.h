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

#ifndef UMAPCONTROL_GLOBAL_H
#define UMAPCONTROL_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QDir>

namespace UM {

#if defined(UMapControl_LIBRARY)
#  define UMAPCONTROLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define UMAPCONTROLSHARED_EXPORT Q_DECL_IMPORT
#endif

#define PI 3.14159265358979323846264338327950288419717
#define DEFAULTTILESIZE 256
#define MAXZOOMLEVEL 19
#define MINZOOMLEVEL 1
#define DEFAULTZOOMLEVEL MINZOOMLEVEL
#define DEFAULTLOCATION QPointF(0,0)
#define CONFIGPATH QDir::homePath() + "/.UISDO/"

typedef struct
{
    int x;
    int y;
    int z;
} TPoint;
/*
 * 图元类型就只会为两种,
 * 1,点类图元,可以有很多种点类图元,都是以中心点画图元
 * 2,面类图元,由多个坐标生成的图元,线条也是面类图元的一种
 * */
typedef enum
{
    iGeoNull,    //初始化类型
    //点类图元
    iGeoCircle,  //点 圆
    iGeoRect,    //矩形
    iGeoPie,     //扁形
    iGeoStar,    //星星
    iGeoTri,     //三角
    //面类图元
    iGeoPolygon,//线段和平面
} GeoType;

//保存图元的边界
typedef struct
{
    qreal minX;
    qreal minY;
    qreal maxX;
    qreal maxY;
} UmapGeoRect;
//数据库字段类型,只设计文本和数值
typedef enum
{
    UMapN,
    UMapT,
} DataType;

//图层数据结构,就只有字段名和这字段类型了
typedef struct
{
    QString name;
    DataType type;
} Format;

} //namespace
#endif // UMAPCONTROL_GLOBAL_H
