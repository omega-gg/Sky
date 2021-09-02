//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkGui.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.

    - Private License Usage:
    Sky kit licensees holding valid private licenses may use this file in accordance with the
    private license agreement provided with the Software or, alternatively, in accordance with the
    terms contained in written agreement between you and Sky kit authors. For further information
    contact us at contact@omega.gg.
*/
//=================================================================================================

#include "WTabTrack.h"

#ifndef SK_NO_TABTRACK

// Qt includes
#include <QDir>
#include <QXmlStreamWriter>
#include <qtlockedfile>

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>
#include <WControllerXml>
#include <WControllerPlaylist>
#include <WAbstractTabs>
#include <WAbstractThreadAction>
#include <WPlaylist>
#ifndef SK_NO_PLAYER
#include <WDeclarativePlayer>
#endif
#include <WPixmapCache>

// Private includes
#include <private/WBookmarkTrack_p>

// Namespaces
using namespace QtLP_Private;

//-------------------------------------------------------------------------------------------------
// Static variables

static const int TABTRACK_MAX = 8;

//=================================================================================================
// WTabTrackWrite and WTabTrackWriteReply
//=================================================================================================

class WTabTrackWrite : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WTabTrackWrite(WTabTrackPrivate * data)
    {
        this->data = data;

        name    = sk->name      ();
        version = sk->versionSky();
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    WTabTrackPrivate * data;

    QString path;

    QString name;
    QString version;

    int currentIndex;

    QList<WTabTrackDataBookmark> dataBookmarks;

#ifndef SK_NO_PLAYER
    QHash<int, QPair<QString, QPixmap> > videoShots;
#endif
};

//-------------------------------------------------------------------------------------------------

class WTabTrackWriteReply : public WAbstractThreadReply
{
    Q_OBJECT

public:
    WTabTrackWriteReply(WTabTrackPrivate * data)
    {
        this->data = data;
    }

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

public: // Variables
    WTabTrackPrivate * data;

    QHash<int, QString> paths;
};

//=================================================================================================
// WTabTrackWrite
//=================================================================================================

/* virtual */ WAbstractThreadReply * WTabTrackWrite::createReply() const
{
    return new WTabTrackWriteReply(data);
}

/* virtual */ bool WTabTrackWrite::run()
{
    QByteArray data;

    QXmlStreamWriter stream(&data);

    stream.setAutoFormatting(true);

    stream.writeStartDocument();

    stream.writeStartElement(name);

    stream.writeTextElement("version", version);

    stream.writeStartElement("tabTrack");

    stream.writeTextElement("currentIndex", QString::number(currentIndex));

    stream.writeStartElement("bookmarks");

    foreach (const WTabTrackDataBookmark & data, dataBookmarks)
    {
        stream.writeStartElement("bookmark");

        stream.writeTextElement("id", QString::number(data.id));

        stream.writeStartElement("idPlaylist");

        foreach (int id, data.idPlaylist)
        {
            stream.writeTextElement("id", QString::number(id));
        }

        stream.writeEndElement(); // idPlaylist

        stream.writeTextElement("idTrack", QString::number(data.idTrack));

        stream.writeTextElement("state", QString::number(data.state));

        stream.writeTextElement("source", data.source);

        stream.writeTextElement("title", data.title);
        stream.writeTextElement("cover", data.cover);

        stream.writeTextElement("author", data.author);
        stream.writeTextElement("feed",   data.feed);

        stream.writeTextElement("duration", QString::number(data.duration));

        stream.writeTextElement("date", Sk::dateToStringNumber(data.date));

        stream.writeTextElement("quality", QString::number(data.quality));

        stream.writeTextElement("videoShot", data.videoShot);

        stream.writeTextElement("currentTime", QString::number(data.currentTime));

        stream.writeTextElement("subtitle", data.subtitle);

        stream.writeEndElement(); // bookmark
    }

    stream.writeEndElement(); // bookmarks
    stream.writeEndElement(); // tabTrack
    stream.writeEndElement(); // name

    stream.writeEndDocument();

    WControllerFile::writeFile(path, data);

#ifndef SK_NO_PLAYER
    if (videoShots.count())
    {
        WTabTrackWriteReply * reply = qobject_cast<WTabTrackWriteReply *> (this->reply());

        QHashIterator<int, QPair<QString, QPixmap> > i(videoShots);

        while (i.hasNext())
        {
            i.next();

            const QString & fileName = i.value().first;
            const QPixmap & pixmap   = i.value().second;

            pixmap.save(fileName, "jpg");

            reply->paths.insert(i.key(), fileName);
        }
    }
#endif

    qDebug("TAB SAVED");

    return true;
}

//=================================================================================================
// WTabTrackWriteReply
//=================================================================================================

