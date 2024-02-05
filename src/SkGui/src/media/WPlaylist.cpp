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

#include "WPlaylist.h"

#ifndef SK_NO_PLAYLIST

// Qt includes
#include <QXmlStreamWriter>
#include <qtlockedfile>

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>
#include <WControllerXml>
#include <WControllerNetwork>
#include <WControllerPlaylist>

// Private includes
#include <private/WTrack_p>

// Private include
#include "WPlaylist_p.h"

// Namespaces
using namespace QtLP_Private;

//-------------------------------------------------------------------------------------------------
// Functions declarations

void WBackendPlaylist_patch(QString & data, const QString & api);

//-------------------------------------------------------------------------------------------------
// Static variables

static const int PLAYLIST_MAX = 500;

//=================================================================================================
// WPlaylistWatcher
//=================================================================================================

/* virtual */ void WPlaylistWatcher::beginTracksInsert(int, int) {}
/* virtual */ void WPlaylistWatcher::endTracksInsert  ()         {}

/* virtual */ void WPlaylistWatcher::beginTracksMove(int, int, int) {}
/* virtual */ void WPlaylistWatcher::endTracksMove  ()              {}

/* virtual */ void WPlaylistWatcher::beginTracksRemove(int, int) {}
/* virtual */ void WPlaylistWatcher::endTracksRemove  ()         {}

/* virtual */ void WPlaylistWatcher::trackUpdated(int) {}

/* virtual */ void WPlaylistWatcher::beginTracksClear() {}
/* virtual */ void WPlaylistWatcher::endTracksClear  () {}

/* virtual */ void WPlaylistWatcher::selectedTracksChanged(const QList<int> &) {}

/* virtual */ void WPlaylistWatcher::currentIndexChanged(int) {}

/* virtual */ void WPlaylistWatcher::playlistDestroyed() {}

//=================================================================================================
// WPlaylistWrite and WPlaylistWriteReply
//=================================================================================================

struct WThreadActionDataTrack
{
    int id;

    WTrack::Type type;

    WTrack::State state;

    QString source;

    QString title;
    QString cover;

    QString author;
    QString feed;

    int duration;

    QDateTime date;
};

class WPlaylistWrite : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WPlaylistWrite(WPlaylistPrivate * data)
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
    WPlaylistPrivate * data;

    QString path;

    QString name;
    QString version;

    QString source;

    QString title;
    QString cover;

    QString label;

    int currentIndex;
    int currentTime;

    qreal scrollValue;

    QList<WThreadActionDataTrack> dataTracks;
};

//-------------------------------------------------------------------------------------------------

class WPlaylistWriteReply : public WAbstractThreadReply
{
    Q_OBJECT

public:
    WPlaylistWriteReply(WPlaylistPrivate * data)
    {
        this->data = data;
    }

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

public: // Variables
    WPlaylistPrivate * data;
};

//=================================================================================================
// WPlaylistWrite
//=================================================================================================

/* virtual */ WAbstractThreadReply * WPlaylistWrite::createReply() const
{
    return new WPlaylistWriteReply(data);
}

/* virtual */ bool WPlaylistWrite::run()
{
    QByteArray data;

    QXmlStreamWriter stream(&data);

    stream.setAutoFormatting(true);

    stream.writeStartDocument();

    stream.writeStartElement(name);

    stream.writeTextElement("version", version);

    stream.writeStartElement("playlist");

    stream.writeTextElement("source", source);

    stream.writeTextElement("title", title);
    stream.writeTextElement("cover", cover);

    stream.writeTextElement("label", label);

    stream.writeTextElement("currentIndex", QString::number(currentIndex));
    stream.writeTextElement("currentTime",  QString::number(currentTime));

    stream.writeTextElement("scrollValue", QString::number(scrollValue));

    stream.writeStartElement("tracks");

    foreach (const WThreadActionDataTrack & data, dataTracks)
    {
        stream.writeStartElement("track");

        stream.writeTextElement("id", QString::number(data.id));

        stream.writeTextElement("type", QString::number(data.type));

        stream.writeTextElement("state", QString::number(data.state));

        stream.writeTextElement("source", data.source);

        stream.writeTextElement("title", data.title);
        stream.writeTextElement("cover", data.cover);

        stream.writeTextElement("author", data.author);
        stream.writeTextElement("feed",   data.feed);

        stream.writeTextElement("duration", QString::number(data.duration));

        stream.writeTextElement("date", Sk::dateToStringNumber(data.date));

        stream.writeEndElement(); // track
    }

    stream.writeEndElement(); // tracks
    stream.writeEndElement(); // playlist
    stream.writeEndElement(); // name

    stream.writeEndDocument();

    WControllerFile::writeFile(path, data);

    qDebug("PLAYLIST SAVED");

    return true;
}

//=================================================================================================
// WPlaylistWriteReply
//=================================================================================================

/* virtual */ void WPlaylistWriteReply::onCompleted(bool ok)
{
    data->setSaved(ok);
}

//=================================================================================================
// WPlaylistRead and WPlaylistReadReply
//=================================================================================================

class WPlaylistReadReply;

class WPlaylistRead : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WPlaylistRead(WPlaylistPrivate * data)
    {
        this->data = data;
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

private: // Functions
    bool extract(WPlaylistReadReply * reply, const QByteArray & array);

    bool loadPlaylist(QXmlStreamReader * stream, WPlaylistReadReply * reply);
    bool loadTracks  (QXmlStreamReader * stream, WPlaylistReadReply * reply);

public: // Variables
    WPlaylistPrivate * data;

    QString path;
};

//-------------------------------------------------------------------------------------------------

class WPlaylistReadReply : public WAbstractThreadReply
{
    Q_OBJECT

public:
    WPlaylistReadReply(WPlaylistPrivate * data)
    {
        this->data = data;
    }

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

public: // Variables
    WPlaylistPrivate * data;

    QString source;

    QString title;
    QString cover;

    QString label;

    int currentIndex;
    int currentTime;

    qreal scrollValue;

    QList<WTrack> dataTracks;
};

//=================================================================================================
// WPlaylistRead
//=================================================================================================

/* virtual */ WAbstractThreadReply * WPlaylistRead::createReply() const
{
    return new WPlaylistReadReply(data);
}

