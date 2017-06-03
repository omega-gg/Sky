//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WPlaylistNet.h"

#ifndef SK_NO_PLAYLISTNET

// Qt includes
#include <QXmlStreamWriter>
#include <QList>

// Sk includes
#include <WControllerApplication>
#include <WControllerXml>
#include <WControllerPlaylist>
#include <WControllerDownload>
#include <WAbstractThreadAction>
#include <WAbstractThreadReply>

// 3rdparty includes
#include <qtlockedfile>

// Private includes
#include <private/WTrackNet_p>

// Private include
#include "WPlaylistNet_p.h"

// Namespaces
using namespace QtLP_Private;

//=================================================================================================
// WPlaylistNetWrite and WPlaylistNetWriteReply
//=================================================================================================

struct WThreadActionDataTrack
{
    int id;

    WAbstractTrack::State state;

    QString source;

    QString title;
    QString cover;

    QString author;
    QString feed;

    int duration;

    QDateTime date;

    WAbstractBackend::Quality quality;
};

class WPlaylistNetWrite : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WPlaylistNetWrite(WPlaylistNetPrivate * data)
    {
        this->data = data;

        name    = sk->name   ();
        version = sk->version();
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

public: // Variables
    WPlaylistNetPrivate * data;

    QString path;

    QString name;
    QString version;

    QUrl source;

    QString title;
    QUrl    cover;

    QString label;

    int currentIndex;
    int currentTime;

    qreal scrollValue;

    QList<WThreadActionDataTrack> dataTracks;
};

//-------------------------------------------------------------------------------------------------

class WPlaylistNetWriteReply : public WAbstractThreadReply
{
    Q_OBJECT

public:
    WPlaylistNetWriteReply(WPlaylistNetPrivate * data)
    {
        this->data = data;
    }

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

public: // Variables
    WPlaylistNetPrivate * data;
};

//=================================================================================================
// WPlaylistNetWrite
//=================================================================================================

/* virtual */ WAbstractThreadReply * WPlaylistNetWrite::createReply() const
{
    return new WPlaylistNetWriteReply(data);
}

/* virtual */ bool WPlaylistNetWrite::run()
{
    QtLockedFile file(path);

    QTimer timer;

    timer.start(60000); // 1 minute timeout

    while (file.isLocked() && timer.isActive()) ;

    if (file.isLocked())
    {
        qWarning("WPlaylistNetWrite::run: File is locked %s.", path.C_STR);

        return false;
    }

    if (file.open(QIODevice::WriteOnly) == false)
    {
        qWarning("WPlaylistNetWrite::run: Failed to open file %s.", path.C_STR);

        return false;
    }

    file.lock(QtLockedFile::WriteLock);

    QXmlStreamWriter stream(&file);

    stream.setAutoFormatting(true);

    stream.writeStartDocument();

    stream.writeStartElement(name);

    stream.writeTextElement("version", version);

    stream.writeStartElement("playlist");

    stream.writeTextElement("source", source.toString());

    stream.writeTextElement("title", title);
    stream.writeTextElement("cover", cover.toString());

    stream.writeTextElement("label", label);

    stream.writeTextElement("currentIndex", QString::number(currentIndex));
    stream.writeTextElement("currentTime",  QString::number(currentTime));

    stream.writeTextElement("scrollValue", QString::number(scrollValue));

    stream.writeStartElement("tracks");

    foreach (const WThreadActionDataTrack & data, dataTracks)
    {
        stream.writeStartElement("track");

        stream.writeTextElement("id", QString::number(data.id));

        stream.writeTextElement("state", QString::number(data.state));

        stream.writeTextElement("source", data.source);

        stream.writeTextElement("title", data.title);
        stream.writeTextElement("cover", data.cover);

        stream.writeTextElement("author", data.author);
        stream.writeTextElement("feed",   data.feed);

        stream.writeTextElement("duration", QString::number(data.duration));

        stream.writeTextElement("date", Sk::dateToStringNumber(data.date));

        stream.writeTextElement("quality", QString::number(data.quality));

        stream.writeEndElement(); // track
    }

    stream.writeEndElement(); // tracks
    stream.writeEndElement(); // playlist
    stream.writeEndElement(); // name

    stream.writeEndDocument();

    file.unlock();

    qDebug("PLAYLIST SAVED");

    return true;
}

