#ifndef MUSICWEBRADIOOBJECT_H
#define MUSICWEBRADIOOBJECT_H

/* =================================================
 * This file is part of the TTK Music Player project
 * Copyright (C) 2015 - 2018 Greedysky Studio

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; If not, see <http://www.gnu.org/licenses/>.
 ================================================= */

#include "musicobject.h"
#include "musicglobaldefine.h"

/*! @brief The class of the web radio object.
 * @author Greedysky <greedysky@163.com>
 */
class MUSIC_TOOLSET_EXPORT MusicWebRadioObject : public QObject
{
    Q_OBJECT
    TTK_DECLARE_MODULE(MusicWebRadioObject)
public:
    /*!
     * Object contsructor.
     */
    explicit MusicWebRadioObject(QObject *parent = nullptr);

    /*!
     * Start to run object.
     */
    void start();

public Q_SLOTS:
    /*!
     * Send recieved data from net.
     */
    void dataDownloadFinished();

};

#endif // MUSICWEBRADIOOBJECT_H