/* virtual */ void WTabTrackWriteReply::onCompleted(bool ok)
{
#ifndef SK_NO_PLAYER
    if (paths.count())
    {
        QHashIterator<int, QString> i(paths);

        while (i.hasNext())
        {
            i.next();

            WBookmarkTrack * bookmark = data->bookmarkFromId(i.key());

            const QString & path = i.value();

            if (bookmark)
            {
                data->setVideoShot(bookmark, WControllerFile::fileUrl(path));
            }

            WPixmapCache::unregisterPixmap(path);
        }
    }
#endif

    data->setSaved(ok);
}

//=================================================================================================
// WTabTrackRead and WTabTrackReadReply
//=================================================================================================

class WTabTrackReadReply;

class WTabTrackRead : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WTabTrackRead(WTabTrackPrivate * data)
    {
        this->data = data;
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

private: // Functions
    bool load(QXmlStreamReader * stream, WTabTrackReadReply * reply);

public: // Variables
    WTabTrackPrivate * data;

    QString path;
};

class WTabTrackReadReply : public WAbstractThreadReply
{
    Q_OBJECT

public:
    WTabTrackReadReply(WTabTrackPrivate * data)
    {
        this->data = data;
    }

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

public: // Variables
    WTabTrackPrivate * data;

    int currentIndex;

    QList<WTabTrackDataBookmark> dataBookmarks;
};


//=================================================================================================
// WTabTrackRead
//=================================================================================================

/* virtual */ WAbstractThreadReply * WTabTrackRead::createReply() const
{
    return new WTabTrackReadReply(data);
}