//=================================================================================================
// WPlaylistNetWriteReply
//=================================================================================================

/* virtual */ void WPlaylistNetWriteReply::onCompleted(bool ok)
{
    data->setSaved(ok);
}

//=================================================================================================
// WPlaylistNetRead and WPlaylistNetReadReply
//=================================================================================================

class WPlaylistNetReadReply;

class WPlaylistNetRead : public WAbstractThreadAction
{
    Q_OBJECT

public:
    WPlaylistNetRead(WPlaylistNetPrivate * data)
    {
        this->data = data;
    }

protected: // WAbstractThreadAction reimplementation
    /* virtual */ WAbstractThreadReply * createReply() const;

protected: // WAbstractThreadAction implementation
    /* virtual */ bool run();

private: // Functions
    bool loadPlaylist(QXmlStreamReader * stream, WPlaylistNetReadReply * reply);
    bool loadTracks  (QXmlStreamReader * stream, WPlaylistNetReadReply * reply);

public: // Variables
    WPlaylistNetPrivate * data;

    QString path;
};

//-------------------------------------------------------------------------------------------------

class WPlaylistNetReadReply : public WAbstractThreadReply
{
    Q_OBJECT

public:
    WPlaylistNetReadReply(WPlaylistNetPrivate * data)
    {
        this->data = data;
    }

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

public: // Variables
    WPlaylistNetPrivate * data;

    QUrl source;

    QString title;
    QUrl    cover;

    QString label;

    int currentIndex;
    int currentTime;

    qreal scrollValue;

    QList<WTrackNet> dataTracks;
};

//=================================================================================================
// WPlaylistNetRead
//=================================================================================================

/* virtual */ WAbstractThreadReply * WPlaylistNetRead::createReply() const
{
    return new WPlaylistNetReadReply(data);
}

