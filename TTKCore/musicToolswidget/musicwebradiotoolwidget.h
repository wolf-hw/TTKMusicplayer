#ifndef MUSICWEBRADIOTOOLWIDGET_H
#define MUSICWEBRADIOTOOLWIDGET_H

/* =================================================
 * This file is part of the TTK Music Player project
 * Copyright (c) 2014 - 2016 Greedysky Studio
 * All rights reserved!
 * Redistribution and use of the source code or any derivative
 * works are strictly forbiden.
   =================================================*/

#include <QPushButton>
#include <QStackedWidget>
#include "musicglobaldefine.h"

class MusicWebEntainRadioListView;

/*! @brief The class of the web radio tool widget.
 * @author Greedysky <greedysky@163.com>
 */
class MUSIC_TOOL_EXPORT MusicWebRadioToolWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MusicWebRadioToolWidget(QWidget *parent = 0);
    /*!
     * Object contsructor.
     */
    ~MusicWebRadioToolWidget();

private Q_SLOTS:
    void buttonClicked(int index);
    /*!
     * Set diff radio to show by index.
     */

protected:
    QStackedWidget *m_stackedWidget;
    QPushButton *m_netRadioButton1,*m_netRadioButton2;
    MusicWebEntainRadioListView *m_musicWebRadioListView;

};

#endif // MUSICWEBRADIOTOOLWIDGET_H