/* virtual */ bool WTabTrackRead::run()
{
    WTabTrackReadReply * reply = qobject_cast<WTabTrackReadReply *> (this->reply());

    QByteArray data = WControllerFile::readFile(path);

    QXmlStreamReader stream(data);

    if (load(&stream, reply) == false)
    {
        qWarning("WTabTrackRead::run: Invalid file %s.", path.C_STR);

        return false;
    }

    qDebug("TAB LOADED");

    return true;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

bool WTabTrackRead::load(QXmlStreamReader * stream, WTabTrackReadReply * reply)
{
    if (WControllerXml::readNextStartElement(stream, "currentIndex") == false) return false;

    reply->currentIndex = WControllerXml::readNextInt(stream);

    while (WControllerXml::readNextStartElement(stream, "bookmark"))
    {
        WTabTrackDataBookmark data;

        //-----------------------------------------------------------------------------------------
        // id

        if (WControllerXml::readNextStartElement(stream, "id") == false) return false;

        data.id = WControllerXml::readNextInt(stream);

        //-----------------------------------------------------------------------------------------
        // idPlaylist

        if (WControllerXml::readNextStartElement(stream, "idPlaylist") == false) return false;

        stream->readNextStartElement();

        while (stream->name() == "id")
        {
            int id = WControllerXml::readNextInt(stream);

            if (id < 0) return false;

            data.idPlaylist.append(id);

            stream->readNextStartElement();
            stream->readNextStartElement();
        }

        //-----------------------------------------------------------------------------------------
        // idTrack

        if (WControllerXml::readNextStartElement(stream, "idTrack") == false) return false;

        data.idTrack = WControllerXml::readNextInt(stream);

        //-----------------------------------------------------------------------------------------
        // state

        if (WControllerXml::readNextStartElement(stream, "state") == false) return false;

        data.state = static_cast<WTrack::State> (WControllerXml::readNextInt(stream));

        //-----------------------------------------------------------------------------------------
        // source

        if (WControllerXml::readNextStartElement(stream, "source") == false) return false;

        data.source = WControllerXml::readNextString(stream);

        //-----------------------------------------------------------------------------------------
        // title

        if (WControllerXml::readNextStartElement(stream, "title") == false) return false;

        data.title = WControllerXml::readNextString(stream);

        //-----------------------------------------------------------------------------------------
        // cover

        if (WControllerXml::readNextStartElement(stream, "cover") == false) return false;

        data.cover = WControllerXml::readNextString(stream);

        //-----------------------------------------------------------------------------------------
        // author

        if (WControllerXml::readNextStartElement(stream, "author") == false) return false;

        data.author = WControllerXml::readNextString(stream);

        //-----------------------------------------------------------------------------------------
        // feed

        if (WControllerXml::readNextStartElement(stream, "feed") == false) return false;

        data.feed = WControllerXml::readNextString(stream);

        //-----------------------------------------------------------------------------------------
        // duration

        if (WControllerXml::readNextStartElement(stream, "duration") == false) return false;

        data.duration = WControllerXml::readNextInt(stream);

        //-----------------------------------------------------------------------------------------
        // date

        if (WControllerXml::readNextStartElement(stream, "date") == false) return false;

        data.date = WControllerXml::readNextDate(stream);

        //-----------------------------------------------------------------------------------------
        // quality

        if (WControllerXml::readNextStartElement(stream, "quality") == false) return false;

        int quality = WControllerXml::readNextInt(stream);

        data.quality = static_cast<WAbstractBackend::Quality> (quality);

        //-----------------------------------------------------------------------------------------
        // videoShot

        if (WControllerXml::readNextStartElement(stream, "videoShot") == false) return false;

        data.videoShot = WControllerXml::readNextString(stream);

        //-----------------------------------------------------------------------------------------
        // currentTime

        if (WControllerXml::readNextStartElement(stream, "currentTime") == false) return false;

        data.currentTime = WControllerXml::readNextInt(stream);

        //-----------------------------------------------------------------------------------------
        // subtitle

        if (WControllerXml::readNextStartElement(stream, "subtitle") == false) return false;

        data.subtitle = WControllerXml::readNextString(stream);

        //-----------------------------------------------------------------------------------------

        reply->dataBookmarks.append(data);
    }

    return true;
}

//=================================================================================================
// WTabTrackReadReply
//=================================================================================================

/* virtual */ void WTabTrackReadReply::onCompleted(bool ok)
{
    WTabTrack * q = data->q_func();

    bool saveEnabled = q->saveEnabled();

    q->setSaveEnabled(false);

    if (ok)
    {
        data->loadBookmarks(dataBookmarks);

        q->setCurrentIndex(currentIndex);
    }

    q->setSaveEnabled(saveEnabled);

    data->setLoaded(ok);
}

//=================================================================================================
// WTabTrackPrivate
//=================================================================================================

WTabTrackPrivate::WTabTrackPrivate(WTabTrack * p) : WAbstractTabPrivate(p) {}

/* virtual */ WTabTrackPrivate::~WTabTrackPrivate()
{
    Q_Q(WTabTrack);

    if (playlist) playlist->tryDelete();

    W_GET_CONTROLLER(WControllerPlaylist, controller);

    if (controller) controller->d_func()->unregisterTab(q);
}

//-------------------------------------------------------------------------------------------------

void WTabTrackPrivate::init()
{
    Q_Q(WTabTrack);

    playlist = NULL;

#ifndef SK_NO_PLAYER
    player = NULL;
#endif

    currentBookmark = NULL;

    stackEnabled = false;

    maxCount = TABTRACK_MAX;

    QObject::connect(q, SIGNAL(playlistChanged()), q, SIGNAL(playlistUpdated()));

    QObject::connect(q, SIGNAL(currentBookmarkChanged()), q, SIGNAL(currentBookmarkUpdated()));

    QObject::connect(q, SIGNAL(focusChanged()), q, SLOT(onFocusChanged()));

    QObject::connect(wControllerPlaylist, SIGNAL(filesCleared(const QList<int> &)),
                     q,                   SLOT(onFilesUpdated(const QList<int> &)));

    QObject::connect(wControllerPlaylist, SIGNAL(filesDeleted(const QList<int> &)),
                     q,                   SLOT(onFilesUpdated(const QList<int> &)));

    wControllerPlaylist->d_func()->registerTab(q);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

bool WTabTrackPrivate::checkId(const QList<int> & idA, const QList<int> & idB)
{
    int count = idA.count();

    if (count > idB.count()) return false;

    for (int i = 0; i < count; i++)
    {
        if (idA.at(i) != idB.at(i)) return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

void WTabTrackPrivate::loadBookmarks(const QList<WTabTrackDataBookmark> & bookmarks)
{
    Q_Q(WTabTrack);

    if (this->bookmarks.count())
    {
        if (hasFocus && playlist)
        {
            playlist->unselectTracks();

#ifndef SK_NO_PLAYER
            if (player) player->stop();
#endif
        }

        this->bookmarks.clear();

        ids   .clear();
        idHash.clear();
    }

    foreach (const WTabTrackDataBookmark & bookmark, bookmarks)
    {
        WBookmarkTrack bookmarkTrack;

        WBookmarkTrackPrivate * p = bookmarkTrack.d_func();

        p->id = bookmark.id;

        p->idPlaylist = bookmark.idPlaylist;
        p->idTrack    = bookmark.idTrack;

        p->state = bookmark.state;

        p->source = bookmark.source;

        p->title = bookmark.title;
        p->cover = bookmark.cover;

        p->author = bookmark.author;
        p->feed   = bookmark.feed;

        p->duration = bookmark.duration;

        p->date = bookmark.date;

        p->quality = bookmark.quality;

        p->videoShot   = bookmark.videoShot;
        p->currentTime = bookmark.currentTime;

        p->subtitle = bookmark.subtitle;

        this->bookmarks.append(bookmarkTrack);

        WBookmarkTrack * lastBookmark = &(this->bookmarks.last());

        lastBookmark->d_func()->setParentTab(q);

        ids.insertId(bookmark.id);
    }

    emit q->countChanged();
}

//-------------------------------------------------------------------------------------------------

void WTabTrackPrivate::removeBookmark(int index)
{
    WBookmarkTrack * bookmark = &(bookmarks[index]);

    QString videoShot = bookmark->videoShot();

    if (videoShot.isEmpty() == false && videoShot.startsWith("image:///") == false)
    {
        videoShot.remove("file:///");

        wControllerFile->startDeleteFile(videoShot);
    }

    int id = bookmark->id();

    ids.removeOne(id);

    idHash.remove(id);

    bookmarks.removeAt(index);

    if (bookmarks.isEmpty())
    {
        Q_Q(WTabTrack);

        q->deleteFolder();
    }
}

//-------------------------------------------------------------------------------------------------

WBookmarkTrack * WTabTrackPrivate::bookmarkFromId(int id)
{
    for (int i = 0; i < bookmarks.count(); i++)
    {
        if (bookmarks.at(i).id() == id)
        {
            return &(bookmarks[i]);
        }
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------

void WTabTrackPrivate::setCurrentBookmark(WBookmarkTrack * bookmark)
{
    Q_Q(WTabTrack);

    saveState();

    currentBookmark = bookmark;

    if (bookmark) applyPlaylist(bookmark);

    emit q->currentBookmarkChanged();

    q->save();
}

//-------------------------------------------------------------------------------------------------

bool WTabTrackPrivate::setPlaylist(WPlaylist * playlist)
{
    Q_Q(WTabTrack);

    if (this->playlist == playlist) return false;

    if (this->playlist)
    {
#ifndef SK_NO_PLAYER
        if (player)
        {
            this->playlist->setCurrentTime(player->currentTime());
        }
#endif

        clearPlaylist();
    }

    this->playlist = playlist;

    if (playlist)
    {
        QObject::connect(playlist, SIGNAL(playlistUpdated()), q, SIGNAL(playlistUpdated()));

        QObject::connect(q, SIGNAL(currentBookmarkChanged()), q, SIGNAL(playlistUpdated()));

        if (hasFocus)
        {
            QObject::connect(playlist, SIGNAL(currentTrackChanged()),
                             q,        SLOT(onCurrentTrackChanged()));
        }

        QObject::connect(playlist, SIGNAL(destroyed()), q, SLOT(onPlaylistDestroyed()));

        playlist->addDeleteLock();
    }

    emit q->playlistChanged();

    if (playlist)
    {
         return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

void WTabTrackPrivate::applyPlaylist(WBookmarkTrack * bookmark)
{
    WBookmarkTrackPrivate * p = bookmark->d_func();

    WPlaylist * playlist = p->playlist;

    if (playlist)
    {
        const WTrack * track = p->track;

        if (hasFocus && track)
        {
            playlist->setCurrentTrackPointer(track);
        }

        setPlaylist(playlist);
    }
    else
    {
        const QList<int> & idPlaylist = p->idPlaylist;

        WLibraryItem * item = wControllerPlaylist->getLibraryItem(idPlaylist);

        if (item == NULL)
        {
            setPlaylist(NULL);

            wControllerPlaylist->loadLibraryItem(idPlaylist);
        }
        else p->setPlaylist(item->toPlaylist());
    }
}

void WTabTrackPrivate::updatePlaylist(WBookmarkTrack * bookmark)
{
    WBookmarkTrackPrivate * p = bookmark->d_func();

    WPlaylist * playlist = p->playlist;

    if (playlist)
    {
        if (hasFocus)
        {
            const WTrack * track = p->track;

            if (track)
            {
                playlist->setCurrentTrackPointer(track);
            }
        }

        setPlaylist(playlist);
    }
    else setPlaylist(NULL);
}

//-------------------------------------------------------------------------------------------------

void WTabTrackPrivate::clearPlaylist()
{
    Q_Q(WTabTrack);

    QObject::disconnect(playlist, 0, q, 0);

    QObject::disconnect(q, SIGNAL(currentBookmarkChanged()), q, SIGNAL(playlistUpdated()));

    playlist->tryDelete();
}

//-------------------------------------------------------------------------------------------------

void WTabTrackPrivate::setTrack(const WTrack * track)
{
    if (track == NULL) return;

    Q_Q(WTabTrack);

    if (currentBookmark)
    {
        WBookmarkTrackPrivate * p = currentBookmark->d_func();

        if (p->track == track) return;

        if (stackEnabled == false && p->currentTime == -1)
        {
            currentBookmark->setTrack(*track);

            emit q->currentBookmarkChanged();

            return;
        }
    }

    WBookmarkTrack bookmark(*track);

    q->pushBookmark(bookmark);
}

//-------------------------------------------------------------------------------------------------

void WTabTrackPrivate::setVideoShot(WBookmarkTrack * bookmark, const QString & url)
{
    WBookmarkTrackPrivate * p = bookmark->d_func();

    if (p->videoShot == url) return;

    Q_Q(WTabTrack);

    p->videoShot = url;

    emit q->currentBookmarkUpdated();
}

//-------------------------------------------------------------------------------------------------

void WTabTrackPrivate::saveState()
{
#ifndef SK_NO_PLAYER
    if (currentBookmark == NULL
        ||
        player == NULL || player->hasStarted() == false || player->isResuming()) return;

    Q_Q(WTabTrack);

    int time = player->currentTime();

    WBookmarkTrackPrivate * p = currentBookmark->d_func();

    p->currentTime = time;

    WPlaylist * playlist = p->playlist;

    if (playlist) playlist->setCurrentTime(time);

    if (player->outputActive() == WAbstractBackend::OutputAudio)
    {
        setVideoShot(currentBookmark, p->cover);
    }
    else if (time != -1)
    {
        int id = p->id;

        QPixmap pixmap = QPixmap::fromImage(player->getFrame());

        if (pixmap.height() > 1080)
        {
            pixmap = pixmap.scaledToHeight(1080, Qt::SmoothTransformation);
        }

        QString fileName = q->getFolderPath() + "/" + QString::number(id) + ".jpg";

        videoShots.insert(id, QPair<QString, QPixmap> (fileName, pixmap));

        WPixmapCache::registerPixmap(fileName, pixmap);

        setVideoShot(currentBookmark, "image:///" + fileName);
    }
    else emit q->currentBookmarkUpdated();

    q->save();
#endif
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WTabTrackPrivate::onFocusChanged()
{
    if (playlist == NULL) return;

    Q_Q(WTabTrack);

    if (hasFocus)
    {
        if (currentBookmark)
        {
            const WTrack * track = currentBookmark->track();

            playlist->setCurrentTrackPointer(track);
        }
        else playlist->setCurrentTrackPointer(NULL);

        QObject::connect(playlist, SIGNAL(currentTrackChanged()),
                         q,        SLOT(onCurrentTrackChanged()));
    }
    else QObject::disconnect(playlist, SIGNAL(currentTrackChanged()),
                             q,        SLOT(onCurrentTrackChanged()));
}

//-------------------------------------------------------------------------------------------------

void WTabTrackPrivate::onCurrentTrackChanged()
{
    const WTrack * track = playlist->currentTrackPointer();

    setTrack(track);
}

//-------------------------------------------------------------------------------------------------

void WTabTrackPrivate::onRegisterItemId(WLibraryItem * item, const QList<int> & idFull)
{
    if (currentBookmark == NULL || playlist) return;

    WBookmarkTrackPrivate * p = currentBookmark->d_func();

    const QList<int> & idPlaylist = p->idPlaylist;

    if (idPlaylist.isEmpty()) return;

    if (item->isPlaylist())
    {
        if (idPlaylist == idFull)
        {
            Q_Q(WTabTrack);

            p->setPlaylist(item->toPlaylist());

            emit q->currentBookmarkUpdated();
        }
    }
    else if (idFull.count() == 1 && idPlaylist.first() == idFull.first())
    {
        wControllerPlaylist->loadLibraryItem(idPlaylist);
    }
}

//-------------------------------------------------------------------------------------------------

void WTabTrackPrivate::onFilesUpdated(const QList<int> & idFull)
{
    bool save = false;

    for (int i = 0; i < bookmarks.count(); i++)
    {
        WBookmarkTrack * bookmark = &(bookmarks[i]);

        WBookmarkTrackPrivate * p = bookmark->d_func();

        if (checkId(idFull, p->idPlaylist))
        {
            WPlaylist * playlist = p->playlist;

            if (playlist)
            {
                playlist->unregisterWatcher(bookmark);
            }

            p->clearTrack();

            save = true;
        }
    }

    if (currentBookmark && currentBookmark->d_func()->playlist == NULL)
    {
        setPlaylist(NULL);
    }

    if (save)
    {
        Q_Q(WTabTrack);

        q->save();
    }
}

//-------------------------------------------------------------------------------------------------

void WTabTrackPrivate::onPlaylistDestroyed()
{
    Q_Q(WTabTrack);

    playlist = NULL;

    emit q->playlistChanged();
}

//=================================================================================================
// WTabTrack
//=================================================================================================

/* explicit */ WTabTrack::WTabTrack(WAbstractTabs * parent)
    : WAbstractTab(new WTabTrackPrivate(this), parent)
{
    Q_D(WTabTrack); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WTabTrack::pushBookmark(const WBookmarkTrack & bookmark)
{
    Q_D(WTabTrack);

    if (bookmark.d_func()->id != -1)
    {
        qWarning("WTabTrack::pushBookmark: Bookmark already has an id.");

        return;
    }

    d->saveState();

    int index;

    if (d->currentBookmark)
    {
         index = currentIndex() + 1;
    }
    else index = d->bookmarks.count();

    d->bookmarks.insert(index, bookmark);

    d->currentBookmark = &(d->bookmarks[index]);

    WBookmarkTrackPrivate * p = d->currentBookmark->d_func();

    p->setParentTab(this);

    p->id = d->ids.generateId();

    d->applyPlaylist(d->currentBookmark);

    int count = d->bookmarks.count();

    index++;

    while (index < d->bookmarks.count())
    {
        d->removeBookmark(index);
    }

    while (d->bookmarks.count() > 1 && d->bookmarks.count() > d->maxCount)
    {
        d->removeBookmark(0);
    }

    if (count != d->bookmarks.count())
    {
        emit countChanged();
    }

    emit currentBookmarkChanged();

    save();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WTabTrack::clearBookmarks()
{
    Q_D(WTabTrack);

    abortAll();

    deleteFolder(true);

    if (d->bookmarks.count())
    {
        if (d->playlist)
        {
            if (d->hasFocus)
            {
                d->playlist->unselectTracks();
            }

#ifndef SK_NO_PLAYER
            if (d->player)
            {
                d->playlist->setCurrentTime(d->player->currentTime());

                d->player->stop();
            }
#endif

            d->clearPlaylist();

            d->playlist = NULL;

            emit playlistChanged();
        }

        d->setCurrentBookmark(NULL);

        d->bookmarks.clear();

        d->ids   .clear();
        d->idHash.clear();

        emit countChanged();
    }

    save();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WTabTrack::setPreviousBookmark()
{
    int index = currentIndex();

    if (index != -1)
    {
        setCurrentIndex(index - 1);
    }
}

/* Q_INVOKABLE */ void WTabTrack::setNextBookmark()
{
    int index = currentIndex();

    if (index != -1)
    {
        setCurrentIndex(index + 1);
    }
}

//---------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WTabTrack::setPreviousTrack(bool cycle)
{
    Q_D(WTabTrack);

    if (d->playlist == NULL) return;

    int index = trackIndex();

    if (index == -1) return;

    if (cycle)
    {
        if (d->playlist->isFeed())
        {
            if (index != d->playlist->count() - 1)
            {
                 index++;
            }
            else index = 0;
        }
        else
        {
            if (index != 0)
            {
                 index--;
            }
            else index = d->playlist->count() - 1;
        }
    }
    else if (d->playlist->isFeed())
    {
        index++;

        if (index == d->playlist->count()) return;
    }
    else
    {
        index--;

        if (index == -1) return;
    }

    setCurrentTime(-1);

    const WTrack * track = d->playlist->trackPointerAt(index);

    setCurrentTrackPointer(track);
}

/* Q_INVOKABLE */ void WTabTrack::setNextTrack(bool cycle)
{
    Q_D(WTabTrack);

    if (d->playlist == NULL) return;

    int index = trackIndex();

    if (index == -1) return;

    if (cycle)
    {
        if (d->playlist->isFeed())
        {
            if (index != 0)
            {
                 index--;
            }
            else index = d->playlist->count() - 1;
        }
        else
        {
            if (index != d->playlist->count() - 1)
            {
                 index++;
            }
            else index = 0;
        }
    }
    else if (d->playlist->isFeed())
    {
        index--;

        if (index == -1) return;
    }
    else
    {
        index++;

        if (index == d->playlist->count()) return;
    }

    setCurrentTime(-1);

    const WTrack * track = d->playlist->trackPointerAt(index);

    setCurrentTrackPointer(track);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ const WTrack * WTabTrack::currentTrackPointer() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->track();
    }
    else return NULL;
}

/* Q_INVOKABLE */ void WTabTrack::setCurrentTrackPointer(const WTrack * track)
{
    Q_D(WTabTrack);

    bool oldStackEnabled = d->stackEnabled;

    d->stackEnabled = false;

    if (d->hasFocus && d->playlist)
    {
         d->playlist->setCurrentTrackPointer(track);
    }
    else d->setTrack(track);

    d->stackEnabled = oldStackEnabled;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WTabTrack::copyTrackTo(WPlaylist * destination, int to)
{
    Q_ASSERT(destination);

    Q_D(WTabTrack);

    if (d->currentBookmark == NULL) return;

    WTrack track = d->currentBookmark->toTrack();

    if (track.isValid() == false) return;

    destination->insertTrack(to, track);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WTabTrack::updateBookmark()
{
    emit currentBookmarkUpdated();
}

//-------------------------------------------------------------------------------------------------
// WLocalObject reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WTabTrack::getParentPath() const
{
    Q_D(const WTabTrack);

    if (d->parentTabs)
    {
         return d->parentTabs->getParentPath() + "/" + QString::number(d->parentTabs->id());
    }
    else return wControllerPlaylist->pathStorageTabs();
}

//-------------------------------------------------------------------------------------------------
// Protected WLocalObject reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ WAbstractThreadAction * WTabTrack::onSave(const QString & path)
{
    Q_D(WTabTrack);

    WTabTrackWrite * action = new WTabTrackWrite(d);

    action->path = path;

    action->currentIndex = currentIndex();

    foreach (const WBookmarkTrack & bookmark, d->bookmarks)
    {
        const WBookmarkTrackPrivate * p = bookmark.d_func();

        WTabTrackDataBookmark data;

        data.id = p->id;

        data.idPlaylist = p->idPlaylist;
        data.idTrack    = p->idTrack;

        data.state = p->state;

        data.source = p->source;

        data.title = p->title;
        data.cover = p->cover;

        data.author = p->author;
        data.feed   = p->feed;

        data.duration = p->duration;

        data.date = p->date;

        data.quality = p->quality;

        QString videoShot = p->videoShot;

        videoShot.replace("image:///", "file:///");

        data.videoShot   = videoShot;
        data.currentTime = p->currentTime;

        data.subtitle = p->subtitle;

        action->dataBookmarks.append(data);
    }

#ifndef SK_NO_PLAYER
    action->videoShots = d->videoShots;

    d->videoShots.clear();
#endif

    return action;
}

/* virtual */ WAbstractThreadAction * WTabTrack::onLoad(const QString & path)
{
    Q_D(WTabTrack);

    WTabTrackRead * action = new WTabTrackRead(d);

    action->path = path;

    return action;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WTabTrack::hasFolder() const
{
    return true;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QList<WBookmarkTrack> WTabTrack::bookmarks() const
{
    Q_D(const WTabTrack); return d->bookmarks;
}

//-------------------------------------------------------------------------------------------------

int WTabTrack::currentIndex() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark == NULL) return -1;

    for (int i = 0; i < d->bookmarks.count(); i++)
    {
        if (d->currentBookmark == &(d->bookmarks.at(i)))
        {
            return i;
        }
    }

    return -1;
}

void WTabTrack::setCurrentIndex(int index)
{
    Q_D(WTabTrack);

    if (index < 0 || index >= d->bookmarks.count()) return;

    bool oldStackEnabled = d->stackEnabled;

    d->stackEnabled = false;

    WBookmarkTrack * bookmark = &(d->bookmarks[index]);

    d->setCurrentBookmark(bookmark);

    d->stackEnabled = oldStackEnabled;
}

//-------------------------------------------------------------------------------------------------

const WBookmarkTrack * WTabTrack::currentBookmark() const
{
    Q_D(const WTabTrack); return d->currentBookmark;
}

//-------------------------------------------------------------------------------------------------

WPlaylist * WTabTrack::playlist() const
{
    Q_D(const WTabTrack); return d->playlist;
}

void WTabTrack::setPlaylist(WPlaylist * playlist)
{
    Q_D(WTabTrack);

    if (d->setPlaylist(playlist))
    {
        d->onCurrentTrackChanged();
    }
}

//-------------------------------------------------------------------------------------------------

#ifndef SK_NO_PLAYER

WDeclarativePlayer * WTabTrack::player() const
{
    Q_D(const WTabTrack); return d->player;
}

void WTabTrack::setPlayer(WDeclarativePlayer * player)
{
    Q_D(WTabTrack);

    if (d->player == player) return;

    if (player == NULL) d->saveState();

    d->player = player;

    emit playerChanged();
}

#endif

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WTabTrack::count() const
{
    Q_D(const WTabTrack); return d->bookmarks.count();
}

//-------------------------------------------------------------------------------------------------

int WTabTrack::trackIndex() const
{
    Q_D(const WTabTrack);

    if (d->playlist && d->currentBookmark)
    {
        const WTrack * track = d->currentBookmark->track();

        if (track)
        {
             return d->playlist->indexOf(track);
        }
        else return -1;
    }
    else return -1;
}

void WTabTrack::setTrackIndex(int index)
{
    Q_D(WTabTrack);

    if (d->playlist == NULL) return;

    const WTrack * track = d->playlist->trackPointerAt(index);

    if (track) setCurrentTrackPointer(track);
}

//-------------------------------------------------------------------------------------------------

bool WTabTrack::isDefault() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->isDefault();
    }
    else return false;
}

bool WTabTrack::isLoading() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->isLoading();
    }
    else return false;
}

bool WTabTrack::isLoaded() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->isLoaded();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

bool WTabTrack::isValid() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->isValid();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

bool WTabTrack::stackEnabled() const
{
    Q_D(const WTabTrack); return d->stackEnabled;
}

void WTabTrack::setStackEnabled(bool enabled)
{
    Q_D(WTabTrack);

    if (d->stackEnabled == enabled) return;

    d->stackEnabled = enabled;

    emit stackEnabledChanged();
}

//-------------------------------------------------------------------------------------------------

bool WTabTrack::hasPreviousBookmark() const
{
    int index = currentIndex();

    if (index > 0)
    {
         return true;
    }
    else return false;
}

bool WTabTrack::hasNextBookmark() const
{
    Q_D(const WTabTrack);

    int index = currentIndex();

    if (index != -1 && index < (d->bookmarks.count() - 1))
    {
         return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

bool WTabTrack::hasPreviousTrack() const
{
    Q_D(const WTabTrack);

    int index = trackIndex();

    if (index != -1)
    {
         return d->playlist->hasPreviousIndex(index);
    }
    else return false;
}

bool WTabTrack::hasNextTrack() const
{
    Q_D(const WTabTrack);

    int index = trackIndex();

    if (index != -1)
    {
         return d->playlist->hasNextIndex(index);
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

QList<int> WTabTrack::folderIds() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->folderIds();
    }
    else return QList<int>();
}

//-------------------------------------------------------------------------------------------------

QList<int> WTabTrack::idPlaylist() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->idPlaylist();
    }
    else return QList<int>();
}

int WTabTrack::idTrack() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->idTrack();
    }
    else return -1;
}

int WTabTrack::idFolderRoot() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->idFolderRoot();
    }
    else return -1;
}

//-------------------------------------------------------------------------------------------------

QVariantMap WTabTrack::trackData() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->toTrackData();
    }
    else return QVariantMap();
}

//-------------------------------------------------------------------------------------------------

WTrack::State WTabTrack::state() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->state();
    }
    else return WTrack::Default;
}

//-------------------------------------------------------------------------------------------------

QString WTabTrack::source() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->source();
    }
    else return QString();
}

