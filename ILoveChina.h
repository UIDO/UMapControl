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

#ifndef ILOVECHINA_H
#define ILOVECHINA_H

#include <QtCore>
#include <QtMath>

#include "umapcontrol_global.h"

namespace UM {
class ILoveChina;
}

/*
 * 这个东西嘛,大家都知道在天朝地图偏移问题了,虽然有坑但不影响我爱中国是吧?所以还是叫ILoveChina,虽然代码不是我写的!
 * 说白了就是用来把WGS坐标和火星坐标(gcj)相互调教的,可以不用理它
 * */

class UMAPCONTROLSHARED_EXPORT ILoveChina
{
public:
    ILoveChina();
    /*
     * 世界WGS坐标转火星坐标
     * */
    static QPointF wgs84TOgcj02(QPointF wgs);
    /*
     * 火星坐标转世界WGS坐标
     * */
    static QPointF gcj02Towgs84(QPointF gcj);
    /*
     * 这个先不管它,可能以后我要用到,先留着
     * */
    static bool DelDir(const QString &path);

private:
    static bool outOfChina(double lon, double lat);
    static double transformLat(double x, double y);
    static double transformLon(double x, double y);
    static QPointF delta(double lon, double lat);
};

#endif // ILOVECHINA_H