/* virtual */ bool WPlaylistNetRead::run()
{
    WPlaylistNetReadReply * reply = qobject_cast<WPlaylistNetReadReply *> (this->reply());

    QtLockedFile file(path);

    QTimer timer;

    timer.start(60000); // 1 minute timeout

    while (file.isLocked() && timer.isActive()) ;

    if (file.isLocked())
    {
        qWarning("WPlaylistNetRead::run: File is locked %s.", path.C_STR);

        return false;
    }

    if (file.open(QIODevice::ReadOnly) == false)
    {
        qWarning("WPlaylistNetRead::run: Failed to open file %s.", path.C_STR);

        return false;
    }

    file.lock(QtLockedFile::ReadLock);

    QXmlStreamReader stream(&file);

    if (loadPlaylist(&stream, reply) == false || loadTracks(&stream, reply) == false)
    {
        qWarning("WPlaylistNetRead::run: Invalid file %s.", path.C_STR);

        return false;
    }

    file.unlock();

    qDebug("PLAYLIST LOADED");

    return true;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

bool WPlaylistNetRead::loadPlaylist(QXmlStreamReader * stream, WPlaylistNetReadReply * reply)
{
    //---------------------------------------------------------------------------------------------
    // source

    if (wControllerXml->readNextStartElement(stream, "source") == false) return false;

    reply->source = wControllerXml->readNextString(stream);

    //---------------------------------------------------------------------------------------------
    // title

    if (wControllerXml->readNextStartElement(stream, "title") == false) return false;

    reply->title = wControllerXml->readNextString(stream);

    //---------------------------------------------------------------------------------------------
    // cover

    if (wControllerXml->readNextStartElement(stream, "cover") == false) return false;

    reply->cover = wControllerXml->readNextString(stream);

    //---------------------------------------------------------------------------------------------
    // label

    if (wControllerXml->readNextStartElement(stream, "label") == false) return false;

    reply->label = wControllerXml->readNextString(stream);

    //---------------------------------------------------------------------------------------------
    // currentIndex

    if (wControllerXml->readNextStartElement(stream, "currentIndex") == false) return false;

    reply->currentIndex = wControllerXml->readNextInt(stream);

    //---------------------------------------------------------------------------------------------
    // currentTime

    if (wControllerXml->readNextStartElement(stream, "currentTime") == false) return false;

    reply->currentTime = wControllerXml->readNextInt(stream);

    //---------------------------------------------------------------------------------------------
    // scrollValue

    if (wControllerXml->readNextStartElement(stream, "scrollValue") == false) return false;

    reply->scrollValue = wControllerXml->readNextFloat(stream);

    return true;
}

bool WPlaylistNetRead::loadTracks(QXmlStreamReader * stream, WPlaylistNetReadReply * reply)
{
    while (wControllerXml->readNextStartElement(stream, "track"))
    {
        WTrackNet track;

        WTrackNetPrivate * p = track.d_func();

        //-----------------------------------------------------------------------------------------
        // id

        if (wControllerXml->readNextStartElement(stream, "id") == false) return false;

        p->id = wControllerXml->readNextInt(stream);

        //-----------------------------------------------------------------------------------------
        // state

        if (wControllerXml->readNextStartElement(stream, "state") == false) return false;

        p->state = static_cast<WAbstractTrack::State> (wControllerXml->readNextInt(stream));

        //-----------------------------------------------------------------------------------------
        // source

        if (wControllerXml->readNextStartElement(stream, "source") == false) return false;

        p->source = wControllerXml->readNextString(stream);

        //-----------------------------------------------------------------------------------------
        // title

        if (wControllerXml->readNextStartElement(stream, "title") == false) return false;

        p->title = wControllerXml->readNextString(stream);

        //-----------------------------------------------------------------------------------------
        // cover

        if (wControllerXml->readNextStartElement(stream, "cover") == false) return false;

        p->cover = wControllerXml->readNextString(stream);

        //-----------------------------------------------------------------------------------------
        // author

        if (wControllerXml->readNextStartElement(stream, "author") == false) return false;

        p->author = wControllerXml->readNextString(stream);

        //-----------------------------------------------------------------------------------------
        // feed

        if (wControllerXml->readNextStartElement(stream, "feed") == false) return false;

        p->feed = wControllerXml->readNextString(stream);

        //-----------------------------------------------------------------------------------------
        // duration

        if (wControllerXml->readNextStartElement(stream, "duration") == false) return false;

        p->duration = wControllerXml->readNextInt(stream);

        //-----------------------------------------------------------------------------------------
        // date

        if (wControllerXml->readNextStartElement(stream, "date") == false) return false;

        p->date = wControllerXml->readNextDate(stream);

        //-----------------------------------------------------------------------------------------
        // quality

        if (wControllerXml->readNextStartElement(stream, "quality") == false) return false;

        int quality = wControllerXml->readNextInt(stream);

        p->quality = static_cast<WAbstractBackend::Quality> (quality);

        //-----------------------------------------------------------------------------------------

        reply->dataTracks.append(track);
    }

    return true;
}

//=================================================================================================
// WPlaylistNetReadReply
//=================================================================================================

/* virtual */ void WPlaylistNetReadReply::onCompleted(bool ok)
{
    WPlaylistNet * q = data->q_func();

    bool saveEnabled = q->saveEnabled();

    q->setSaveEnabled(false);

    if (ok)
    {
        if (data->cacheLoad)
        {
            if (data->source.isEmpty()) q->loadSource(source, false);

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
            q->loadSource(source, false);

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
        q->loadSource(QUrl(), false);

        q->setTitle(QString());
        q->setCover(QUrl   ());

        q->setLabel(QString());

        q->setScrollValue(0);

        q->setCurrentIndex(-1);
        q->setCurrentTime (-1);
    }

    q->setSaveEnabled(saveEnabled);

    data->setLoaded(ok);
}

//=================================================================================================
// WPlaylistNetPrivate
//=================================================================================================

WPlaylistNetPrivate::WPlaylistNetPrivate(WPlaylistNet * p) : WAbstractPlaylistPrivate(p) {}

void WPlaylistNetPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

const WTrackNet * WPlaylistNetPrivate::getTrackConst(int index) const
{
    if (index < 0 || index >= tracks.count()) return NULL;

    return &(tracks[index]);
}

WTrackNet * WPlaylistNetPrivate::getTrack(int index)
{
    if (index < 0 || index >= tracks.count()) return NULL;

    return &(tracks[index]);
}

//-------------------------------------------------------------------------------------------------

bool WPlaylistNetPrivate::containsTrackPointer(WTrackNet * track) const
{
    for (int i = 0; i < tracks.count(); i++)
    {
        if (&(tracks.at(i)) == track) return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

void WPlaylistNetPrivate::loadTracks(const QList<WTrackNet> & tracks)
{
    Q_Q(WPlaylistNet);

    if (this->tracks.count())
    {
        q->setCurrentId(-1);

        selectedTracks.clear();

        foreach (WAbstractPlaylistWatcher * watcher, watchers)
        {
            watcher->beginTracksClear();
        }

        this->tracks.clear();

        foreach (WAbstractPlaylistWatcher * watcher, watchers)
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

//=================================================================================================
// WPlaylistNet
//=================================================================================================

/* explicit */ WPlaylistNet::WPlaylistNet(WLibraryFolder * parent)
    : WAbstractPlaylist(new WPlaylistNetPrivate(this), PlaylistNet, parent)
{
    Q_D(WPlaylistNet); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected

WPlaylistNet::WPlaylistNet(WPlaylistNetPrivate * p, Type type, WLibraryFolder * parent)
    : WAbstractPlaylist(p, type, parent)
{
    Q_D(WPlaylistNet); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

void WPlaylistNet::addTrack(const WTrackNet & track)
{
    insertTracks(count(), QList<WTrackNet>() << track);
}

void WPlaylistNet::addTracks(const QList<WTrackNet> & tracks)
{
    insertTracks(count(), tracks);
}

//-------------------------------------------------------------------------------------------------

void WPlaylistNet::insertTrack(int index, const WTrackNet & track)
{
    insertTracks(index, QList<WTrackNet>() << track);
}

void WPlaylistNet::insertTracks(int index, const QList<WTrackNet> & tracks)
{
    Q_D(WPlaylistNet);

    int countAdd = tracks.count();

    if (countAdd == 0) return;

    int count = d->tracks.count();

    if (checkFull(countAdd)) return;

    if (index < 0 || index > count)
    {
        if (d->type == PlaylistFeed)
        {
             index = 0;
        }
        else index = count;
    }

    beginTracksInsert(index, index + countAdd - 1);

    int oldIndex = index;

    QList<WTrackNet *> tracksDefault;

    foreach (const WTrackNet & track, tracks)
    {
        d->tracks.insert(index, track);

        WTrackNet * newTrack = &(d->tracks[index]);

        WTrackNetPrivate * p = newTrack->d_func();

        p->playlist = this;

        int id = p->id;

        if (id == -1)
        {
            p->id = d->ids.generateId();
        }
        else if (d->ids.insertId(id) == false)
        {
            qWarning("WPlaylistNet::insertTracks: Id is already taken '%d'.", id);

            p->id = d->ids.generateId();
        }

        if (p->state != WAbstractTrack::Loaded)
        {
            tracksDefault.append(newTrack);
        }

        index++;
    }

    endTracksInsert();

    updateCover();
    updateIndex();

    emit countChanged();

    emit tracksInserted(oldIndex, countAdd);

    save();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ int WPlaylistNet::addSource(const QUrl & url)
{
    return insertSource(count(), url);
}

/* Q_INVOKABLE */ int WPlaylistNet::insertSource(int index, const QUrl & url)
{
    Q_D(WPlaylistNet);

    QList<WTrackNet> tracks;

    QStringList urls = url.toString().split('\n', QString::SkipEmptyParts);

    foreach (const QString & source, urls)
    {
        WTrackNet track(source, WAbstractTrack::Default);

        tracks.append(track);
    }

    int count = tracks.count();

    if (checkFull(count))
    {
        count = d->maxCount - this->count() - 1;

        tracks = tracks.mid(0, count);
    }

    insertTracks(index, tracks);

    return count;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylistNet::removeTrack(int index)
{
    removeTracks(QList<int>() << index);
}

/* Q_INVOKABLE */ void WPlaylistNet::removeTracks(const QList<int> & indexes)
{
    Q_D(WPlaylistNet);

    if (indexes.isEmpty()) return;

    QList<int> sortedIndexes = indexes;

    qSort(sortedIndexes.begin(), sortedIndexes.end());

    QList<WTrackNet *> tracks;

    QList<int> changed;

    foreach (int index, sortedIndexes)
    {
        if (index < 0 || index >= d->tracks.count()) continue;

        WTrackNet * track = &(d->tracks[index]);

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

    foreach (WTrackNet * track, tracks)
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

/* Q_INVOKABLE */ void WPlaylistNet::removeTracks(const QList<QVariant> & indexes)
{
    QList<int> list;

    foreach (QVariant variant, indexes)
    {
        list.append(variant.toInt());
    }

    removeTracks(list);
}

/* Q_INVOKABLE */ void WPlaylistNet::removeSelectedTracks()
{
    removeTracks(selectedTracks());
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylistNet::updateCover()
{
    Q_D(WPlaylistNet);

    if (d->cover.isEmpty() == false
        ||
        (d->type != PlaylistNet && d->type != PlaylistFeed)) return;

    foreach (const WTrackNet & track, d->tracks)
    {
        QUrl cover = track.d_func()->cover;

        if (cover.isValid())
        {
            setCover(cover);

            return;
        }
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylistNet::loadTrack(int at)
{
    Q_D(WPlaylistNet);

    if (at < 0 || at >= d->tracks.count()) return;

    WTrackNet * track = &(d->tracks[at]);

    if (track->isDefault() == false) return;

    wControllerPlaylist->d_func()->applySourceTrack(this, track, track->source());
}

/* Q_INVOKABLE */ void WPlaylistNet::loadTracks(int at, int count)
{
    Q_D(WPlaylistNet);

    if (at < 0 || at >= d->tracks.count()) return;

    int index = at - count / 2;

    if (index < 0) index = 0;

    while (index < at)
    {
        const WTrackNet & track = d->tracks[index];

        if (track.isDefault()) break;

        index++;
    }

    while (index < d->tracks.count() && count)
    {
        WTrackNet * track = &(d->tracks[index]);

        if (track->isDefault())
        {
            wControllerPlaylist->d_func()->applySourceTrack(this, track, track->source());
        }

        count--;

        index++;
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylistNet::abortTracks()
{
    wControllerPlaylist->d_func()->abortQueriesTracks(this);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylistNet::loadCover(const QString & backend, int at)
{
    Q_D(WPlaylistNet);

    if (at < 0 || at >= d->tracks.count()) return;

    WTrackNet * track = &(d->tracks[at]);

    if (track->isLoaded() == false || track->cover().isEmpty() == false) return;

    QUrl url = WControllerPlaylist::createSource(backend, "cover", "track", track->title());

    wControllerPlaylist->d_func()->applySourceTrack(this, track, url);
}

/* Q_INVOKABLE */ void WPlaylistNet::loadCovers(const QString & backend, int at, int count)
{
    Q_D(WPlaylistNet);

    if (at < 0 || at >= d->tracks.count()) return;

    int index = at - count / 2;

    if (index < 0) index = 0;

    while (index < at)
    {
        const WTrackNet & track = d->tracks[index];

        if (track.isLoaded() && track.cover().isEmpty()) break;

        index++;
    }

    while (index < d->tracks.count() && count)
    {
        WTrackNet * track = &(d->tracks[index]);

        if (track->isLoaded() && track->cover().isEmpty())
        {
            QUrl url = WControllerPlaylist::createSource(backend,
                                                         "cover", "track", track->title());

            wControllerPlaylist->d_func()->applySourceTrack(this, track, url);
        }

        count--;

        index++;
    }
}

//-------------------------------------------------------------------------------------------------

bool WPlaylistNet::contains(const WTrackNet & track) const
{
    Q_D(const WPlaylistNet); return d->tracks.contains(track);
}

/* Q_INVOKABLE */ bool WPlaylistNet::containsSource(const QUrl & source) const
{
    Q_D(const WPlaylistNet);

    foreach (const WTrackNet & track, d->tracks)
    {
        if (track.d_func()->source == source) return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

WTrackNet WPlaylistNet::getTrackAt(int index) const
{
    Q_D(const WPlaylistNet);

    if (index < 0 || index >= d->tracks.count())
    {
        return WTrackNet();
    }

    WTrackNet track = d->tracks.at(index);

    track.d_func()->id = -1;

    return track;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WPlaylistNet * WPlaylistNet::duplicate() const
{
    Q_D(const WPlaylistNet);

    WPlaylistNet * playlist = WPlaylistNet::create(d->type);

    if (d->stateQuery == Loading)
    {
         playlist->d_func()->stateQuery = Default;
    }
    else playlist->d_func()->stateQuery = d->stateQuery;

    playlist->d_func()->source = d->source;

    playlist->setTitle(d->title);
    playlist->setCover(d->cover);

    playlist->addTracks(d->tracks);

    playlist->setCurrentIndex(d->currentIndex);
    playlist->setCurrentTime (d->currentTime);

    playlist->setScrollValue(d->scrollValue);

    return playlist;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylistNet::copyTracksTo(const QList<int> & indexes,
                                                  WPlaylistNet     * destination, int to) const
{
    Q_ASSERT(destination);

    QList<WTrackNet> tracks;

    foreach (int index, indexes)
    {
        tracks.append(getTrackAt(index));
    }

    destination->insertTracks(to, tracks);
}

/* Q_INVOKABLE */ void WPlaylistNet::copyTrackTo(int            from,
                                                 WPlaylistNet * destination, int to) const
{
    Q_ASSERT(destination);

    Q_D(const WPlaylistNet);

    if (from < 0 || from >= d->tracks.count()) return;

    WTrackNet track = getTrackAt(from);

    destination->insertTrack(to, track);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WPlaylistNet::copyAllTo(WPlaylistNet * destination, int to) const
{
    Q_ASSERT(destination);

    Q_D(const WPlaylistNet);

    destination->insertTracks(to, d->tracks);
}

/* Q_INVOKABLE */ void WPlaylistNet::copySelectedTo(WPlaylistNet * destination, int to) const
{
    Q_ASSERT(destination);

    Q_D(const WPlaylistNet);

    copyTracksTo(d->getSelected(), destination, to);
}

//-------------------------------------------------------------------------------------------------
// Track interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WPlaylistNet::trackIsValid(int index) const
{
    const WAbstractTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->isValid();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QVariantMap WPlaylistNet::trackData(int index) const
{
    const WAbstractTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->toMap();
    }
    else return QVariantMap();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ WAbstractTrack::State WPlaylistNet::trackState(int index) const
{
    const WAbstractTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->state();
    }
    else return WAbstractTrack::Default;
}

/* Q_INVOKABLE */ void WPlaylistNet::setTrackState(int index, WAbstractTrack::State state)
{
    Q_D(WPlaylistNet);

    WTrackNet * track = d->getTrack(index);

    if (track == NULL || track->state() == state) return;

    track->setState(state);

    updateTrack(index);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WPlaylistNet::trackIsDefault(int index) const
{
    const WAbstractTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->isDefault();
    }
    else return false;
}

/* Q_INVOKABLE */ bool WPlaylistNet::trackIsLoading(int index) const
{
    const WAbstractTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->isLoading();
    }
    else return false;
}

/* Q_INVOKABLE */ bool WPlaylistNet::trackIsLoaded(int index) const
{
    const WAbstractTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->isLoaded();
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QUrl WPlaylistNet::trackSource(int index) const
{
    const WAbstractTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->source();
    }
    else return QUrl();
}

/* Q_INVOKABLE */ void WPlaylistNet::setTrackSource(int index, const QUrl & source)
{
    Q_D(WPlaylistNet);

    WTrackNet * track = d->getTrack(index);

    if (track == NULL || track->source() == source) return;

    track->setSource(source);

    updateTrack(index);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WPlaylistNet::trackTitle(int index) const
{
    const WAbstractTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->title();
    }
    else return QString();
}

/* Q_INVOKABLE */ void WPlaylistNet::setTrackTitle(int index, const QString & title)
{
    Q_D(WPlaylistNet);

    WTrackNet * track = d->getTrack(index);

    if (track == NULL || track->title() == title) return;

    track->setTitle(title);

    updateTrack(index);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QUrl WPlaylistNet::trackCover(int index) const
{
    const WAbstractTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->cover();
    }
    else return QUrl();
}

/* Q_INVOKABLE */ void WPlaylistNet::setTrackCover(int index, const QUrl & cover)
{
    Q_D(WPlaylistNet);

    WTrackNet * track = d->getTrack(index);

    if (track == NULL || track->cover() == cover) return;

    track->setCover(cover);

    updateTrack(index);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WPlaylistNet::trackAuthor(int index) const
{
    const WAbstractTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->author();
    }
    else return QString();
}

/* Q_INVOKABLE */ void WPlaylistNet::setTrackAuthor(int index, const QString & author)
{
    Q_D(WPlaylistNet);

    WTrackNet * track = d->getTrack(index);

    if (track == NULL || track->author() == author) return;

    track->setAuthor(author);

    updateTrack(index);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WPlaylistNet::trackFeed(int index) const
{
    const WAbstractTrack * track = trackPointerAt(index);

    if (track)
    {
         return track->feed();
    }
    else return QString();
}

/* Q_INVOKABLE */ void WPlaylistNet::setTrackFeed(int index, const QString & feed)
{
    Q_D(WPlaylistNet);

    WTrackNet * track = d->getTrack(index);

    if (track == NULL || track->feed() == feed) return;

    track->setFeed(feed);

    updateTrack(index);
}

//-------------------------------------------------------------------------------------------------
// QML

/* Q_INVOKABLE */ void WPlaylistNet::copyTracksTo(const QVariantList & tracks,
                                                  WPlaylistNet       * destination, int to) const
{
    copyTracksTo(Sk::variantsToInts(tracks), destination, to);
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ WPlaylistNet * WPlaylistNet::create(Type type)
{
    if (type == PlaylistFeed)
    {
        return new WPlaylistFeed;
    }
    else if (type == PlaylistSearch)
    {
        return new WPlaylistSearch;
    }
    else return new WPlaylistNet;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ WPlaylistNet * WPlaylistNet::fromPlaylist(WAbstractPlaylist * playlist)
{
    if (playlist)
    {
         return playlist->toPlaylistNet();
    }
    else return NULL;
}

//-------------------------------------------------------------------------------------------------
// Public WAbstractPlaylist implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ int WPlaylistNet::indexOf(const WAbstractTrack * track) const
{
    Q_D(const WPlaylistNet);

    for (int i = 0; i < d->tracks.count(); i++)
    {
        if (&(d->tracks.at(i)) == track)
        {
            return i;
        }
    }

    return -1;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ int WPlaylistNet::indexFromId(int id) const
{
    Q_D(const WPlaylistNet);

    for (int i = 0; i < d->tracks.count(); i++)
    {
        if (d->tracks.at(i).d_func()->id == id)
        {
            return i;
        }
    }

    return -1;
}

//-------------------------------------------------------------------------------------------------
// Protected WLocalObject reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ WAbstractThreadAction * WPlaylistNet::onSave(const QString & path)
{
    Q_D(WPlaylistNet);

    WPlaylistNetWrite * action = new WPlaylistNetWrite(d);

    action->path = path;

    action->source = d->source;

    action->title = d->title;
    action->cover = d->cover;

    action->label = d->label;

    action->currentIndex = d->currentIndex;
    action->currentTime  = d->currentTime;

    action->scrollValue = d->scrollValue;

    foreach (const WTrackNet & track, d->tracks)
    {
        const WTrackNetPrivate * p = track.d_func();

        WThreadActionDataTrack data;

        data.id = p->id;

        data.state = p->state;

        data.source = p->source.toString();

        data.title = p->title;
        data.cover = p->cover.toString();

        data.author = p->author;
        data.feed   = p->feed;

        data.duration = p->duration;

        data.date = p->date;

        data.quality = p->quality;

        action->dataTracks.append(data);
    }

    return action;
}

/* virtual */ WAbstractThreadAction * WPlaylistNet::onLoad(const QString & path)
{
    Q_D(WPlaylistNet);

    WPlaylistNetRead * action = new WPlaylistNetRead(d);

    action->path = path;

    return action;
}

//-------------------------------------------------------------------------------------------------
// Protected WLibraryItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WPlaylistNet::applySource(const QUrl & source)
{
    return wControllerPlaylist->d_func()->applySourcePlaylist(this, source);
}

/* virtual */ bool WPlaylistNet::applyQuery(const WBackendNetQuery & query)
{
    return wControllerPlaylist->d_func()->applyQueryPlaylist(this, query);
}

/* virtual */ bool WPlaylistNet::stopQuery()
{
    return wControllerPlaylist->d_func()->abortQueriesPlaylist(this);
}

//-------------------------------------------------------------------------------------------------
// Protected WAbstractPlaylist implementation
//-------------------------------------------------------------------------------------------------

/* virtual */ int WPlaylistNet::itemCount() const
{
    Q_D(const WPlaylistNet); return d->tracks.count();
}

//-------------------------------------------------------------------------------------------------

/* virtual */ const WAbstractTrack * WPlaylistNet::itemFromId(int id) const
{
    Q_D(const WPlaylistNet);

    for (int i = 0; i < d->tracks.count(); i++)
    {
        const WAbstractTrack & track = d->tracks.at(i);

        if (track.id() == id)
        {
            return &(track);
        }
    }

    return NULL;
}

/* virtual */ const WAbstractTrack * WPlaylistNet::itemAt(int index) const
{
    Q_D(const WPlaylistNet); return &(d->tracks[index]);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WPlaylistNet::moveItemTo(int from, int to)
{
    Q_D(WPlaylistNet); d->tracks.move(from, to);
}

//-------------------------------------------------------------------------------------------------

/* virtual */ void WPlaylistNet::clearItems()
{
    Q_D(WPlaylistNet);

    wControllerPlaylist->d_func()->abortQueriesItem(this);

    d->tracks.clear();

    d->ids.clear();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QList<WTrackNet> WPlaylistNet::tracks() const
{
    Q_D(const WPlaylistNet); return d->tracks;
}

//=================================================================================================
// WPlaylistFeed
//=================================================================================================

/* explicit */ WPlaylistFeed::WPlaylistFeed(WLibraryFolder * parent)
    : WPlaylistNet(new WPlaylistNetPrivate(this), PlaylistFeed, parent) {}

//=================================================================================================
// WPlaylistSearch
//=================================================================================================

/* explicit */ WPlaylistSearch::WPlaylistSearch(WLibraryFolder * parent)
    : WPlaylistNet(new WPlaylistNetPrivate(this), PlaylistSearch, parent) {}

#endif // SK_NO_PLAYLISTNET

#include "WPlaylistNet.moc"
