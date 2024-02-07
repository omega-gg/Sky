//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkMedia.

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

#include "WControllerMedia.h"

#ifndef SK_NO_CONTROLLERMEDIA

// Qt includes
#include <QThread>

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>
#include <WControllerNetwork>
#include <WControllerDownload>
#include <WControllerPlaylist>
#include <WBackendUniversal>
#include <WRegExp>
#include <WZipper>
#include <WUnzipper>
#ifndef SK_NO_PLAYER
#include <WVlcEngine>
#include <WVlcPlayer>
#endif

W_INIT_CONTROLLER(WControllerMedia)

//-------------------------------------------------------------------------------------------------
// Static variables

static const int CONTROLLERMEDIA_MAX_CACHE  = 100;
static const int CONTROLLERMEDIA_MAX_SLICES =  64;

static const int CONTROLLERMEDIA_MAX_QUERY  = 100;
static const int CONTROLLERMEDIA_MAX_RELOAD =  10;

static const int CONTROLLERMEDIA_CHANNEL_DURATION = 604800000; // 7 days in milliseconds

//=================================================================================================
// WMediaReply
//=================================================================================================
// Private

WMediaReply::WMediaReply(QObject * parent) : QObject(parent) {}

//-------------------------------------------------------------------------------------------------
// Public

/* virtual */ WMediaReply::~WMediaReply()
{
    W_GET_CONTROLLER(WControllerMedia, controller);

    if (controller) controller->d_func()->clearReply(this);
}

