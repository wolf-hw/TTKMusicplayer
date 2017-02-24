#include "musicdownloadquerybdplaylistthread.h"
#include "musictime.h"
#///QJson import
#include "qjson/parser.h"

MusicDownLoadQueryBDPlaylistThread::MusicDownLoadQueryBDPlaylistThread(QObject *parent)
    : MusicDownLoadQueryThreadAbstract(parent)
{
    m_queryServer = "Baidu";
}

QString MusicDownLoadQueryBDPlaylistThread::getClassName()
{
    return staticMetaObject.className();
}

void MusicDownLoadQueryBDPlaylistThread::startSearchSong(QueryType type, const QString &playlist)
{
    if(type == MusicQuery)
    {
        startSearchSong(playlist);
    }
    else
    {
        startSearchSongAll();
    }
}

void MusicDownLoadQueryBDPlaylistThread::startSearchSongAll()
{
    QUrl musicUrl = MusicCryptographicHash::decryptData(BD_PLAYLIST_URL, URL_KEY);

    if(m_reply)
    {
        m_reply->deleteLater();
        m_reply = nullptr;
    }

    QNetworkRequest request;
    request.setUrl(musicUrl);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
#ifndef QT_NO_SSL
    QSslConfiguration sslConfig = request.sslConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(sslConfig);
#endif
    m_reply = m_manager->get( request );
    connect(m_reply, SIGNAL(finished()), SLOT(downLoadFinished()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)),
                     SLOT(replyError(QNetworkReply::NetworkError)));
}

void MusicDownLoadQueryBDPlaylistThread::startSearchSong(const QString &playlist)
{
    QUrl musicUrl =  MusicCryptographicHash::decryptData(BD_PLAYLIST_ATTR_URL, URL_KEY).arg(playlist);

    QNetworkRequest request;
    request.setUrl(musicUrl);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
#ifndef QT_NO_SSL
    QSslConfiguration sslConfig = request.sslConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(sslConfig);
#endif
    QNetworkReply *reply = m_manager->get(request);
    connect(reply, SIGNAL(finished()), SLOT(getDetailsFinished()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(replyError(QNetworkReply::NetworkError)));
}

void MusicDownLoadQueryBDPlaylistThread::downLoadFinished()
{
    if(m_reply == nullptr)
    {
        deleteAll();
        return;
    }

    emit clearAllItems();      ///Clear origin items
    m_musicSongInfos.clear();  ///Empty the last search to songsInfo

    if(m_reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = m_reply->readAll(); ///Get all the data obtained by request

        QJson::Parser parser;
        bool ok;
        QVariant data = parser.parse(bytes, &ok);
        if(ok)
        {
            QVariantMap value = data.toMap();
            if(value["error_code"].toInt() == 22000 && value.contains("content"))
            {
                QVariantList datas = value["content"].toList();
                foreach(const QVariant &var, datas)
                {
                    if(var.isNull())
                    {
                        continue;
                    }

                    value = var.toMap();
                    MusicPlaylistItem item;
                    item.m_coverUrl = value["pic_w300"].toString();
                    item.m_id = value["listid"].toString();
                    item.m_name = value["title"].toString();
                    item.m_playCount = value["listenum"].toString();
                    item.m_description = value["desc"].toString();
                    item.m_updateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd");
                    item.m_nickname = "Greedysky";
                    item.m_tags = value["tag"].toString().replace(",", "|");

                    emit createPlaylistItems(item);
                }
            }
        }
    }

//    emit downLoadDataChanged(QString());
    deleteAll();
}

void MusicDownLoadQueryBDPlaylistThread::getDetailsFinished()
{
    QNetworkReply *reply = MObject_cast(QNetworkReply*, QObject::sender());

    emit clearAllItems();      ///Clear origin items
    m_musicSongInfos.clear();  ///Empty the last search to songsInfo

    if(reply && reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();

        QJson::Parser parser;
        bool ok;
        QVariant data = parser.parse(bytes, &ok);
        if(ok)
        {
            QVariantMap value = data.toMap();
            if(value["error_code"].toInt() == 22000 && value.contains("content"))
            {
                QVariantList datas = value["content"].toList();
                foreach(const QVariant &var, datas)
                {
                    if(var.isNull())
                    {
                        continue;
                    }

                    value = var.toMap();
                    MusicObject::MusicSongInfomation musicInfo;
                    musicInfo.m_singerName = value["author"].toString();
                    musicInfo.m_songName = value["title"].toString();
                    musicInfo.m_timeLength = "-";

                    musicInfo.m_songId = value["song_id"].toString();
                    musicInfo.m_artistId = value["ting_uid"].toString();
                    musicInfo.m_albumId = value["album_id"].toString();

//                    musicInfo.m_lrcUrl = value["lrclink"].toString();
//                    musicInfo.m_smallPicUrl = value["pic_small"].toString().replace(",w_90", ",w_500");

                    readFromMusicSongAttribute(&musicInfo, m_manager, value["all_rate"].toString(),
                                               m_searchQuality, m_queryAllRecords);
                    if(musicInfo.m_songAttrs.isEmpty())
                    {
                        continue;
                    }

                    MusicSearchedItem item;
                    item.m_songname = musicInfo.m_songName;
                    item.m_artistname = musicInfo.m_singerName;
                    item.m_time = musicInfo.m_songAttrs.first().m_duration;
                    item.m_type = mapQueryServerString();
                    emit createSearchedItems(item);

                    m_musicSongInfos << musicInfo;
                }
            }
        }
    }

    emit downLoadDataChanged(QString());
}