//-------------------------------------------------------------------------------------------------

QString WTabTrack::title() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->title();
    }
    else return QString();
}

QString WTabTrack::cover() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->cover();
    }
    else return QString();
}

//-------------------------------------------------------------------------------------------------

QString WTabTrack::author() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->author();
    }
    else return QString();
}

QString WTabTrack::feed() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->feed();
    }
    else return QString();
}

//-------------------------------------------------------------------------------------------------

int WTabTrack::duration() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->duration();
    }
    else return -1;
}

void WTabTrack::setDuration(int msec)
{
    Q_D(WTabTrack);

    if (d->currentBookmark == NULL) return;

    WBookmarkTrackPrivate * p = d->currentBookmark->d_func();

    if (p->duration == msec) return;

    p->duration = msec;

    if (msec > 0 && p->playlist)
    {
        WPlaylist * playlist = p->playlist->toPlaylist();

        int index = playlist->indexOf(p->track);

        playlist->setTrackDuration(index, msec);
    }

    emit currentBookmarkUpdated();

    save();
}

//-------------------------------------------------------------------------------------------------

QDateTime WTabTrack::date() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->date();
    }
    else return QDateTime();
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Quality WTabTrack::quality() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->quality();
    }
    else return WAbstractBackend::QualityInvalid;
}