/* virtual */ bool WPlaylistRead::run()
{
    WPlaylistReadReply * reply = qobject_cast<WPlaylistReadReply *> (this->reply());

    return extract(reply, WControllerFile::readFile(path));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

bool WPlaylistRead::extract(WPlaylistReadReply * reply, const QByteArray & array)
{
    QXmlStreamReader stream(array);

    if (WControllerXml::readNextStartElement(&stream, "version") == false) return false;

    QString version = WControllerXml::readNextString(&stream);

    if (Sk::versionIsHigher(Sk::versionSky(), version))
    {
        QString content = array;

        WBackendPlaylist_patch(content, version);

        return extract(reply, content.toUtf8());
    }

    if (loadPlaylist(&stream, reply) == false || loadTracks(&stream, reply) == false)
    {
        qWarning("WPlaylistRead::extract: Invalid file %s.", path.C_STR);

        return false;
    }

    qDebug("PLAYLIST LOADED");

    return true;
}

//-------------------------------------------------------------------------------------------------

bool WPlaylistRead::loadPlaylist(QXmlStreamReader * stream, WPlaylistReadReply * reply)
{
    //---------------------------------------------------------------------------------------------
    // source

    if (WControllerXml::readNextStartElement(stream, "source") == false) return false;

    reply->source = WControllerXml::readNextString(stream);

    //---------------------------------------------------------------------------------------------
    // title

    if (WControllerXml::readNextStartElement(stream, "title") == false) return false;

    reply->title = WControllerXml::readNextString(stream);

    //---------------------------------------------------------------------------------------------
    // cover

    if (WControllerXml::readNextStartElement(stream, "cover") == false) return false;

    reply->cover = WControllerXml::readNextString(stream);

    //---------------------------------------------------------------------------------------------
    // label

    if (WControllerXml::readNextStartElement(stream, "label") == false) return false;

    reply->label = WControllerXml::readNextString(stream);

    //---------------------------------------------------------------------------------------------
    // currentIndex

    if (WControllerXml::readNextStartElement(stream, "currentIndex") == false) return false;

    reply->currentIndex = WControllerXml::readNextInt(stream);

    //---------------------------------------------------------------------------------------------
    // currentTime

    if (WControllerXml::readNextStartElement(stream, "currentTime") == false) return false;

    reply->currentTime = WControllerXml::readNextInt(stream);

    //---------------------------------------------------------------------------------------------
    // scrollValue

    if (WControllerXml::readNextStartElement(stream, "scrollValue") == false) return false;

    reply->scrollValue = WControllerXml::readNextFloat(stream);

    return true;
}

bool WPlaylistRead::loadTracks(QXmlStreamReader * stream, WPlaylistReadReply * reply)
{
    while (WControllerXml::readNextStartElement(stream, "track"))
    {
        WTrack track;

        WTrackPrivate * p = track.d_func();

        //-----------------------------------------------------------------------------------------
        // id

        if (WControllerXml::readNextStartElement(stream, "id") == false) return false;

        p->id = WControllerXml::readNextInt(stream);

        //-----------------------------------------------------------------------------------------
        // type

        if (WControllerXml::readNextStartElement(stream, "type") == false) return false;

        p->type = static_cast<WTrack::Type> (WControllerXml::readNextInt(stream));

        //-----------------------------------------------------------------------------------------
        // state

        if (WControllerXml::readNextStartElement(stream, "state") == false) return false;

        p->state = static_cast<WTrack::State> (WControllerXml::readNextInt(stream));

        //-----------------------------------------------------------------------------------------
        // source

        if (WControllerXml::readNextStartElement(stream, "source") == false) return false;

        p->source = WControllerXml::readNextString(stream);

        //-----------------------------------------------------------------------------------------
        // title

        if (WControllerXml::readNextStartElement(stream, "title") == false) return false;

        p->title = WControllerXml::readNextString(stream);

        //-----------------------------------------------------------------------------------------
        // cover

        if (WControllerXml::readNextStartElement(stream, "cover") == false) return false;

        p->cover = WControllerXml::readNextString(stream);

        //-----------------------------------------------------------------------------------------
        // author

        if (WControllerXml::readNextStartElement(stream, "author") == false) return false;

        p->author = WControllerXml::readNextString(stream);

        //-----------------------------------------------------------------------------------------
        // feed

        if (WControllerXml::readNextStartElement(stream, "feed") == false) return false;

        p->feed = WControllerXml::readNextString(stream);

        //-----------------------------------------------------------------------------------------
        // duration

        if (WControllerXml::readNextStartElement(stream, "duration") == false) return false;

        p->duration = WControllerXml::readNextInt(stream);

        //-----------------------------------------------------------------------------------------
        // date

        if (WControllerXml::readNextStartElement(stream, "date") == false) return false;

        p->date = WControllerXml::readNextDate(stream);

        //-----------------------------------------------------------------------------------------

        reply->dataTracks.append(track);
    }

    return true;
}

//=================================================================================================
// WPlaylistReadReply
//=================================================================================================

/* virtual */ void WPlaylistReadReply::onCompleted(bool ok)
{
    WPlaylist * q = data->q_func();

    bool saveEnabled = q->saveEnabled();

    q->setSaveEnabled(false);

    if (ok)
    {
        if (data->cacheLoad)
        {
            if (data->source.isEmpty()) q->applySource(source);

            if (data->title.isEmpty()) q->setTitle(title);
            if (data->cover.isEmpty()) q->setCover(cover);

            if (data->label.isEmpty()) q->setLabel(label);

            if (data->scrollValue == 0) q->setScrollValue(scrollValue);

            data->loadTracks(dataTracks);

            if (data->currentIndex == -1) q->setCurrentIndex(currentIndex);
            if (data->currentTime  == -1) q->setCurrentTime (currentTime);
        }
        else
        {
            q->applySource(source);

            q->setTitle(title);
            q->setCover(cover);

            q->setScrollValue(scrollValue);

            data->loadTracks(dataTracks);

            q->setCurrentIndex(currentIndex);
            q->setCurrentTime (currentTime);
        }
    }
    else
    {
        q->applySource(QString());

        q->setTitle(QString());
        q->setCover(QString());

        q->setLabel(QString());

        q->setScrollValue(0);

        q->setCurrentIndex(-1);
        q->setCurrentTime (-1);
    }

    q->setSaveEnabled(saveEnabled);

    data->setLoaded(ok);
}

//=================================================================================================
// WPlaylistPrivate
//=================================================================================================

WPlaylistPrivate::WPlaylistPrivate(WPlaylist * p) : WLibraryItemPrivate(p) {}

/* virtual */ WPlaylistPrivate::~WPlaylistPrivate()
{
    foreach (WPlaylistWatcher * watcher, watchers)
    {
        watcher->playlistDestroyed();
    }
}

//-------------------------------------------------------------------------------------------------

void WPlaylistPrivate::init()
{
    Q_Q(WPlaylist);

    currentTrack = NULL;

    currentIndex = -1;
    currentTime  = -1;

    scrollValue = 0;

    maxCount = PLAYLIST_MAX;

    QObject::connect(q, SIGNAL(countChanged()), q, SIGNAL(playlistUpdated()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

int WPlaylistPrivate::beginInsert(int index, int count)
{
    Q_Q(WPlaylist);

    int trackCount = tracks.count();

    if (index < 0 || index > trackCount)
    {
        if (type == WPlaylist::PlaylistFeed) index = 0;
        else                                 index = trackCount;
    }

    q->beginTracksInsert(index, index + count - 1);

    return index;
}

void WPlaylistPrivate::endInsert(int index, int count)
{
    Q_Q(WPlaylist);

    q->endTracksInsert();

    q->updateCover();
    q->updateIndex();

    emit q->countChanged();

    emit q->tracksInserted(index, count);

    q->save();
}

void WPlaylistPrivate::insertTrack(WPlaylist * playlist, int index, const WTrack & track)
{
    tracks.insert(index, track);

    WTrackPrivate * p = tracks[index].d_func();

    p->playlist = playlist;

    int id = p->id;

    if (id == -1)
    {
        p->id = ids.generateId();
    }
    else if (ids.insertId(id) == false)
    {
        qWarning("WPlaylistPrivate::insertTrack: Id is already taken '%d'.", id);

        p->id = ids.generateId();
    }
}

//-------------------------------------------------------------------------------------------------

#ifdef QT_6

void WPlaylistPrivate::insertTracks(int index, WList<WTrack> tracks)
{
    Q_Q(WPlaylist);

    int count = tracks.count();

    if (count == 0 || q->checkFull(count)) return;

    index = beginInsert(index, count);

    int oldIndex = index;

    W_FOREACH (const WTrack & track, tracks)
    {
        insertTrack(q, index, track);

        index++;
    }

    endInsert(oldIndex, count);
}

#endif

//-------------------------------------------------------------------------------------------------

/* virtual */ const WTrack * WPlaylistPrivate::itemFromId(int id) const
{
    for (int i = 0; i < tracks.count(); i++)
    {
        const WTrack & track = tracks.at(i);

        if (track.id() == id)
        {
            return &(track);
        }
    }

    return NULL;
}

/* virtual */ const WTrack * WPlaylistPrivate::itemAt(int index) const
{
    return &(tracks.at(index));
}

//-------------------------------------------------------------------------------------------------

/* virtual */ WTrack * WPlaylistPrivate::getTrack(int index)
{
    if (index < 0 || index >= tracks.count()) return NULL;

    return &(tracks[index]);
}

//-------------------------------------------------------------------------------------------------

void WPlaylistPrivate::loadTracks(const QList<WTrack> & tracks)
{
    Q_Q(WPlaylist);

    if (this->tracks.count())
    {
        q->setCurrentId(-1);

        selectedTracks.clear();

        foreach (WPlaylistWatcher * watcher, watchers)
        {
            watcher->beginTracksClear();
        }

        this->tracks.clear();

        foreach (WPlaylistWatcher * watcher, watchers)
        {
            watcher->endTracksClear();
        }

        ids.clear();

        emit q->tracksCleared();

        if (tracks.isEmpty())
        {
            emit q->countChanged();
        }
        else q->addTracks(tracks);
    }
    else if (tracks.count())
    {
        q->addTracks(tracks);
    }
}

//-------------------------------------------------------------------------------------------------

void WPlaylistPrivate::loadTrack(int index)
{
    WTrack * track = &(tracks[index]);

    WTrack::State state = track->state();

    if (state == WTrack::LoadCover)
    {
        loadCover(track, QNetworkRequest::NormalPriority);
    }
    else if (state == WTrack::Default)
    {
        applyTrack(track, index, -1);
    }
}

void WPlaylistPrivate::applyTrack(WTrack * track, int index, int delay)
{
    Q_Q(WPlaylist);

    WTrackPrivate * p = track->d_func();

    qint64 timeUpdate = p->timeUpdate;

    p->timeUpdate = Sk::currentDateToMSecs();

    // NOTE: Sometimes we don't want to reload a track too soon.
    if (p->timeUpdate - timeUpdate < delay) return;

#ifdef SK_NO_TORRENT
    WControllerPlaylistPrivate * pController = wControllerPlaylist->d_func();

    pController->abortQueryTrack(track);

    pController->applySourceTrack(q, track, p->source, 0);
#else
    // FIXME: For now, we don't want to reload a loaded torrent. It causes issues when a magnet
    //        is not responding well.
    if (p->state == WTrack::Default
        ||
        WControllerPlaylist::urlIsTorrent(p->source) == false)
#endif
    {
        WControllerPlaylistPrivate * pController = wControllerPlaylist->d_func();

        pController->abortQueryTrack(track);

        pController->applySourceTrack(q, track, p->source, 0);
    }

    WTrack::State state = p->state;

    if (state == WTrack::Loaded)
    {
        if (p->cover.isEmpty())
        {
            loadCover(track, QNetworkRequest::NormalPriority);
        }
    }
    else if (state == WTrack::LoadCover)
    {
        loadCover(track, QNetworkRequest::NormalPriority);
    }
    else if (state == WTrack::Default)
    {
        track->setState(WTrack::Loaded);

        q->updateTrack(index);
    }
}

//-------------------------------------------------------------------------------------------------

void WPlaylistPrivate::loadCover(WTrack * track, QNetworkRequest::Priority priority)
{
    QString label = track->author();

    if (label.isEmpty())
    {
        label = WControllerFile::folderName(track->feed());
    }

    QString title = track->title();

    WBackendNet * backend = wControllerPlaylist->backendForCover(label, title);

    if (backend == NULL) return;

    Q_Q(WPlaylist);

    WBackendNetQuery query = backend->createQuery("cover", label, title);

    query.priority = priority;

    wControllerPlaylist->d_func()->applyQueryTrack(backend->id(), q, track, &query);

    backend->tryDelete();
}

//-------------------------------------------------------------------------------------------------

void WPlaylistPrivate::applyTrackDefault()
{
    Q_Q(WPlaylist);

    emit q->trackQueryEnded    ();
    emit q->trackQueryCompleted();
}

void WPlaylistPrivate::applyTrackLoaded(int index)
{
    Q_Q(WPlaylist);

    WTrack * track = &(tracks[index]);

    if (track->state() != WTrack::LoadCover)
    {
        emit q->trackQueryCompleted();

        return;
    }

    // NOTE: The priority should be high because it's tied to the track loading completion.
    loadCover(track, QNetworkRequest::HighPriority);
}

void WPlaylistPrivate::applyTrackAbort(int index)
{
    Q_Q(WPlaylist);

    WTrack * track = &(tracks[index]);

    // NOTE: We want to clear the update timeout so we can reload the track right away.
    track->d_func()->timeUpdate = 0;

    track->setState(WTrack::Default);

    q->updateTrack(index);
}

//-------------------------------------------------------------------------------------------------

void WPlaylistPrivate::setPrevious(bool cycle)
{
    if (currentIndex == -1) return;

    Q_Q(WPlaylist);

    int index = currentIndex;

    if (cycle)
    {
        if (index == 0)
        {
             index = q->count() - 1;
        }
        else index--;

        q->setCurrentIndex(index);
    }
    else if (hasPrevious(index))
    {
        q->setCurrentIndex(index - 1);
    }
}

void WPlaylistPrivate::setNext(bool cycle)
{
    if (currentIndex == -1) return;

    Q_Q(WPlaylist);

    int index = currentIndex;

    if (cycle)
    {
        if (index == q->count() - 1)
        {
             index = 0;
        }
        else index++;

        q->setCurrentIndex(index);
    }
    else if (hasNext(index))
    {
        q->setCurrentIndex(index + 1);
    }
}

//-------------------------------------------------------------------------------------------------

bool WPlaylistPrivate::hasPrevious(int index) const
{
    if (index > 0)
    {
         return true;
    }
    else return false;
}

bool WPlaylistPrivate::hasNext(int index) const
{
    Q_Q(const WPlaylist);

    if (index >= 0 && index < (q->count() - 1))
    {
         return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

bool WPlaylistPrivate::insertSelected(const QList<int> & indexes,
                                      const WTrack     * track, int index)
{
    if (selectedTracks.contains(track)) return false;

    for (int i = 0; i < indexes.count(); i++)
    {
        if (indexes.at(i) > index)
        {
            selectedTracks.insert(i, track);

            return true;
        }
    }

    selectedTracks.append(track);

    return true;
}

QList<int> WPlaylistPrivate::getSelected() const
{
    Q_Q(const WPlaylist);

    QList<int> selected;

    foreach (const WTrack * track, selectedTracks)
    {
        int index = q->indexOf(track);

        selected.append(index);
    }

    std::sort(selected.begin(), selected.end());

    return selected;
}

//-------------------------------------------------------------------------------------------------

void WPlaylistPrivate::vbmlTrack(QString & vbml, const WTrackPrivate * p,
                                                 const QString       & tab) const
{
    Sk::bmlPair(vbml, tab + "title", p->title);
    Sk::bmlPair(vbml, tab + "cover", p->cover);

    Sk::bmlPair(vbml, tab + "author", p->author);
    Sk::bmlPair(vbml, tab + "feed",   p->feed);

    if (p->duration > 0)
    {
        Sk::bmlPair(vbml, tab + "duration", QString::number(p->duration));
    }

    if (p->date.isValid())
    {
        Sk::bmlPair(vbml, tab + "date", Sk::bmlDate(p->date));
    }
}

//-------------------------------------------------------------------------------------------------

void WPlaylistPrivate::currentIndexChanged()
{
    foreach (WPlaylistWatcher * watcher, watchers)
    {
        watcher->currentIndexChanged(currentIndex);
    }
}

//-------------------------------------------------------------------------------------------------

void WPlaylistPrivate::emitSelectedTracksChanged(const QList<int> & indexes)
{
    Q_Q(WPlaylist);

    foreach (WPlaylistWatcher * watcher, watchers)
    {
        watcher->selectedTracksChanged(indexes);
    }

    emit q->selectedTracksChanged();
}

//=================================================================================================
// WPlaylist
//=================================================================================================

/* explicit */ WPlaylist::WPlaylist(WLibraryFolder * parent)
    : WLibraryItem(new WPlaylistPrivate(this), Playlist, parent)
{
    Q_D(WPlaylist); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

WPlaylist::WPlaylist(WPlaylistPrivate * p, Type type, WLibraryFolder * parent)
    : WLibraryItem(p, type, parent)
{
    Q_D(WPlaylist); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WPlaylist::addTrack(const WTrack & track)
{
    return insertTracks(count(), QList<WTrack>() << track);
}

/* Q_INVOKABLE */ int WPlaylist::addTracks(const QList<WTrack> & tracks)
{
    return insertTracks(count(), tracks);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WPlaylist::insertTrack(int index, const WTrack & track)
{
    return insertTracks(index, QList<WTrack>() << track);
}

/* Q_INVOKABLE */ int WPlaylist::insertTracks(int index, const QList<WTrack> & tracks)
{
    Q_D(WPlaylist);

    int count = tracks.count();

    if (count == 0) return 0;

    if (checkFull(count))
    {
        count = d->maxCount - this->count();

        if (count == 0) return 0;
    }

    index = d->beginInsert(index, count);

    int oldIndex = index;

    for (int i = 0; i < count; i++)
    {
        d->insertTrack(this, index, tracks.at(i));

        index++;
    }

    d->endInsert(oldIndex, count);

    return count;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WPlaylist::addSource(const QString & url, bool load)
{
    return insertSource(count(), url, load);
}

/* Q_INVOKABLE */ int WPlaylist::insertSource(int index, const QString & url, bool load)
{
    WTrack track(url, WTrack::Default);

    if (load)
    {
        if (insertTrack(index, track))
        {
            loadTrack(index);

            return 1;
        }
        else return 0;
    }
    else return insertTrack(index, track);
}

/* Q_INVOKABLE */ int WPlaylist::addSources(const QString & urls)
{
    return insertSources(count(), urls);
}

/* Q_INVOKABLE */ int WPlaylist::insertSources(int index, const QString & urls)
{
    QList<WTrack> tracks;

    QStringList list = Sk::split(urls, '\n');

    foreach (const QString & source, list)
    {
        WTrack track(source, WTrack::Default);

        tracks.append(track);
    }

    return insertTracks(index, tracks);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylist::moveTrack(int from, int to)
{
    Q_D(WPlaylist);

    int count = d->tracks.count();

    if (from < 0 || from > count
        ||
        to < 0 || to > count) return;

    if (from <= to && from > to - 2) return;

    beginTracksMove(from, from, to);

    if (from < to)
    {
         d->tracks.move(from, to - 1);
    }
    else d->tracks.move(from, to);

    endTracksMove();

    updateIndex();

    QList<int> indexes;

    indexes.append(from);

    emit tracksMoved(indexes, to);

    emit playlistUpdated();

    save();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylist::removeTrack(int index)
{
    removeTracks(QList<int>() << index);
}

/* Q_INVOKABLE */ void WPlaylist::removeTracks(const QList<int> & indexes)
{
    Q_D(WPlaylist);

    if (indexes.isEmpty()) return;

    QList<int> sortedIndexes = indexes;

    std::sort(sortedIndexes.begin(), sortedIndexes.end());

    QList<WTrack *> tracks;

    QList<int> changed;

    foreach (int index, sortedIndexes)
    {
        if (index < 0 || index >= d->tracks.count()) continue;

        WTrack * track = &(d->tracks[index]);

        if (d->selectedTracks.contains(track))
        {
            d->selectedTracks.removeOne(track);

            changed.append(index);
        }

        tracks.append(track);
    }

    if (changed.count())
    {
        d->emitSelectedTracksChanged(changed);
    }

    foreach (WTrack * track, tracks)
    {
        wControllerPlaylist->d_func()->abortQueryTrack(track);

        int index = indexOf(track);

        if (d->currentTrack == track)
        {
            setCurrentId(-1);
        }

        d->ids.removeOne(track->d_func()->id);

        beginTracksRemove(index, index);

        d->tracks.removeAt(index);

        endTracksRemove();
    }

    updateIndex();

    emit countChanged();

    emit tracksRemoved(sortedIndexes);

    save();
}

/* Q_INVOKABLE */ void WPlaylist::removeTracks(int from, int count)
{
    QList<int> indexes;

    while (count)
    {
        indexes.append(from);

        from++;

        count--;
    }

    removeTracks(indexes);
}

/* Q_INVOKABLE */ void WPlaylist::removeSelectedTracks()
{
    removeTracks(selectedTracks());
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylist::clearDuplicates()
{
    Q_D(WPlaylist);

    QStringList sources;

    W_FOREACH (const WTrack & track, d->tracks)
    {
        sources.append(track.source());
    }

    QStringList urls;

    QList<int> indexes;

    for (int i = 0; i < sources.count(); i++)
    {
        QString source = sources.at(i);

        if (urls.contains(source))
        {
            indexes.append(i);
        }
        else urls.append(source);
    }

    removeTracks(indexes);
}

/* Q_INVOKABLE */ void WPlaylist::clearTracks()
{
    abortAll();

    wControllerPlaylist->d_func()->abortQueriesItem(this);

    if (count())
    {
        Q_D(WPlaylist);

        setCurrentId(-1);

        setScrollValue(0);

        d->selectedTracks.clear();

        foreach (WPlaylistWatcher * watcher, d->watchers)
        {
            watcher->beginTracksClear();
        }

        d->tracks.clear();
        d->ids   .clear();

        foreach (WPlaylistWatcher * watcher, d->watchers)
        {
            watcher->endTracksClear();
        }

        emit tracksCleared();

        emit countChanged();
    }

    save();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylist::updateCover()
{
    Q_D(WPlaylist);

    if (d->cover.isEmpty() == false
        ||
        (d->type != Playlist && d->type != PlaylistFeed)) return;

    W_FOREACH (const WTrack & track, d->tracks)
    {
        QString cover = track.d_func()->cover;

        if (cover.isEmpty() == false)
        {
            setCover(cover);

            return;
        }
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylist::loadTrack(int at)
{
    Q_D(WPlaylist);

    if (at < 0 || at >= d->tracks.count()) return;

    d->loadTrack(at);
}

/* Q_INVOKABLE */ void WPlaylist::reloadTrack(int at, int delay)
{
    Q_D(WPlaylist);

    if (at < 0 || at >= d->tracks.count()) return;

    WTrack * track = &(d->tracks[at]);

    d->applyTrack(track, at, delay);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylist::loadTracks(int at, int count)
{
    Q_D(WPlaylist);

    if (at < 0 || at >= d->tracks.count() || count < 1) return;

    while (at < d->tracks.count() && count)
    {
        d->loadTrack(at);

        count--;

        at++;
    }
}

/* Q_INVOKABLE */ void WPlaylist::reloadTracks(int at, int count, int delay)
{
    Q_D(WPlaylist);

    if (at < 0 || at >= d->tracks.count() || count < 1) return;

    while (at < d->tracks.count() && count)
    {
        WTrack * track = &(d->tracks[at]);

        d->applyTrack(track, at, delay);

        count--;

        at++;
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylist::loadTracksBetween(int at, int count)
{
    at -= count / 2;

    if (at < 0)
    {
         loadTracks(0, count);
    }
    else loadTracks(at, count);
}

/* Q_INVOKABLE */ void WPlaylist::reloadTracksBetween(int at, int count, int delay)
{
    at -= count / 2;

    if (at < 0)
    {
         reloadTracks(0, count, delay);
    }
    else reloadTracks(at, count, delay);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylist::abortTracks()
{
    wControllerPlaylist->d_func()->abortQueriesTracks(this);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WPlaylist::checkFull(int count) const
{
    Q_D(const WPlaylist);

    return (this->count() + count > d->maxCount);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WPlaylist::hasPreviousIndex(int index) const
{
    Q_D(const WPlaylist);

    if (d->type == PlaylistFeed)
    {
         return d->hasNext(index);
    }
    else return d->hasPrevious(index);
}

/* Q_INVOKABLE */ bool WPlaylist::hasNextIndex(int index) const
{
    Q_D(const WPlaylist);

    if (d->type == PlaylistFeed)
    {
         return d->hasPrevious(index);
    }
    else return d->hasNext(index);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylist::setPreviousTrack(bool cycle)
{
    Q_D(WPlaylist);

    if (d->type == PlaylistFeed)
    {
         d->setNext(cycle);
    }
    else d->setPrevious(cycle);
}

/* Q_INVOKABLE */ void WPlaylist::setNextTrack(bool cycle)
{
    Q_D(WPlaylist);

    if (d->type == PlaylistFeed)
    {
         d->setPrevious(cycle);
    }
    else d->setNext(cycle);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylist::selectTrack(int index)
{
    if (index < 0 || index >= count()) return;

    Q_D(WPlaylist);

    const WTrack * track = d->itemAt(index);

    if (d->selectedTracks.contains(track)) return;

    d->selectedTracks.append(track);

    d->emitSelectedTracksChanged(QList<int>() << index);
}

/* Q_INVOKABLE */ void WPlaylist::selectTracks(int from, int to)
{
    if (from < 0 || from >= count() || to < 0 || to >= count()) return;

    Q_D(WPlaylist);

    QList<int> selected = d->getSelected();

    QList<int> changed;

    if (from < to)
    {
        for (int i = from; i <= to; i++)
        {
            const WTrack * track = d->itemAt(i);

            if (d->insertSelected(selected, track, i))
            {
                changed.append(i);
            }
        }
    }
    else
    {
        for (int i = from; i >= to; i--)
        {
            const WTrack * track = d->itemAt(i);

            if (d->insertSelected(selected, track, i))
            {
                changed.append(i);
            }
        }
    }

    if (changed.count())
    {
        std::sort(changed.begin(), changed.end());

        d->emitSelectedTracksChanged(changed);
    }
}

/* Q_INVOKABLE */ void WPlaylist::selectAll()
{
    selectTracks(0, count() - 1);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylist::selectSingleTrack(int index)
{
    if (index < 0 || index >= count()) return;

    Q_D(WPlaylist);

    const WTrack * track = d->itemAt(index);

    if (d->selectedTracks.contains(track))
    {
        if (d->selectedTracks.count() == 1) return;

        d->selectedTracks.removeOne(track);
    }
    else d->selectedTracks.append(track);

    QList<int> selected = d->getSelected();

    d->selectedTracks.clear();

    d->selectedTracks.append(track);

    d->emitSelectedTracksChanged(selected);
}

/* Q_INVOKABLE */ void WPlaylist::selectCurrentTrack()
{
    Q_D(WPlaylist); selectSingleTrack(d->currentIndex);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylist::moveSelectedTo(int to)
{
    Q_D(WPlaylist);

    if (d->selectedTracks.isEmpty() || to < 0 || to > count()) return;

    QList<int> indexes = selectedTracks();

    // NOTE: We backup the 'to' before messing up with it.
    int at = to;

    foreach (const WTrack * track, d->selectedTracks)
    {
        int from = indexOf(track);

        if (from > to || from < to - 1)
        {
            beginTracksMove(from, from, to);

            if (from < to) to--;

            d->tracks.move(from, to);

            endTracksMove();

            to++;
        }
        else if (from == to) to++;
    }

    updateIndex();

    emit tracksMoved(indexes, at);

    emit playlistUpdated();

    save();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylist::unselectTrack(int index)
{
    if (index < 0 || index >= count()) return;

    Q_D(WPlaylist);

    const WTrack * track = d->itemAt(index);

    if (d->selectedTracks.contains(track) == false) return;

    d->selectedTracks.removeOne(track);

    d->emitSelectedTracksChanged(QList<int>() << index);
}

/* Q_INVOKABLE */ void WPlaylist::unselectTracks()
{
    Q_D(WPlaylist);

    if (d->selectedTracks.isEmpty()) return;

    QList<int> selected = d->getSelected();

    d->selectedTracks.clear();

    d->emitSelectedTracksChanged(selected);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WPlaylist::indexSelected(int index) const
{
    if (index < 0 || index >= count()) return false;

    Q_D(const WPlaylist);

    const WTrack * track = d->itemAt(index);

    if (d->selectedTracks.contains(track))
    {
         return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WPlaylist::closestSelected(int index) const
{
    Q_D(const WPlaylist);

    int closest = -1;

    QList<int> selected = d->getSelected();

    foreach (int selectedIndex, selected)
    {
        if (closest == -1 || (selectedIndex != index
                              &&
                              qAbs(index - selectedIndex) < qAbs(index - closest)))
        {
            closest = selectedIndex;
        }
    }

    return closest;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WTrack WPlaylist::getTrackAt(int index) const
{
    Q_D(const WPlaylist);

    if (index < 0 || index >= d->tracks.count()) return WTrack();

    WTrack track = d->tracks.at(index);

    track.d_func()->id = -1;

    return track;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WPlaylist::idAt(int index) const
{
    Q_D(const WPlaylist);

    if (index < 0 || index >= d->tracks.count())
    {
         return -1;
    }
    else return d->tracks.at(index).d_func()->id;
}

/* Q_INVOKABLE */ int WPlaylist::indexOf(const WTrack * track) const
{
    Q_D(const WPlaylist);

    for (int i = 0; i < d->tracks.count(); i++)
    {
        if (&(d->tracks.at(i)) == track)
        {
            return i;
        }
    }

    return -1;
}

/* Q_INVOKABLE */ int WPlaylist::indexFromId(int id) const
{
    Q_D(const WPlaylist);

    for (int i = 0; i < d->tracks.count(); i++)
    {
        if (d->tracks.at(i).d_func()->id == id)
        {
            return i;
        }
    }

    return -1;
}

/* Q_INVOKABLE */ int WPlaylist::indexFromSource(const QString & source, bool clean) const
{
    Q_D(const WPlaylist);

    if (clean)
    {
        QString string = WControllerPlaylist::cleanSource(source);

        for (int i = 0; i < d->tracks.count(); i++)
        {
            if (WControllerPlaylist::cleanSource(d->tracks.at(i).d_func()->source) == string)
            {
                return i;
            }
        }
    }
    else
    {
        for (int i = 0; i < d->tracks.count(); i++)
        {
            if (d->tracks.at(i).d_func()->source == source)
            {
                return i;
            }
        }
    }

    return -1;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WPlaylist::contains(const WTrack & track) const
{
    Q_D(const WPlaylist); return d->tracks.contains(track);
}

/* Q_INVOKABLE */ bool WPlaylist::containsSource(const QString & source, bool clean) const
{
    return (indexFromSource(source, clean) != -1);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WPlaylist * WPlaylist::duplicate() const
{
    Q_D(const WPlaylist);

    WPlaylist * playlist = create(d->type);

    if (d->stateQuery == Loading)
    {
         playlist->d_func()->stateQuery = Default;
    }
    else playlist->d_func()->stateQuery = d->stateQuery;

    playlist->d_func()->source = d->source;

    playlist->setTitle(d->title);
    playlist->setCover(d->cover);

#ifdef QT_OLD
    playlist->addTracks(d->tracks);
#else
    playlist->d_func()->insertTracks(0, d->tracks);
#endif

    playlist->setCurrentIndex(d->currentIndex);
    playlist->setCurrentTime (d->currentTime);

    playlist->setScrollValue(d->scrollValue);

    return playlist;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylist::copyTracksTo(const QList<int> & indexes,
                                               WPlaylist        * destination, int to) const
{
    Q_ASSERT(destination);

    QList<WTrack> tracks;

    foreach (int index, indexes)
    {
        tracks.append(getTrackAt(index));
    }

    destination->insertTracks(to, tracks);
}

/* Q_INVOKABLE */ void WPlaylist::copyTrackTo(int from, WPlaylist * destination, int to) const
{
    Q_ASSERT(destination);

    Q_D(const WPlaylist);

    if (from < 0 || from >= d->tracks.count()) return;

    WTrack track = getTrackAt(from);

    destination->insertTrack(to, track);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylist::copyAllTo(WPlaylist * destination, int to) const
{
    Q_ASSERT(destination);

    Q_D(const WPlaylist);

#ifdef QT_OLD
    destination->insertTracks(to, d->tracks);
#else
    destination->d_func()->insertTracks(to, d->tracks);
#endif
}

/* Q_INVOKABLE */ void WPlaylist::copySelectedTo(WPlaylist * destination, int to) const
{
    Q_ASSERT(destination);

    Q_D(const WPlaylist);

    copyTracksTo(d->getSelected(), destination, to);
}

//-------------------------------------------------------------------------------------------------
// Track interface

/* Q_INVOKABLE */ bool WPlaylist::trackIsValid(int index) const
{
    const WTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->isValid();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QVariantMap WPlaylist::trackData(int index) const
{
    const WTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->toMap();
    }
    else return QVariantMap();
}

/* Q_INVOKABLE */ QString WPlaylist::trackVbml(int index) const
{
    const WTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->toVbml();
    }
    else return QString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WTrack::Type WPlaylist::trackType(int index) const
{
    const WTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->type();
    }
    else return WTrack::Track;
}

/* Q_INVOKABLE */ void WPlaylist::setTrackType(int index, WTrack::Type type)
{
    Q_D(WPlaylist);

    WTrack * track = d->getTrack(index);

    if (track == NULL || track->type() == type) return;

    track->setType(type);

    updateTrack(index);
}

/* Q_INVOKABLE */ bool WPlaylist::trackIsHub(int index) const
{
    const WTrack * track = trackPointerAt(index);

    if (track)
    {
        return track->isHub();
    }
    else return false;
}

/* Q_INVOKABLE */ bool WPlaylist::trackIsChannel(int index) const
{
    const WTrack * track = trackPointerAt(index);

    if (track)
    {
        return track->isChannel();
    }
    else return false;
}

/* Q_INVOKABLE */ bool WPlaylist::trackIsInteractive(int index) const
{
    const WTrack * track = trackPointerAt(index);

    if (track)
    {
        return track->isInteractive();
    }
    else return false;
}

/* Q_INVOKABLE */ bool WPlaylist::trackIsLite(int index) const
{
    const WTrack * track = trackPointerAt(index);

    if (track)
    {
        return track->isLite();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WTrack::State WPlaylist::trackState(int index) const
{
    const WTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->state();
    }
    else return WTrack::Default;
}

/* Q_INVOKABLE */ void WPlaylist::setTrackState(int index, WTrack::State state)
{
    Q_D(WPlaylist);

    WTrack * track = d->getTrack(index);

    if (track == NULL || track->state() == state) return;

    track->setState(state);

    updateTrack(index);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WPlaylist::trackIsDefault(int index) const
{
    const WTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->isDefault();
    }
    else return false;
}

/* Q_INVOKABLE */ bool WPlaylist::trackIsLoading(int index) const
{
    const WTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->isLoading();
    }
    else return false;
}

/* Q_INVOKABLE */ bool WPlaylist::trackIsLoaded(int index) const
{
    const WTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->isLoaded();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WPlaylist::trackSource(int index) const
{
    const WTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->source();
    }
    else return QString();
}

/* Q_INVOKABLE */ void WPlaylist::setTrackSource(int index, const QString & source)
{
    Q_D(WPlaylist);

    WTrack * track = d->getTrack(index);

    if (track == NULL || track->source() == source) return;

    track->setSource(source);

    updateTrack(index);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WPlaylist::trackTitle(int index) const
{
    const WTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->title();
    }
    else return QString();
}

/* Q_INVOKABLE */ void WPlaylist::setTrackTitle(int index, const QString & title)
{
    Q_D(WPlaylist);

    WTrack * track = d->getTrack(index);

    if (track == NULL || track->title() == title) return;

    track->setTitle(title);

    updateTrack(index);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WPlaylist::trackCover(int index) const
{
    const WTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->cover();
    }
    else return QString();
}

/* Q_INVOKABLE */ void WPlaylist::setTrackCover(int index, const QString & cover)
{
    Q_D(WPlaylist);

    WTrack * track = d->getTrack(index);

    if (track == NULL || track->cover() == cover) return;

    track->setCover(cover);

    updateTrack(index);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WPlaylist::trackAuthor(int index) const
{
    const WTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->author();
    }
    else return QString();
}

/* Q_INVOKABLE */ void WPlaylist::setTrackAuthor(int index, const QString & author)
{
    Q_D(WPlaylist);

    WTrack * track = d->getTrack(index);

    if (track == NULL || track->author() == author) return;

    track->setAuthor(author);

    updateTrack(index);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WPlaylist::trackFeed(int index) const
{
    const WTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->feed();
    }
    else return QString();
}

/* Q_INVOKABLE */ void WPlaylist::setTrackFeed(int index, const QString & feed)
{
    Q_D(WPlaylist);

    WTrack * track = d->getTrack(index);

    if (track == NULL || track->feed() == feed) return;

    track->setFeed(feed);

    updateTrack(index);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WPlaylist::trackDuration(int index) const
{
    const WTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->duration();
    }
    else return -1;
}

/* Q_INVOKABLE */ void WPlaylist::setTrackDuration(int index, int msec)
{
    Q_D(WPlaylist);

    WTrack * track = d->getTrack(index);

    if (track == NULL || track->duration() == msec) return;

    track->setDuration(msec);

    updateTrack(index);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QDateTime WPlaylist::trackDate(int index) const
{
    const WTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->date();
    }
    else return QDateTime();
}

/* Q_INVOKABLE */ void WPlaylist::setTrackDate(int index, const QDateTime & date)
{
    Q_D(WPlaylist);

    WTrack * track = d->getTrack(index);

    if (track == NULL || track->date() == date) return;

    track->setDate(date);

    updateTrack(index);
}

//-------------------------------------------------------------------------------------------------
// Pointers

/* Q_INVOKABLE */ const WTrack * WPlaylist::trackPointerFromId(int id) const
{
    Q_D(const WPlaylist);

    return d->itemFromId(id);
}

/* Q_INVOKABLE */ const WTrack * WPlaylist::trackPointerAt(int index) const
{
    if (index < 0 || index >= count()) return NULL;

    Q_D(const WPlaylist);

    return d->itemAt(index);
}

/* Q_INVOKABLE */ QList<const WTrack *> WPlaylist::trackPointers() const
{
    Q_D(const WPlaylist);

    QList<const WTrack *> tracks;

    for (int i = 0; i < count(); i++)
    {
        tracks.append(d->itemAt(i));
    }

    return tracks;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ const WTrack * WPlaylist::currentTrackPointer() const
{
    Q_D(const WPlaylist); return d->currentTrack;
}

/* Q_INVOKABLE */ void WPlaylist::setCurrentTrackPointer(const WTrack * track)
{
    setCurrentIndex(indexOf(track));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WPlaylist::containsTrackPointer(const WTrack * track) const
{
    return (indexOf(track) != -1);
}

//---------------------------------------------------------------------------------------------
// Watchers

/* Q_INVOKABLE */ void WPlaylist::registerWatcher(WPlaylistWatcher * watcher)
{
    Q_ASSERT(watcher);

    Q_D(WPlaylist);

    if (d->watchers.contains(watcher)) return;

    d->watchers.append(watcher);
}

/* Q_INVOKABLE */ void WPlaylist::unregisterWatcher(WPlaylistWatcher * watcher)
{
    Q_ASSERT(watcher);

    Q_D(WPlaylist);

    if (d->watchers.contains(watcher) == false) return;

    d->watchers.removeOne(watcher);
}

//-------------------------------------------------------------------------------------------------
// QML

/* Q_INVOKABLE */ int WPlaylist::trackTypeInt(int index) const
{
    return trackType(static_cast<WTrack::Type> (index));
}

/* Q_INVOKABLE */ int WPlaylist::trackStateInt(int index) const
{
    return trackState(static_cast<WTrack::State> (index));
}

#ifdef QT_4

/* Q_INVOKABLE */ void WPlaylist::removeTracks(const QVariantList & indexes)
{
    QList<int> list;

    foreach (QVariant variant, indexes)
    {
        list.append(variant.toInt());
    }

    removeTracks(list);
}


/* Q_INVOKABLE */ void WPlaylist::copyTracksTo(const QVariantList & tracks,
                                               WPlaylist          * destination, int to) const
{
    copyTracksTo(Sk::variantsToInts(tracks), destination, to);
}

#endif

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ WPlaylist * WPlaylist::create(Type type)
{
    if (type == PlaylistFeed)
    {
        return new WPlaylistFeed;
    }
    else if (type == PlaylistSearch)
    {
        return new WPlaylistSearch;
    }
    else return new WPlaylist;
}

//---------------------------------------------------------------------------------------------
// WLibraryItem reimplementation
//---------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WPlaylist::toVbml(int expand, int maximum) const
{
    Q_D(const WPlaylist);

    QString vbml = WControllerPlaylist::vbml();

    if (d->type == WLibraryItem::PlaylistFeed)
    {
         Sk::bmlPair(vbml, "type", "feed", "\n\n");
    }
    else Sk::bmlPair(vbml, "type", "playlist", "\n\n");

    int mode;

    QString source = d->source;

    // NOTE: We don't export local sources.
    if (source.isEmpty() || WControllerNetwork::urlIsFile(source))
    {
        if (expand == 0) expand = 1;
    }
    else Sk::bmlPair(vbml, "origin", source, "\n\n");

    if (expand == 1)
    {
        mode = 1;

        for (int i = 0; i < d->tracks.count(); i++)
        {
            if (i == maximum) break;

            source = d->tracks.at(i).d_func()->source;

            // NOTE: When a source is empty or local, we enforce a more comprehensive export.
            if (source.isEmpty() || WControllerNetwork::urlIsFile(source))
            {
                mode = 2;

                break;
            }
        }
    }
    else if (expand == 2) mode = 3;
    else                  mode = 0;

    Sk::bmlPair(vbml, "title", d->title, "\n\n");
    Sk::bmlPair(vbml, "cover", d->cover, "\n\n");

    if (d->tracks.isEmpty())
    {
        // NOTE: We clear the last '\n'.
        vbml.chop(1);

        return vbml;
    }

    if (mode == 1) // Minimal
    {
        Sk::bmlList(vbml, "tracks");

        QString tab = Sk::tabs(1);

        for (int i = 0; i < d->tracks.count(); i++)
        {
            if (i == maximum) break;

            const WTrackPrivate * p = d->tracks.at(i).d_func();

            Sk::bmlValue(vbml, tab + p->source);
        }

        vbml.append('\n');
    }
    else if (mode == 2) // Expand if necessary
    {
        Sk::bmlTag(vbml, "tracks");

        QString tabA = Sk::tabs(1);
        QString tabB = Sk::tabs(2);

        for (int i = 0; i < d->tracks.count(); i++)
        {
            if (i == maximum) break;

            const WTrackPrivate * p = d->tracks.at(i).d_func();

            Sk::bmlTag(vbml, tabA + WTrack::typeToString(p->type));

            source = p->source;

            // NOTE: When the source is empty or local, we enforce a more comprehensive export.
            if (source.isEmpty() || WControllerNetwork::urlIsFile(source))
            {
                d->vbmlTrack(vbml, p, tabB);
            }
            else Sk::bmlPair(vbml, tabB + "source", source);
        }

        vbml.append('\n');
    }
    else if (mode == 3) // Always expand
    {
        Sk::bmlTag(vbml, "tracks");

        QString tabA = Sk::tabs(1);
        QString tabB = Sk::tabs(2);

        for (int i = 0; i < d->tracks.count(); i++)
        {
            if (i == maximum) break;

            const WTrackPrivate * p = d->tracks.at(i).d_func();

            Sk::bmlTag(vbml, tabA + WTrack::typeToString(p->type));

            source = p->source;

            if (WControllerNetwork::urlIsFile(source) == false)
            {
                Sk::bmlPair(vbml, tabB + "source", source);
            }

            d->vbmlTrack(vbml, p, tabB);
        }

        vbml.append('\n');
    }

    // NOTE: We clear the last '\n'.
    vbml.chop(1);

    return vbml;
}

//-------------------------------------------------------------------------------------------------
// Protected functions
//-------------------------------------------------------------------------------------------------

void WPlaylist::updateIndex()
{
    Q_D(WPlaylist);

    int index = indexOf(d->currentTrack);

    if (d->currentIndex == index) return;

    d->currentIndex = index;

    d->currentIndexChanged();

    emit currentIndexChanged();
}

//-------------------------------------------------------------------------------------------------

void WPlaylist::updateTrack(int index)
{
    Q_D(WPlaylist);

    foreach (WPlaylistWatcher * watcher, d->watchers)
    {
        watcher->trackUpdated(index);
    }

    emit trackUpdated(index);

    if (d->currentIndex == index)
    {
        emit currentTrackUpdated();
    }

    save();
}

void WPlaylist::updateTrack(const WTrack * track)
{
    updateTrack(indexOf(track));
}

//-------------------------------------------------------------------------------------------------

void WPlaylist::beginTracksInsert(int first, int last) const
{
    Q_D(const WPlaylist);

    foreach (WPlaylistWatcher * watcher, d->watchers)
    {
        watcher->beginTracksInsert(first, last);
    }
}

void WPlaylist::beginTracksMove(int first, int last, int to) const
{
    Q_D(const WPlaylist);

    foreach (WPlaylistWatcher * watcher, d->watchers)
    {
        watcher->beginTracksMove(first, last, to);
    }
}

void WPlaylist::beginTracksRemove(int first, int last) const
{
    Q_D(const WPlaylist);

    foreach (WPlaylistWatcher * watcher, d->watchers)
    {
        watcher->beginTracksRemove(first, last);
    }
}

//-------------------------------------------------------------------------------------------------

void WPlaylist::endTracksInsert() const
{
    Q_D(const WPlaylist);

    foreach (WPlaylistWatcher * watcher, d->watchers)
    {
        watcher->endTracksInsert();
    }
}

void WPlaylist::endTracksMove() const
{
    Q_D(const WPlaylist);

    foreach (WPlaylistWatcher * watcher, d->watchers)
    {
        watcher->endTracksMove();
    }
}

void WPlaylist::endTracksRemove() const
{
    Q_D(const WPlaylist);

    foreach (WPlaylistWatcher * watcher, d->watchers)
    {
        watcher->endTracksRemove();
    }
}

//-------------------------------------------------------------------------------------------------
// Protected WLocalObject reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ WAbstractThreadAction * WPlaylist::onSave(const QString & path)
{
    Q_D(WPlaylist);

    WPlaylistWrite * action = new WPlaylistWrite(d);

    action->path = path;

    action->source = d->source;

    action->title = d->title;
    action->cover = d->cover;

    action->label = d->label;

    action->currentIndex = d->currentIndex;
    action->currentTime  = d->currentTime;

    action->scrollValue = d->scrollValue;

    W_FOREACH (const WTrack & track, d->tracks)
    {
        const WTrackPrivate * p = track.d_func();

        WThreadActionDataTrack data;

        data.id = p->id;

        data.type = p->type;

        data.state = p->state;

        data.source = p->source;

        data.title = p->title;
        data.cover = p->cover;

        data.author = p->author;
        data.feed   = p->feed;

        data.duration = p->duration;

        data.date = p->date;

        action->dataTracks.append(data);
    }

    return action;
}

/* virtual */ WAbstractThreadAction * WPlaylist::onLoad(const QString & path)
{
    Q_D(WPlaylist);

    WPlaylistRead * action = new WPlaylistRead(d);

    action->path = path;

    return action;
}

//-------------------------------------------------------------------------------------------------
// Protected WLibraryItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WPlaylist::onApplySource(const QString & source)
{
    clearTracks();

    return wControllerPlaylist->d_func()->applySourcePlaylist(this, source, QString(), 0);
}

/* virtual */ bool WPlaylist::onApplyQuery(const WBackendNetQuery & query)
{
    clearTracks();

    return wControllerPlaylist->d_func()->applyQueryPlaylist(this, query);
}

/* virtual */ bool WPlaylist::onStopQuery()
{
    return wControllerPlaylist->d_func()->abortQueriesPlaylist(this);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WPlaylist::onApplyCurrentIds(const QList<int> & ids)
{
    Q_D(WPlaylist);

    if (ids.count() > 1)
    {
        qWarning("WPlaylist::onApplyCurrentIds: Item does not support multiple ids.");
    }

    unselectTracks();

    setCurrentId(ids.first());

    selectTrack(d->currentIndex);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WPlaylist::isBase() const
{
    Q_D(const WPlaylist); return (d->type == Playlist);
}

bool WPlaylist::isFeed() const
{
    Q_D(const WPlaylist); return (d->type == PlaylistFeed);
}

bool WPlaylist::isSearch() const
{
    Q_D(const WPlaylist); return (d->type == PlaylistSearch);
}

//-------------------------------------------------------------------------------------------------

int WPlaylist::currentId() const
{
    Q_D(const WPlaylist);

    if (d->currentTrack)
    {
         return d->currentTrack->id();
    }
    else return -1;
}

void WPlaylist::setCurrentId(int id)
{
    if (currentId() == id) return;

    Q_D(WPlaylist);

    const WTrack * track = d->itemFromId(id);

    if (d->currentTrack == track) return;

    d->currentTrack = track;
    d->currentIndex = indexOf(track);

    if (track == NULL)
    {
        setCurrentTime(-1);
    }

    d->currentIndexChanged();

    emit currentTrackChanged();
    emit currentIndexChanged();

    emit playlistUpdated();

    save();
}

//-------------------------------------------------------------------------------------------------

int WPlaylist::currentIndex() const
{
    Q_D(const WPlaylist); return d->currentIndex;
}

void WPlaylist::setCurrentIndex(int index)
{
    if (index < 0 || index >= count()) return;

    Q_D(WPlaylist);

    const WTrack * track = d->itemAt(index);

    setCurrentId(track->id());
}

//-------------------------------------------------------------------------------------------------

int WPlaylist::currentTime() const
{
    Q_D(const WPlaylist); return d->currentTime;
}

void WPlaylist::setCurrentTime(int msec)
{
    Q_D(WPlaylist);

    if (d->currentTime == msec) return;

    d->currentTime = msec;

    emit currentTimeChanged();

    save();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WPlaylist::firstSelected() const
{
    Q_D(const WPlaylist);

    if (d->selectedTracks.isEmpty())
    {
         return -1;
    }
    else return indexOf(d->selectedTracks.first());
}

/* Q_INVOKABLE */ int WPlaylist::lastSelected() const
{
    Q_D(const WPlaylist);

    if (d->selectedTracks.isEmpty())
    {
         return -1;
    }
    else return indexOf(d->selectedTracks.last());
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WPlaylist::selectedAligned() const
{
    Q_D(const WPlaylist);

    QList<int> selected = d->getSelected();

    if (selected.count() == 1) return true;

    int lastIndex = -1;

    foreach (int index, selected)
    {
        if (lastIndex != -1)
        {
            if (lastIndex != index) return false;

            lastIndex++;
        }
        else lastIndex = index + 1;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

int WPlaylist::count() const
{
    Q_D(const WPlaylist); return d->tracks.count();
}

//-------------------------------------------------------------------------------------------------

int WPlaylist::maxCount() const
{
    Q_D(const WPlaylist); return d->maxCount;
}

void WPlaylist::setMaxCount(int max)
{
    Q_D(WPlaylist);

    if (d->maxCount == max) return;

    d->maxCount = max;

    emit maxCountChanged();
}

//-------------------------------------------------------------------------------------------------

bool WPlaylist::isEmpty() const
{
    return (count() == 0);
}

bool WPlaylist::isFull() const
{
    Q_D(const WPlaylist);

    return (count() >= d->maxCount);
}

//-------------------------------------------------------------------------------------------------

bool WPlaylist::hasPreviousTrack() const
{
    Q_D(const WPlaylist);

    return hasPreviousIndex(d->currentIndex);
}

bool WPlaylist::hasNextTrack() const
{
    Q_D(const WPlaylist);

    return hasNextIndex(d->currentIndex);
}

//-------------------------------------------------------------------------------------------------

QList<int> WPlaylist::selectedTracks() const
{
    Q_D(const WPlaylist); return d->getSelected();
}

void WPlaylist::setSelectedTracks(const QList<int> & indexes)
{
    if (indexes.isEmpty()) return;

    Q_D(WPlaylist);

    QList<int> selected = d->getSelected();

    QList<int> changed;

    for (int i = 0; i < indexes.count(); i++)
    {
        const WTrack * track = d->itemAt(i);

        if (d->insertSelected(selected, track, i))
        {
            changed.append(i);
        }
    }

    if (changed.count())
    {
        std::sort(changed.begin(), changed.end());

        d->emitSelectedTracksChanged(changed);
    }
}

//-------------------------------------------------------------------------------------------------

int WPlaylist::selectedCount() const
{
    Q_D(const WPlaylist); return d->selectedTracks.count();
}

QString WPlaylist::selectedSources() const
{
    Q_D(const WPlaylist);

    QString sources;

    foreach (const WTrack * track, d->selectedTracks)
    {
        sources.append(track->source() + '\n');
    }

    return sources;
}

//-------------------------------------------------------------------------------------------------

qreal WPlaylist::scrollValue() const
{
    Q_D(const WPlaylist); return d->scrollValue;
}

void WPlaylist::setScrollValue(qreal value)
{
    Q_D(WPlaylist);

    if (d->scrollValue == value) return;

    d->scrollValue = value;

    emit scrollValueChanged();

    save();
}

//-------------------------------------------------------------------------------------------------

QString WPlaylist::currentTitle() const
{
    const WTrack * track = currentTrackPointer();

    if (track)
    {
        return track->title();
    }
    else return QString();
}

QString WPlaylist::currentCover() const
{
    const WTrack * track = currentTrackPointer();

    if (track)
    {
         return track->cover();
    }
    else return QString();
}

int WPlaylist::currentDuration() const
{
    const WTrack * track = currentTrackPointer();

    if (track)
    {
         return track->duration();
    }
    else return -1;
}

//=================================================================================================
// WPlaylistFeed
//=================================================================================================

/* explicit */ WPlaylistFeed::WPlaylistFeed(WLibraryFolder * parent)
    : WPlaylist(new WPlaylistPrivate(this), PlaylistFeed, parent) {}

//=================================================================================================
// WPlaylistSearch
//=================================================================================================

/* explicit */ WPlaylistSearch::WPlaylistSearch(WLibraryFolder * parent)
    : WPlaylist(new WPlaylistPrivate(this), PlaylistSearch, parent) {}

#endif // SK_NO_PLAYLIST

#include "WPlaylist.moc"