//-------------------------------------------------------------------------------------------------
// Virtual interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WMediaReply::toVbml() const
{
    QString vbml = WControllerPlaylist::vbml();

    Sk::bmlPair(vbml, "type", "media", "\n\n");

    QString tab = Sk::tabs(1);

    if (_medias.isEmpty() == false)
    {
        Sk::bmlTag(vbml, "medias");

        QHashIterator<WAbstractBackend::Quality, QString> i(_medias);

        while (i.hasNext())
        {
            i.next();

            Sk::bmlPair(vbml, tab + WAbstractBackend::qualityToString(i.key()), i.value(), "\n\n");
        }
    }

    if (_audios.isEmpty() == false)
    {
        Sk::bmlTag(vbml, "audios", "\n");

        QHashIterator<WAbstractBackend::Quality, QString> i(_audios);

        while (i.hasNext())
        {
            i.next();

            Sk::bmlPair(vbml, tab + WAbstractBackend::qualityToString(i.key()), i.value(), "\n\n");
        }
    }

    // NOTE: We clear the last '\n'.
    vbml.chop(1);

    return vbml;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QString WMediaReply::url() const
{
    return _url;
}

QString WMediaReply::urlSource() const
{
    return _urlSource;
}

//-------------------------------------------------------------------------------------------------

WTrack::Type WMediaReply::type() const
{
    return _type;
}

WTrack::Type WMediaReply::typeSource() const
{
    return _typeSource;
}

bool WMediaReply::isVbml() const
{
    return _vbml;
}

QString WMediaReply::timeZone() const
{
    return _timeZone;
}

int WMediaReply::currentTime() const
{
    return _currentTime;
}

int WMediaReply::duration() const
{
    return _duration;
}

int WMediaReply::timeA() const
{
    return _timeA;
}

int WMediaReply::timeB() const
{
    return _timeB;
}

int WMediaReply::start() const
{
    return _start;
}

QString WMediaReply::context() const
{
    return _context;
}

QHash<WAbstractBackend::Quality, QString> WMediaReply::medias() const
{
    return _medias;
}

QHash<WAbstractBackend::Quality, QString> WMediaReply::audios() const
{
    return _audios;
}

//-------------------------------------------------------------------------------------------------

bool WMediaReply::isLoaded() const
{
    return _loaded;
}

//-------------------------------------------------------------------------------------------------

bool WMediaReply::hasError() const
{
    return (_error.isEmpty() == false);
}

QString WMediaReply::error() const
{
    return _error;
}

//=================================================================================================
// WControllerMediaSource
//=================================================================================================

WControllerMediaSource::WControllerMediaSource(const WYamlNode * node, int index)
{
    Q_ASSERT(node);

    this->node = node;

    // NOTE: An interactive id has a maximum length of 10 characters.
    id = node->extractString("id").left(10);

    this->index = index;

    duration = -2;

    at  = -1;
    end = -1;
}

int WControllerMediaSource::getDuration(int at) const
{
    if (duration != -1) return duration;

    if (end == -1)
    {
        return 0;
    }
    else return end - at;
}

//=================================================================================================
// WControllerMediaData
//=================================================================================================

WControllerMediaData::WControllerMediaData()
{
    type       = WTrack::Unknown;
    typeSource = WTrack::Unknown;

    vbml = false;

    currentTime = 0;

    duration = -1;

    timeA =  0;
    timeB = -1;

    start = 0;
}

//-------------------------------------------------------------------------------------------------
// Interface

void WControllerMediaData::applyVbml(const QByteArray & array, const QString & url,
                                                               const QString & argument)
{
    QString content = Sk::readBml(array);

    //---------------------------------------------------------------------------------------------
    // Api

    QString api = WControllerPlaylist::vbmlVersion(content);

    if (api.isEmpty())
    {
        if (WControllerPlaylist::textIsRedirect(content, url))
        {
            origin = content;

            return;
        }

        // NOTE: If it's HTML we try to extract a VBML link.
        if (array.contains("<html>"))
        {
            origin = WControllerPlaylistData::extractHtmlLink(array, url);

            if (origin.isEmpty() == false) return;
        }
    }
    else
    {
        if (Sk::versionIsHigher(WControllerPlaylist::versionApi(), api))
        {
            WControllerPlaylist::vbmlPatch(content, api);

            applyVbml(content.toUtf8(), url, argument);

            return;
        }

        if (Sk::versionIsLower(WControllerPlaylist::versionApi(), api))
        {
            qWarning("WControllerMediaReply::applyVbml: The required API is too high.");
        }
    }

    WYamlReader reader(content.toUtf8());

    //---------------------------------------------------------------------------------------------
    // Source

    vbml = true;

    QString string = reader.extractString("origin");

    if (string.isEmpty() == false
        &&
        // NOTE: The origin has to be different than the current URL.
        WControllerNetwork::removeUrlPrefix(url) != WControllerNetwork::removeUrlPrefix(string))
    {
        origin = string;

        return;
    }

    const WYamlNode * node = reader.at("source");

    // NOTE: If the parsing fails we add the current url as the default source.
    if (node == NULL)
    {
        medias.insert(WAbstractBackend::QualityDefault, url);

        return;
    }

    const QList<WYamlNode> & children = node->children;

    if (children.isEmpty())
    {
        source = node->value;

        // NOTE: By default, an image is played like a hub.
        if (WControllerFile::urlIsImage(source))
        {
            type = WTrack::Hub;
        }
        else type = WTrack::typeFromString(reader.extractString("type"));

        applyMedia(reader.node(), source);

        return;
    }

    type = WTrack::typeFromString(reader.extractString("type"));

    start += reader.extractMsecs("at");

    if (duration == -1)
    {
        // NOTE: The Channel type is only taken into account for the root source.
        if (type == WTrack::Channel)
        {
            timeZone = reader.extractString("timezone");

            QDateTime date = WControllerApplication::currentDateUtc(timeZone);

            currentTime = WControllerApplication::getMsecsWeek(date);

            duration = CONTROLLERMEDIA_CHANNEL_DURATION;
        }
        // NOTE: The Interactive type is only taken into account for the root source.
        else if (type == WTrack::Interactive)
        {
            QString contextBase = reader.extractString("context");

            QStringList tags = extractTags(reader);

            QList<WControllerMediaSource> sources = extractSources(reader);

            QHash<QString, WControllerMediaSource *> hash = generateHash(sources);

            QList<WControllerMediaObject> timeline;

            if (context.isEmpty() == false)
            {
                timeline = generateTimeline(hash,
                                            WUnzipper::extractBase64(context.toUtf8()), tags);

                if (timeline.isEmpty())
                {
                    timeline = generateTimeline(hash, contextBase, tags);

                    if (timeline.isEmpty()) return;
                }
            }
            else timeline = generateTimeline(hash, contextBase, tags);

            QList<int> starts;
            QList<int> durations;

            duration = extractDuration(timeline, &starts, &durations, start);

            if (duration == -1) return;

            int index = extractSourceTimeline(timeline, durations, starts);

            if (timeB != -1 || argument.isEmpty()) return;

            QString result = extractResult(reader, argument);

            QList<WControllerMediaObject> timelineNew = generateTimeline(hash, result, tags);

            if (timelineNew.isEmpty() == false)
            {
                context = generateContext(timeline);

                return;
            }

            starts   .clear();
            durations.clear();

            int durationNew = extractDuration(timelineNew, &starts, &durations, 0);

            if (durationNew == -1)
            {
                context = generateContext(timeline);

                return;
            }

            timeA = timeB;

            currentTime = timeA;

            duration = timeA + durationNew;

            extractSourceTimeline(timelineNew, durations, starts);

            timeline.erase(timeline.begin() + index, timeline.end());

            timeline.append(timelineNew);

            context = generateContext(timeline);

            return;
        }

        duration = WControllerPlaylist::vbmlDuration(reader.node(), start);

        if (duration == 0)
        {
            QList<int> starts;
            QList<int> durations;

            duration = 0;

            int startSource = start;

            foreach (const WYamlNode & child, children)
            {
                startSource += child.extractMsecs("at");

                starts.append(startSource);

                int durationSource = WControllerPlaylist::vbmlDuration(child, startSource);

                durations.append(durationSource);

                if (durationSource > 0)
                {
                    startSource = 0;

                    duration += durationSource;
                }
                else startSource = -durationSource;
            }

            if (duration == 0)
            {
                duration = -1;
            }
            else extractSourceDuration(children, durations, starts);

            return;
        }
    }

    extractSource(children);

    if (source.isEmpty()) applyEmpty();
}

void WControllerMediaData::applyM3u(const QByteArray & array, const QString & url)
{
    QString content = Sk::readUtf8(array);

    QString urlBase = WControllerNetwork::extractBaseUrl(url) + '/';

    QString fileName = WControllerNetwork::extractUrlFileName(url);

    fileName = WControllerNetwork::removeFileExtension(fileName);

    QStringList list = Sk::slicesIn(content, WRegExp("EXT-X-STREAM-INF"), WRegExp("[#$]"));

    foreach (const QString & media, list)
    {
        QString resolution = Sk::sliceIn(media, WRegExp("RESOLUTION="), WRegExp("[,\\n]"));

        resolution = resolution.mid(resolution.indexOf('x') + 1);

        WAbstractBackend::Quality quality = WAbstractBackend::qualityFromString(resolution);

        if (quality == WAbstractBackend::QualityDefault || medias.contains(quality)) continue;

        QString source;

        int index = media.lastIndexOf("http");

        if (index == -1)
        {
            index = media.lastIndexOf(fileName);

            if (index == -1) continue;

            source = media.mid(index);

            if (source.endsWith('\n')) source.chop(1);

            if (source.isEmpty()) continue;

            source.prepend(urlBase);
        }
        else
        {
            source = media.mid(index);

            if (source.endsWith('\n')) source.chop(1);

            if (source.isEmpty()) continue;
        }

        medias.insert(quality, source);
    }

    // NOTE: If we couldn't parse a resolution we add the current url as the default source.
    if (medias.isEmpty())
    {
        medias.insert(WAbstractBackend::QualityDefault, url);
    }
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* static */ QStringList WControllerMediaData::extractTags(const WYamlReader & reader)
{
    QStringList tags;

    QStringList list = reader.extractList("tags");

    foreach (const QString & tag, list)
    {
        if (tags.contains(tag)) continue;

        // NOTE: An interactive id has a maximum length of 10 characters.
        tags.append(tag.left(10));
    }

    return tags;
}

/* static */ QList<WControllerMediaSource>
WControllerMediaData::extractSources(const WYamlReader & reader)
{
    QList<WControllerMediaSource> list;

    const WYamlNode * node = reader.at("source");

    if (node == NULL) return list;

    const QList<WYamlNode> & children = node->children;

    if (children.isEmpty())
    {
        WControllerMediaSource source(node, list.count());

        list.append(source);

        return list;
    }

    foreach (const WYamlNode & child, children)
    {
        WControllerMediaSource source(&child, list.count());

        list.append(source);
    }

    return list;
}

/* static */
int WControllerMediaData::extractDuration(const QList<WControllerMediaObject> & timeline,
                                          QList<int>                          * starts,
                                          QList<int>                          * durations,
                                          int                                   start)
{
    int duration = 0;

    foreach (const WControllerMediaObject & object, timeline)
    {
        WControllerMediaSource * media = object.media;

        if (media == NULL) continue;

        start += media->at;

        starts->append(start);

        int durationSource = media->getDuration(start);

        durations->append(durationSource);

        if (durationSource > 0)
        {
            start = 0;

            duration += durationSource;
        }
        else start = -durationSource;
    }

    if (duration == 0)
    {
        return -1;
    }
    else return duration;
}

/* static */ QString WControllerMediaData::extractResult(const WYamlReader & reader,
                                                         const QString     & argument)
{
    QStringList list = argument.split(',');

    if (list.isEmpty()) return QString();

    // NOTE: A routine name has a maximum length of 16 characters.
    QString routine = list.takeFirst().left(16).toUpper();

    WBackendUniversalScript script(reader.extractString(routine));

    if (script.isValid() == false) return QString();

    WBackendUniversalParameters parameters(script);

    parameters.add("argument", list.first());
    parameters.add("args",     list);

    return script.run(&parameters).toString();
}

/* static */ QHash<QString, WControllerMediaSource *>
WControllerMediaData::generateHash(QList<WControllerMediaSource> & sources)
{
    QHash<QString, WControllerMediaSource *> hash;

    for (int i = 0; i < sources.count(); i++)
    {
        WControllerMediaSource & source = sources[i];

        hash.insert(source.id, &source);
    }

    return hash;
}

/* static */ QList<WControllerMediaObject>
WControllerMediaData::generateTimeline(const QHash<QString, WControllerMediaSource *> & hash,
                                       const QString                                  & context,
                                       const QStringList                              & tags)
{
    QList<WControllerMediaObject> timeline;

    QStringList list = Sk::split(context.toLower(), ',');

    int lastIndex = -1;

    foreach (const QString & id, list)
    {
        WControllerMediaSource * media = getMediaSource(hash, id);

        if (media == NULL)
        {
            // NOTE: When the tag is unknown we skip it entirely.
            if (tags.contains(id) == false) continue;

            WControllerMediaObject object;

            object.id    = id;
            object.media = NULL;

            timeline.append(object);

            continue;
        }

        int index = media->index;

        if (index == lastIndex) continue;

        if (index < lastIndex)
        {
            qWarning("WControllerMediaData::generateTimeline: Partial timeline.");

            return timeline;
        }

        WControllerMediaObject object;

        object.id    = id;
        object.media = media;

        timeline.append(object);
    }

    return timeline;
}

/* static */
QString WControllerMediaData::generateContext(const QList<WControllerMediaObject> & timeline)
{
    QString context;

    foreach (const WControllerMediaObject & object, timeline)
    {
        context.append(object.id + ',');
    }

    if (context.isEmpty()) return context;

    context.chop(1);

    return WZipper::compressBase64(context.toUtf8());
}

/* static */ WControllerMediaSource *
WControllerMediaData::getMediaSource(const QHash<QString, WControllerMediaSource *> & hash,
                                     const QString                                  & id)
{
    WControllerMediaSource * source = hash.value(id);

    if (source == NULL || source->duration != -2) return source;

    const WYamlNode * node = source->node;

    source->duration = node->extractMsecs("duration", -1);

    source->at = node->extractMsecs("at");

    if (source->duration != -1) return source;

    source->end = node->extractMsecs("end", -1);

    return source;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WControllerMediaData::extractSourceDuration(const QList<WYamlNode> & children,
                                                 const QList<int>       & durations,
                                                 const QList<int>       & starts)
{
    for (int i = 0; i < durations.length(); i++)
    {
        start = starts.at(i);

        int durationSource = durations.at(i);

        if (durationSource <= 0)
        {
            start = -durationSource;

            continue;
        }

        int time = timeA + durationSource;

        if (currentTime >= time)
        {
            timeA = time;

            start = 0;

            continue;
        }

        const WYamlNode & child = children.at(i);

        const WYamlNode * node = child.at("source");

        if (node == NULL) return;

        const QList<WYamlNode> & nodes = node->children;

        if (nodes.isEmpty())
        {
            applySource(child, node->value, durationSource);
        }
        else extractSource(nodes);

        if (source.isEmpty()) applyEmpty();

        return;
    }
}

int WControllerMediaData::extractSourceTimeline(const QList<WControllerMediaObject> & timeline,
                                                const QList<int>                    & durations,
                                                const QList<int>                    & starts)
{
    int index = 0;

    for (int i = 0; i < timeline.count(); i++)
    {
        WControllerMediaSource * media = timeline.at(i).media;

        if (media == NULL) continue;

        start = starts.at(index);

        int durationSource = durations.at(index);

        index++;

        if (durationSource <= 0)
        {
            start = -durationSource;

            continue;
        }

        int time = timeA + durationSource;

        if (currentTime >= time)
        {
            timeA = time;

            start = 0;

            continue;
        }

        const WYamlNode * child = media->node;

        const WYamlNode * node = child->at("source");

        if (node == NULL) break;

        const QList<WYamlNode> & nodes = node->children;

        if (nodes.isEmpty())
        {
            applySource(*child, node->value, durationSource);
        }
        else extractSource(nodes);

        if (source.isEmpty()) applyEmpty();

        return i;
    }

    return -1;
}

void WControllerMediaData::extractSource(const QList<WYamlNode> & children)
{
    foreach (const WYamlNode & child, children)
    {
        start += child.extractMsecs("at");

        int durationSource = WControllerPlaylist::vbmlDuration(child, start);

        if (durationSource <= 0)
        {
            start = -durationSource;

            continue;
        }

        int time = timeA + durationSource;

        if (currentTime >= time)
        {
            timeA = time;

            start = 0;

            continue;
        }

        const WYamlNode * node = child.at("source");

        if (node == NULL) return;

        const QList<WYamlNode> & nodes = node->children;

        if (nodes.isEmpty())
        {
            applySource(child, node->value, durationSource);
        }
        else extractSource(nodes);

        return;
    }
}

void WControllerMediaData::applySource(const WYamlNode & node, const QString & url, int duration)
{
    if (WControllerFile::urlIsImage(url) == false)
    {
        QString type = node.extractString("type");

        if (type.isEmpty())
        {
             typeSource = WTrack::typeFromString(node.key);
        }
        else typeSource = WTrack::typeFromString(type);
    }
    // NOTE: By default, an image is played like a hub.
    else typeSource = WTrack::Hub;

    source = url;

    timeB = timeA + duration;
}

void WControllerMediaData::applyMedia(const WYamlNodeBase & node, const QString & url)
{
    int durationSource = WControllerPlaylist::vbmlDuration(node, 0, -1);

    if (durationSource == -1)
    {
        source = url;

        return;
    }

    int at = node.extractMsecs("at");

    if (at)
    {
        durationSource -= at;

        start += at;
    }

    if (durationSource <= 0) return;

    if (duration == -1)
    {
        if (type == WTrack::Channel)
        {
            duration = CONTROLLERMEDIA_CHANNEL_DURATION;
        }
        else duration = durationSource;
    }

    source = url;

    timeB = timeA + durationSource;
}

void WControllerMediaData::applyEmpty()
{
    if (currentTime >= duration) return;

    timeB = duration;

    start = 0;
}

//=================================================================================================
// WControllerMediaReply
//=================================================================================================

class WControllerMediaReply : public QObject
{
    Q_OBJECT

public: // Interface
    Q_INVOKABLE void extractVbml(QIODevice     * device,
                                 const QString & url,
                                 int             currentTime,
                                 int             duration,
                                 int             timeA,
                                 int             start,
                                 const QString & context,
                                 const QString & argument);

    Q_INVOKABLE void extractM3u(QIODevice * device, const QString & url);

signals:
    void loaded(QIODevice * device, const WControllerMediaData & data);
};

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerMediaReply::extractVbml(QIODevice     * device,
                                                          const QString & url,
                                                          int             currentTime,
                                                          int             duration,
                                                          int             timeA,
                                                          int             start,
                                                          const QString & context,
                                                          const QString & argument)
{
    WControllerMediaData data;

    data.currentTime = currentTime;
    data.duration    = duration;

    data.timeA = timeA;

    data.start = start;

    data.context = context;

    data.applyVbml(WControllerFile::readAll(device), url, argument);

    emit loaded(device, data);

    deleteLater();
}

/* Q_INVOKABLE */ void WControllerMediaReply::extractM3u(QIODevice * device, const QString & url)
{
    WControllerMediaData data;

    data.applyM3u(WControllerFile::readAll(device), url);

    emit loaded(device, data);

    deleteLater();
}

//=================================================================================================
// WControllerMediaPrivate
//=================================================================================================

WControllerMediaPrivate::WControllerMediaPrivate(WControllerMedia * p) : WControllerPrivate(p) {}

/* virtual */ WControllerMediaPrivate::~WControllerMediaPrivate()
{
#ifndef SK_NO_PLAYER
    engine->deleteInstance();
#endif

    QHashIterator<WRemoteData *, WPrivateMediaData *> i(jobs);

    while (i.hasNext())
    {
        i.next();

        delete i.value();
    }

    QHashIterator<QIODevice *, WPrivateMediaData *> j(queries);

    while (j.hasNext())
    {
        j.next();

        delete j.value();
    }

    jobs   .clear();
    queries.clear();

    // FIXME Qt4: Not sure why we needed that before but that causes a crash.
    //sk->processEvents();

    thread->quit();
    thread->wait();

#ifndef SK_NO_PLAYER
    delete engine;
#endif

    W_CLEAR_CONTROLLER(WControllerMedia);
}

//-------------------------------------------------------------------------------------------------

#ifdef SK_NO_PLAYER
void WControllerMediaPrivate::init(const QStringList &)
#else
void WControllerMediaPrivate::init(const QStringList & options)
#endif
{
    Q_Q(WControllerMedia);

    loader = NULL;

    qRegisterMetaType<WControllerMediaData>("WControllerMediaData");

    const QMetaObject * meta = WControllerMediaReply().metaObject();

    methodVbml = meta->method(meta->indexOfMethod("extractVbml(QIODevice*,QString,int,int,int,int,"
                                                              "QString,QString)"));

    methodM3u = meta->method(meta->indexOfMethod("extractM3u(QIODevice*,QString)"));

    thread = new QThread(q);

    thread->start();

#ifndef SK_NO_PLAYER
    engine = new WVlcEngine(options, thread);
#endif
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WControllerMediaPrivate::loadSources(WMediaReply * reply)
{
    const QString & url = reply->_url;

    int currentTime = reply->_currentTime;

    QHashIterator<WRemoteData *, WPrivateMediaData *> i(jobs);

    while (i.hasNext())
    {
        i.next();

        WPrivateMediaData * media = i.value();

        if (media->url == url && media->currentTime == currentTime)
        {
            media->replies.append(reply);

            return;
        }
    }

    QHashIterator<QIODevice *, WPrivateMediaData *> j(queries);

    while (j.hasNext())
    {
        j.next();

        WPrivateMediaData * media = j.value();

        if (media->url == url && media->currentTime == currentTime)
        {
            media->replies.append(reply);

            return;
        }
    }

    QString source = WControllerMedia::generateSource(url);

    WBackendNetQuery query;

    // NOTE: The VBML uri is prioritized because it might contain an http url.
    if (WControllerPlaylist::urlIsVbmlUri(source))
    {
        if (source.startsWith("vbml://") == false)
        {
            query.type = WBackendNetQuery::TypeVbml;

            query.url = source;
        }
        else query.url = source.replace("vbml://", "https://");
    }

    WBackendNet * backend = wControllerPlaylist->backendFromUrl(source);

    if (backend)
    {
        QString backendId = backend->id();

        query = backend->getQuerySource(source);

        backend->tryDelete();

        if (resolve(backendId, query) == false)
        {
            reply->_medias.insert(WAbstractBackend::QualityDefault, url);

            reply->_loaded = true;

            return;
        }
    }
    else
    {
        QString extension = WControllerNetwork::extractUrlExtension(source);

        if (WControllerPlaylist::extensionIsM3u(extension))
        {
            query.target = WBackendNetQuery::TargetM3u;

            query.url = source;
        }
        else if (WControllerPlaylist::extensionIsVbml(extension))
        {
            query.url = source;
        }
        // NOTE: An http source could be VBML so we try to load it anyway.
        else if (WControllerNetwork::urlIsHttp(source)
                 &&
                 // NOTE: An IP can be a HookTorrent server.
                 WControllerNetwork::urlIsIp(source) == false)
        {
            // NOTE: We want to avoid large binary files.
            query.scope = WAbstractLoader::ScopeText;

            query.url = source;
        }
        else
        {
            reply->_medias.insert(WAbstractBackend::QualityDefault, url);

            reply->_loaded = true;

            return;
        }
    }

    query.mode = reply->_mode;

    WPrivateMediaData * media = new WPrivateMediaData;

    media->url       = url;
    media->urlSource = url;

    media->type       = WTrack::Unknown;
    media->typeSource = WTrack::Track;

    media->vbml = false;

    media->currentTime = currentTime;
    media->duration    = -1;

    media->timeA =  0;
    media->timeB = -1;

    media->start = 0;

    media->context  = WControllerNetwork::extractFragmentValue(url, "ctx");
    media->argument = WControllerNetwork::extractFragmentValue(url, "arg");

    media->backend = NULL;
    media->query   = query;
    media->reply   = NULL;

    media->replies.append(reply);

    medias.append(media);

    getData(media, &query);
}

void WControllerMediaPrivate::loadUrl(QIODevice              * device,
                                      const WBackendNetQuery & query,
                                      int                      currentTime,
                                      int                      duration,
                                      int                      timeA,
                                      int                      start,
                                      const QString          & context,
                                      const QString          & argument) const
{
    Q_Q(const WControllerMedia);

    WControllerMediaReply * reply = new WControllerMediaReply;

    QObject::connect(reply, SIGNAL(loaded(QIODevice *, const WControllerMediaData &)),
                     q,     SLOT  (onUrl (QIODevice *, const WControllerMediaData &)));

    reply->moveToThread(thread);

    if (query.target == WBackendNetQuery::TargetM3u)
    {
        methodM3u.invoke(reply, Q_ARG(QIODevice *, device), Q_ARG(const QString &, query.url));
    }
    else methodVbml.invoke(reply, Q_ARG(QIODevice     *, device),
                                  Q_ARG(const QString &, query.url),
                                  Q_ARG(int,             currentTime),
                                  Q_ARG(int,             duration),
                                  Q_ARG(int,             timeA),
                                  Q_ARG(int,             start),
                                  Q_ARG(const QString &, context),
                                  Q_ARG(const QString &, argument));
}

//-------------------------------------------------------------------------------------------------

void WControllerMediaPrivate::applyData(WPrivateMediaData          * media,
                                        const WControllerMediaData & data)
{
    WTrack::Type type = data.type;

    if (media->type == WTrack::Unknown)
    {
        media->type = type;
    }

    WTrack::Type typeSource = data.typeSource;

    if (typeSource)
    {
        media->typeSource = typeSource;
    }
    else if (type)
    {
        media->typeSource = type;
    }

    int timeB = data.timeB;

    if (timeB == -1) return;

    int duration = data.duration;

    if (duration != -1)
    {
        if (media->timeZone.isEmpty())
        {
            media->timeZone = data.timeZone;
        }

        media->currentTime = data.currentTime;

        media->duration = duration;

        media->context = data.context;
    }

    media->timeA = data.timeA;

    if (media->timeB == -1 || timeB < media->timeB)
    {
        media->timeB = timeB;
    }

    media->start = data.start;
}

void WControllerMediaPrivate::applySource(WPrivateMediaData            * media,
                                          const WBackendNetSource      & source,
                                          WAbstractBackend::SourceMode   mode)
{
    const QHash<WAbstractBackend::Quality, QString> & medias = source.medias;

    int timeB = media->timeB;

    if (timeB == -1)
    {
        if (medias.count() == 0)
        {
            foreach (WMediaReply * reply, media->replies)
            {
                emit reply->loaded(reply);
            }

            return;
        }

        const QHash<WAbstractBackend::Quality, QString> & audios = source.audios;

        WPrivateMediaSlice slice;

        QString urlSource = media->urlSource;

        WTrack::Type type       = media->type;
        WTrack::Type typeSource = media->typeSource;

        bool vbml = media->vbml;

        slice.urlSource = urlSource;

        slice.type       = type;
        slice.typeSource = typeSource;

        slice.vbml = vbml;

        slice.medias = medias;
        slice.audios = audios;

        slice.expiry = source.expiry;

        slice.duration = -1;

        slice.timeA = -1;
        slice.timeB = -1;

        slice.start = 0;

        appendSlice(slice, media->url, mode);

        foreach (WMediaReply * reply, media->replies)
        {
            reply->_loaded = true;

            reply->_urlSource = urlSource;

            reply->_type       = type;
            reply->_typeSource = typeSource;

            reply->_vbml = vbml;

            reply->_medias = medias;
            reply->_audios = audios;

            emit reply->loaded(reply);
        }
    }
    else
    {
        const QHash<WAbstractBackend::Quality, QString> & audios = source.audios;

        WPrivateMediaSlice slice;

        QString urlSource = media->urlSource;

        WTrack::Type type       = media->type;
        WTrack::Type typeSource = media->typeSource;

        bool vbml = media->vbml;

        QString timeZone = media->timeZone;

        int duration = media->duration;

        int timeA = media->timeA;
        int timeB = media->timeB;

        int start = media->start;

        QString context = media->context;

        slice.urlSource = urlSource;

        slice.type       = type;
        slice.typeSource = typeSource;

        slice.vbml = vbml;

        slice.timeZone = timeZone;

        slice.medias = medias;
        slice.audios = audios;

        slice.expiry = source.expiry;

        slice.duration = duration;

        slice.timeA = timeA;
        slice.timeB = timeB;

        slice.start = start;

        slice.context = context;

        appendSlice(slice, media->url, mode);

        foreach (WMediaReply * reply, media->replies)
        {
            reply->_loaded = true;

            reply->_urlSource = urlSource;

            reply->_type       = type;
            reply->_typeSource = typeSource;

            reply->_vbml = vbml;

            reply->_timeZone = timeZone;

            reply->_duration = duration;

            reply->_timeA = timeA;
            reply->_timeB = timeB;

            reply->_start = start;

            reply->_context = context;

            reply->_medias = medias;
            reply->_audios = audios;

            emit reply->loaded(reply);
        }
    }
}

void WControllerMediaPrivate::appendSlice(const WPrivateMediaSlice     & slice,
                                          const QString                & url,
                                          WAbstractBackend::SourceMode   mode)
{
    WPrivateMediaSource * mediaSource = getSource(url);

    if (mediaSource)
    {
        WPrivateMediaMode * data = getMode(mediaSource, mode);

        if (data)
        {
#ifdef QT_OLD
            QList<WPrivateMediaSlice> & slices = data->slices;
#else
            WList<WPrivateMediaSlice> & slices = data->slices;
#endif

            while (slices.count() == CONTROLLERMEDIA_MAX_SLICES)
            {
                slices.removeAt(0);
            }

            slices.append(slice);
        }
        else
        {
            WPrivateMediaMode data;

            data.slices.append(slice);

            mediaSource->modes.insert(mode, data);
        }
    }
    else
    {
        while (urls.count() == CONTROLLERMEDIA_MAX_CACHE)
        {
            sources.remove(urls.takeFirst());
        }

        WPrivateMediaSource mediaSource;

        WPrivateMediaMode data;

        data.slices.append(slice);

        mediaSource.modes.insert(mode, data);

        urls.append(url);

        sources.insert(url, mediaSource);
    }
}

void WControllerMediaPrivate::updateSources()
{
    QDateTime date = QDateTime::currentDateTime();

    QMutableHashIterator<QString, WPrivateMediaSource> i(sources);

    while (i.hasNext())
    {
        i.next();

        QHash<WAbstractBackend::SourceMode, WPrivateMediaMode> & modes = i.value().modes;

        QMutableHashIterator<WAbstractBackend::SourceMode, WPrivateMediaMode> j(modes);

        while (j.hasNext())
        {
            j.next();

#ifdef QT_OLD
            QList<WPrivateMediaSlice> & slices = j.value().slices;
#else
            WList<WPrivateMediaSlice> & slices = j.value().slices;
#endif

            int index = 0;

            while (index < slices.count())
            {
                const WPrivateMediaSlice & slice = slices.at(index);

                QDateTime expiry = slice.expiry;

                if (expiry.isValid() && expiry < date)
                {
                    qDebug("MEDIA EXPIRED");

                    slices.removeAt(index);

                    continue;
                }

                index++;
            }

            if (slices.isEmpty()) j.remove();
        }

        if (modes.isEmpty())
        {
            urls.removeOne(i.key());

            i.remove();
        }
    }
}

//-------------------------------------------------------------------------------------------------

void WControllerMediaPrivate::clearReply(WMediaReply * reply)
{
    foreach (WPrivateMediaData * media, medias)
    {
        QList<WMediaReply *> * replies = &(media->replies);

        if (replies->contains(reply) == false) continue;

        replies->removeOne(reply);

        if (replies->isEmpty())
        {
            WRemoteData * data = jobs.key(media);

            if (data) jobs.remove(data);

            WBackendNet * backend = media->backend;

            if (backend) backend->tryDelete();

            QIODevice * networkReply = media->reply;

            if (networkReply)
            {
                queries.remove(networkReply);
            }

            medias.removeOne(media);

            delete media;
            delete data;
        }

        return;
    }
}

//-------------------------------------------------------------------------------------------------

int WControllerMediaPrivate::checkMax(WPrivateMediaData * media, WBackendNetQuery & query)
{
    int indexNext = query.indexNext;

    if (indexNext < CONTROLLERMEDIA_MAX_QUERY)
    {
        return indexNext + 1;
    }

    qWarning("WControllerMediaPrivate::checkMax: Maximum queries reached.");

    foreach (WMediaReply * reply, media->replies)
    {
        emit reply->loaded(reply);
    }

    medias.removeOne(media);

    delete media;

    return -1;
}

bool WControllerMediaPrivate::resolve(const QString & backendId, WBackendNetQuery & query)
{
    QString id = query.backend;

    if (id.isEmpty() || id == backendId) return query.isValid();

    WBackendNet * backend = wControllerPlaylist->backendFromId(id);

    if (backend == NULL) return query.isValid();

    query = backend->getQuerySource(query.url);

    backend->tryDelete();

    return query.isValid();
}

void WControllerMediaPrivate::getData(WPrivateMediaData * media, WBackendNetQuery * query)
{
    Q_Q(WControllerMedia);

    // NOTE: Media sources should be high priority by default. But maybe we should let the user
    //       configure this.
    query->priority = static_cast<QNetworkRequest::Priority> (QNetworkRequest::HighPriority);

    WRemoteData * data = wControllerPlaylist->getData(loader, *query, q);

    QObject::connect(data, SIGNAL(loaded(WRemoteData *)), q, SLOT(onLoaded(WRemoteData *)));

    jobs.insert(data, media);
}

//-------------------------------------------------------------------------------------------------

WPrivateMediaSource * WControllerMediaPrivate::getSource(const QString & url)
{
    QHash<QString, WPrivateMediaSource>::iterator i = sources.find(url);

    if (i == sources.end())
    {
        return NULL;
    }
    else return &(i.value());
}

WPrivateMediaMode * WControllerMediaPrivate::getMode(WPrivateMediaSource * source,
                                                     WAbstractBackend::SourceMode mode)
{
    QHash<WAbstractBackend::SourceMode, WPrivateMediaMode> & modes = source->modes;

    QHash<WAbstractBackend::SourceMode, WPrivateMediaMode>::iterator i = modes.find(mode);

    if (i == modes.end())
    {
        return NULL;
    }
    else return &(i.value());
}

const WPrivateMediaSlice * WControllerMediaPrivate::getSlice(WPrivateMediaSource * source,
                                                             WAbstractBackend::SourceMode mode,
                                                             int currentTime)
{
    WPrivateMediaMode * modes = getMode(source, mode);

    if (modes == NULL) return NULL;

#ifdef QT_OLD
    QList<WPrivateMediaSlice> & slices = modes->slices;
#else
    WList<WPrivateMediaSlice> & slices = modes->slices;
#endif

    int count = slices.count();

    for (int i = 0; i < count; i++)
    {
        const WPrivateMediaSlice & slice = slices.at(i);

        int timeA = slice.timeA;

        // NOTE: When the time is -1 it means the currentTime is irrelevant so we return the slice
        //       right away.
        if (timeA != -1
            &&
            (currentTime < timeA || currentTime > slice.timeB)) continue;

        count--;

        // NOTE: We pop the slice at the top of the stack.
        slices.move(i, count);

        return &(slices.last());
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WControllerMediaPrivate::onLoaded(WRemoteData * data)
{
    WPrivateMediaData * media = jobs.take(data);

    WBackendNetQuery * backendQuery = &(media->query);

    WBackendNet * backend;

    QString id = backendQuery->backend;

    if (id.isEmpty())
    {
         backend = wControllerPlaylist->backendFromUrl(backendQuery->url);
    }
    else if (id == "vbml")
    {
        backend = NULL;
    }
    else backend = wControllerPlaylist->backendFromId(id);

    if (data->hasError() && backendQuery->skipError == false)
    {
        qWarning("WControllerMediaPrivate::onLoaded: Failed to load media %s.", data->url().C_STR);

        if (backend)
        {
            backend->queryFailed(*backendQuery);

            backend->tryDelete();
        }

        QString error = data->error();

        foreach (WMediaReply * reply, media->replies)
        {
            reply->_error = error;

            emit reply->loaded(reply);
        }

        medias.removeOne(media);

        delete media;
    }
    else
    {
        Q_Q(WControllerMedia);

        QIODevice * reply = data->takeReply(NULL);

        media->reply = reply;

        queries.insert(reply, media);

        if (backend)
        {
            // NOTE: We apply the backend to the query so we can delete it later.
            media->backend = backend;

            backend->loadSource(reply, *backendQuery,
                                q, SLOT(onSourceLoaded(QIODevice *, WBackendNetSource)));
        }
        else loadUrl(reply, *backendQuery, media->currentTime, media->duration, media->timeA,
                     media->start, media->context, media->argument);
    }

    delete data;
}

void WControllerMediaPrivate::onSourceLoaded(QIODevice * device, const WBackendNetSource & source)
{
    WPrivateMediaData * media = queries.take(device);

    device->deleteLater();

    if (media == NULL) return;

    media->reply = NULL;

    const WBackendNetQuery & backendQuery = media->query;

    if (source.reload)
    {
        int indexReload = backendQuery.indexReload;

        if (indexReload < CONTROLLERMEDIA_MAX_RELOAD)
        {
            WBackendNetQuery nextQuery = backendQuery;

            nextQuery.indexReload = indexReload + 1;

            // NOTE: We propagate the compatibility mode.
            nextQuery.mode = backendQuery.mode;

            media->query = nextQuery;

            getData(media, &nextQuery);

            return;
        }
    }

    WBackendNet * backend = media->backend;

    QString backendId;

    if (backend)
    {
        backend->applySource(backendQuery, source);

        backendId = backend->id();

        media->backend = NULL;

        backend->tryDelete();
    }

    if (source.valid)
    {
        const QByteArray & cache = source.cache;

        if (cache.isEmpty() == false)
        {
            wControllerFile->addCache(media->url, cache);
        }
    }

    const QList<WBackendNetQuery> & queries = source.nextQueries;

    if (queries.isEmpty() == false)
    {
        WBackendNetQuery nextQuery = queries.first();

        int indexNext = backendQuery.indexNext;

        if (indexNext < CONTROLLERMEDIA_MAX_QUERY)
        {
            if (resolve(backendId, nextQuery))
            {
                nextQuery.indexNext = indexNext + 1;

                // NOTE: We propagate the compatibility mode.
                nextQuery.mode = backendQuery.mode;

                media->query = nextQuery;

                getData(media, &nextQuery);

                return;
            }
        }
        else qWarning("WControllerMediaPrivate::onSourceLoaded: Maximum queries reached.");
    }

    if (media->type == WTrack::Unknown)
    {
        media->type = WTrack::Track;
    }

    applySource(media, source, backendQuery.mode);

    this->medias.removeOne(media);

    delete media;
}

void WControllerMediaPrivate::onUrl(QIODevice * device, const WControllerMediaData & data)
{
    WPrivateMediaData * media = queries.take(device);

    device->deleteLater();

    if (media == NULL) return;

    QString origin = data.origin;

    WBackendNetQuery & query = media->query;

    WAbstractBackend::SourceMode mode = query.mode;

    if (data.vbml)
    {
        media->vbml = true;
    }

    if (origin.isEmpty() == false)
    {
        int indexNext = checkMax(media, query);

        if (indexNext == -1) return;

        WBackendNet * backend = wControllerPlaylist->backendFromUrl(origin);

        if (backend)
        {
            QString backendId = backend->id();

            query = backend->getQuerySource(origin);

            backend->tryDelete();

            if (resolve(backendId, query))
            {
                query.indexNext = indexNext;

                // NOTE: We propagate the compatibility mode.
                query.mode = mode;

                getData(media, &query);

                return;
            }
        }
        else
        {
            query = WBackendNetQuery(origin);

            query.indexNext = indexNext;

            if (WControllerPlaylist::urlIsVbmlUri(origin))
            {
                query.type = WBackendNetQuery::TypeVbml;

                query.url = origin;
            }

            // NOTE: We propagate the compatibility mode.
            query.mode = mode;

            getData(media, &query);

            return;
        }
    }

    WBackendNetSource backendSource;

    QString source = data.source;

    if (source.isEmpty() == false)
    {
        WBackendNet * backend = wControllerPlaylist->backendFromUrl(source);

        if (backend)
        {
            int indexNext = checkMax(media, query);

            if (indexNext == -1) return;

            QString backendId = backend->id();

            query = backend->getQuerySource(source);

            backend->tryDelete();

            if (resolve(backendId, query))
            {
                query.indexNext = indexNext;

                // NOTE: We propagate the compatibility mode.
                query.mode = mode;

                media->urlSource = source;

                applyData(media, data);

                getData(media, &query);

                return;
            }
        }
        else if (WControllerPlaylist::urlIsVbml(source))
        {
            int indexNext = checkMax(media, query);

            if (indexNext == -1) return;

            query = WBackendNetQuery(source);

            query.indexNext = indexNext;

            if (WControllerPlaylist::urlIsVbmlUri(source))
            {
                query.type = WBackendNetQuery::TypeVbml;

                query.url = source;
            }

            // NOTE: We propagate the compatibility mode.
            query.mode = mode;

            media->urlSource = source;

            applyData(media, data);

            getData(media, &query);

            return;
        }
        else if (WControllerNetwork::urlIsHttp(source))
        {
            int indexNext = checkMax(media, query);

            if (indexNext == -1) return;

            query = WBackendNetQuery(source);

            query.indexNext = indexNext;

            // NOTE: We propagate the compatibility mode.
            query.mode = mode;

            // NOTE: We want to avoid large binary files.
            query.scope = WAbstractLoader::ScopeText;

            media->urlSource = source;

            applyData(media, data);

            getData(media, &query);

            return;
        }

        // NOTE: If the parsing fails we add the current url as the default source.
        backendSource.medias.insert(WAbstractBackend::QualityDefault, source);
    }
    else backendSource.medias = data.medias;

    applyData(media, data);

    applySource(media, backendSource, mode);

    medias.removeOne(media);

    delete media;
}

//=================================================================================================
// WControllerMedia
//=================================================================================================
// Private

WControllerMedia::WControllerMedia() : WController(new WControllerMediaPrivate(this)) {}

//-------------------------------------------------------------------------------------------------
// Initialize
//-------------------------------------------------------------------------------------------------

/* virtual */ void WControllerMedia::initController(const QStringList & options)
{
    Q_D(WControllerMedia);

    if (d->created == false)
    {
        d->created = true;

        d->init(options);
    }
    else qWarning("WControllerMedia::initController: Controller is already initialized.");
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

#ifndef SK_NO_PLAYER

/* Q_INVOKABLE */ WVlcPlayer * WControllerMedia::createVlcPlayer() const
{
    Q_D(const WControllerMedia);

    return new WVlcPlayer(d->engine, d->thread);
}

#endif

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */
WMediaReply * WControllerMedia::getMedia(const QString              & url,
                                         QObject                    * parent,
                                         WAbstractBackend::SourceMode mode,
                                         int                          currentTime)
{
    if (url.isEmpty()) return NULL;

    Q_D(WControllerMedia);

    WMediaReply * reply;

    if (parent) reply = new WMediaReply(parent);
    else        reply = new WMediaReply(this);

    reply->_url = url;

    reply->_mode = mode;

    reply->_type       = WTrack::Track;
    reply->_typeSource = WTrack::Track;

    reply->_vbml = false;

    reply->_currentTime = currentTime;
    reply->_duration    = -1;

    reply->_timeA = -1;
    reply->_timeB = -1;

    reply->_start = 0;

    reply->_backend = NULL;

    reply->_loaded = false;

    d->updateSources();

    WPrivateMediaSource * source = d->getSource(url);

    if (source)
    {
        const WPrivateMediaSlice * slice = d->getSlice(source, mode, currentTime);

        if (slice)
        {
            qDebug("MEDIA CACHED");

            d->urls.removeOne(url);
            d->urls.append   (url);

            reply->_urlSource = slice->urlSource;

            reply->_type       = slice->type;
            reply->_typeSource = slice->typeSource;

            reply->_vbml = slice->vbml;

            reply->_timeZone = slice->timeZone;

            reply->_duration = slice->duration;

            reply->_timeA = slice->timeA;
            reply->_timeB = slice->timeB;

            reply->_start = slice->start;

            reply->_context = slice->context;

            reply->_medias = slice->medias;
            reply->_audios = slice->audios;

            reply->_loaded = true;

            return reply;
        }
    }

    d->loadSources(reply);

    return reply;
}

/* Q_INVOKABLE */
WMediaReply * WControllerMedia::getMedia(const QString              & url,
                                         WAbstractBackend::SourceMode mode,
                                         int                          currentTime)
{
    return getMedia(url, NULL, mode, currentTime);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerMedia::clearMedia(const QString & url)
{
    Q_D(WControllerMedia);

    QList<WMediaReply *> replies;

    QHashIterator<WRemoteData *, WPrivateMediaData *> i(d->jobs);

    while (i.hasNext())
    {
        i.next();

        foreach (WMediaReply * reply, i.value()->replies)
        {
            if (reply->_url == url)
            {
                replies.append(reply);
            }
        }
    }

    QHashIterator<QIODevice *, WPrivateMediaData *> j(d->queries);

    while (j.hasNext())
    {
        j.next();

        foreach (WMediaReply * reply, j.value()->replies)
        {
            if (reply->_url == url)
            {
                replies.append(reply);
            }
        }
    }

    foreach (WMediaReply * reply, replies)
    {
        delete reply;
    }

    d->sources.remove(url);
}

/* Q_INVOKABLE */ void WControllerMedia::clearMedias()
{
    Q_D(WControllerMedia);

    QList<WMediaReply *> replies;

    QHashIterator<WRemoteData *, WPrivateMediaData *> i(d->jobs);

    while (i.hasNext())
    {
        i.next();

        foreach (WMediaReply * reply, i.value()->replies)
        {
            replies.append(reply);
        }
    }

    QHashIterator<QIODevice *, WPrivateMediaData *> j(d->queries);

    while (j.hasNext())
    {
        j.next();

        foreach (WMediaReply * reply, j.value()->replies)
        {
            replies.append(reply);
        }
    }

    foreach (WMediaReply * reply, replies)
    {
        delete reply;
    }

    d->sources.clear();
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerMedia::generateSource(const QString & url)
{
#ifdef Q_OS_WIN
    if (url.startsWith('/') || url.startsWith('\\') || (url.length() > 1 && url.at(1) == ':'))
#else
    if (url.startsWith('/') || (url.length() > 1 && url.at(1) == ':'))
#endif
    {
        return url;
    }
    else if (WControllerNetwork::urlIsHttp(url) == false)
    {
        if (QUrl(url).scheme().isEmpty())
        {
            return "https://" + url;
        }
        else return url;
    }

    QString source = WControllerNetwork::removeUrlPrefix(url);

    if (source.startsWith("vbml.", Qt::CaseInsensitive))
    {
        return WControllerPlaylist::vbmlUriFromUrl(source);
    }
    else return url;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

#ifndef SK_NO_PLAYER

WVlcEngine * WControllerMedia::engine() const
{
    Q_D(const WControllerMedia); return d->engine;
}

#endif

//-------------------------------------------------------------------------------------------------

WAbstractLoader * WControllerMedia::loader() const
{
    Q_D(const WControllerMedia); return d->loader;
}

void WControllerMedia::setLoader(WAbstractLoader * loader)
{
    Q_D(WControllerMedia);

    if (d->loader == loader) return;

    d->loader = loader;

    emit loaderChanged();
}

#endif // SK_NO_CONTROLLERMEDIA

#include "WControllerMedia.moc"