//-------------------------------------------------------------------------------------------------

QString WTabTrack::videoShot() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->videoShot();
    }
    else return QString();
}

QString WTabTrack::coverShot() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark == NULL)
    {
        return QString();
    }
    else if (d->currentBookmark->currentTime() != -1)
    {
         return d->currentBookmark->videoShot();
    }
    else return d->currentBookmark->cover();
}

//-------------------------------------------------------------------------------------------------

int WTabTrack::currentTime() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->currentTime();
    }
    else return -1;
}

void WTabTrack::setCurrentTime(int msec)
{
    Q_D(WTabTrack);

    if (d->currentBookmark == NULL) return;

    WBookmarkTrackPrivate * p = d->currentBookmark->d_func();

    if (p->currentTime == msec) return;

    p->currentTime = msec;

    if (msec == -1)
    {
        p->videoShot = QString();
    }

    emit currentBookmarkUpdated();

    save();
}

//-------------------------------------------------------------------------------------------------

QString WTabTrack::subtitle() const
{
    Q_D(const WTabTrack);

    if (d->currentBookmark)
    {
         return d->currentBookmark->subtitle();
    }
    else return QString();
}

void WTabTrack::setSubtitle(const QString & subtitle)
{
    Q_D(WTabTrack);

    if (d->currentBookmark == NULL) return;

    WBookmarkTrackPrivate * p = d->currentBookmark->d_func();

    if (p->subtitle == subtitle) return;

    p->subtitle = subtitle;

    emit currentBookmarkUpdated();

    save();
}

#endif // SK_NO_TABTRACK

#include "WTabTrack.moc"
