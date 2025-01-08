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

    QString tab  = Sk::tabs(1);
    QString tab2 = Sk::tabs(2);

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
        Sk::bmlTag(vbml, "audios");

        QHashIterator<WAbstractBackend::Quality, QString> i(_audios);

        while (i.hasNext())
        {
            i.next();

            Sk::bmlPair(vbml, tab + WAbstractBackend::qualityToString(i.key()), i.value(), "\n\n");
        }
    }

    if (_chapters.isEmpty() == false)
    {
        Sk::bmlTag(vbml, "chapters");

        foreach (const WChapter & chapter, _chapters)
        {
            Sk::bmlTag(vbml, tab + "chapter");

            Sk::bmlPair(vbml, tab2 + "time", QString::number(chapter.time()));

            Sk::bmlPair(vbml, tab2 + "title", chapter.title());
            Sk::bmlPair(vbml, tab2 + "cover", chapter.cover());
        }

        vbml.append('\n');
    }

    if (_ambient.isEmpty() == false)
    {
        Sk::bmlPair(vbml, "ambient", _ambient, "\n\n");
    }

    if (_subtitles.isEmpty() == false)
    {
        Sk::bmlList(vbml, "subtitles");

        foreach (const WSubtitle & subtitle, _subtitles)
        {
            Sk::bmlTag(vbml, tab + "subtitle");

            Sk::bmlPair(vbml, tab2 + "source", subtitle.source());
            Sk::bmlPair(vbml, tab2 + "title",  subtitle.title ());
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

void WMediaReply::setType(WTrack::Type type)
{
    _type = type;
}

WTrack::Type WMediaReply::typeSource() const
{
    return _typeSource;
}

void WMediaReply::setTypeSource(WTrack::Type type)
{
    _typeSource = type;
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

QString WMediaReply::contextId() const
{
    return _contextId;
}

QHash<WAbstractBackend::Quality, QString> WMediaReply::medias() const
{
    return _medias;
}

QHash<WAbstractBackend::Quality, QString> WMediaReply::audios() const
{
    return _audios;
}

QList<WChapter> WMediaReply::chapters() const
{
    return _chapters;
}

QString WMediaReply::ambient() const
{
    return _ambient;
}

QList<WSubtitle> WMediaReply::subtitles() const
{
    return _subtitles;
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
                                                               const QString & urlBase)
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

            applyVbml(content.toUtf8(), url, urlBase);

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

    QString baseUrl = WControllerNetwork::extractBaseUrl(url);

    type = WTrack::typeFromString(reader.extractString("type"));

    if (type > WTrack::Track) typeSource = type;

    const QList<WYamlNode> & children = node->children;

    if (children.isEmpty())
    {
        source = node->value;

        applyMedia(reader.node(), source, baseUrl);

        return;
    }

    const WYamlNodeBase & root = reader.node();

    if (duration == -1)
    {
        // NOTE: The Interactive type is only taken into account for the root source.
        if (type == WTrack::Interactive)
        {
            // NOTE: The interactive mode ignores the root start property.
            start = 0;

            QStringList tags = WControllerPlaylist::vbmlTags(root);

            QList<WControllerMediaSource> sources = WControllerMediaSource::extractSources(root);

            QHash<QString, WControllerMediaSource *> hash
                = WControllerMediaSource::generateHash(sources);

            QList<WControllerMediaObject> timeline;

            QStringList list;

            context = WControllerNetwork::extractFragmentValue(urlBase, "ctx");

            if (context.isEmpty())
            {
                QString contextBase = reader.extractString("context");

                list = WControllerMediaSource::getContextList(contextBase);

                timeline = WControllerMediaSource::generateTimeline(hash, list, tags);
            }
            else
            {
                contextId = WControllerNetwork::extractFragmentValue(urlBase, "id");

                QByteArray array = WUnzipper::extractBase64(context.toUtf8());

                list = WControllerMediaSource::getContextList(array);

                timeline = WControllerMediaSource::generateTimeline(hash, list, tags);

                if (timeline.isEmpty())
                {
                    QString contextBase = reader.extractString("context");

                    list = WControllerMediaSource::getContextList(contextBase);

                    timeline = WControllerMediaSource::generateTimeline(hash, list, tags);
                }
            }

            if (timeline.isEmpty()) return;

            //qDebug("CONTEXT BEFORE %s %s", getContext(timeline).C_STR, contextId.C_STR);

            duration = WControllerMediaSource::applyDurations(&timeline);

            if (duration == -1)
            {
                context = cleanTimeline(timeline, -1, baseUrl);

                return;
            }

            int index;

            if (contextId.isEmpty())
            {
                index = extractSourceTimeline(root, timeline, baseUrl);

                // NOTE: We found no source, so we clear the currentTime and try again.
                if (index == -1)
                {
                    currentTime = 0;

                    timeA = 0;

                    index = extractSourceTimeline(root, timeline, baseUrl);

                    context = cleanTimeline(timeline, index, baseUrl);

                    return;
                }
            }
            else
            {
                index = extractSourceTimeline(root, timeline, baseUrl);

                // NOTE: We found no source, so we try to update the currentTime based on the
                //       contextId.
                if (index == -1)
                {
                    index = updateCurrentTime(root, timeline, baseUrl);

                    // NOTE: We found no source, so we clear the contextId and try again.
                    if (index == -1)
                    {
                        timeA = 0;

                        contextId = QString();

                        index = extractSourceTimeline(root, timeline, baseUrl);

                        // NOTE: We found no source, so we clear the currentTime and try again.
                        if (index == -1)
                        {
                            currentTime = 0;

                            timeA = 0;

                            index = extractSourceTimeline(root, timeline, baseUrl);
                        }

                        context = cleanTimeline(timeline, index, baseUrl);

                        return;
                    }
                }
            }

            QString argument = WControllerNetwork::extractFragmentValue(urlBase, "arg");

            if (argument.isEmpty())
            {
                context = cleanTimeline(timeline, index, baseUrl);

                return;
            }

            // NOTE: We want to provide the context up until now.
            QVariant result = extractResult(reader, argument, getContext(timeline, index + 1),
                                            contextId);

            QList<WControllerMediaObject> timelineNew;

            if (result.typeId() == QMetaType::QVariantList)
            {
                QVariantList variants = result.toList();

                int count = variants.count();

                if (count < 2)
                {
                    context = cleanTimeline(timeline, index, baseUrl);

                    return;
                }

                QString name = variants.first().toString().toLower();

                if (name != "swap")
                {
                    context = cleanTimeline(timeline, index, baseUrl);

                    return;
                }

                // NOTE: We clear the chapters when swapping.
                chapters.clear();

                list = Sk::split(variants.at(1).toString().toLower(), ',');

                timelineNew = WControllerMediaSource::generateTimeline(hash, list, tags);

                int durationNew = WControllerMediaSource::applyDurations(&timelineNew);

                if (durationNew == -1)
                {
                    context = cleanTimeline(timeline, index, baseUrl);

                    return;
                }

                duration = timeA + durationNew;

                contextId = QString();
            }
            else
            {
                list = Sk::split(result.toString().toLower(), ',');

                timelineNew = WControllerMediaSource::generateTimeline(hash, list, tags);

                int durationNew = WControllerMediaSource::applyDurations(&timelineNew);

                if (durationNew == -1)
                {
                    context = cleanTimeline(timeline, index, baseUrl);

                    return;
                }

                timeA = timeB;

                currentTime = timeA;
                duration    = timeA + durationNew;

                contextId = QString();

                index++;
            }

            int indexNew = extractSourceTimeline(root, timelineNew, baseUrl);

            if (indexNew == -1)
            {
                context = cleanTimeline(timeline, index, baseUrl);

                return;
            }

            if (index < timeline.count())
            {
                timeline.erase(timeline.begin() + index, timeline.end());
            }

            timeline.append(timelineNew);

            context = cleanTimeline(timeline, index + indexNew, baseUrl);

            return;
        }

        start += reader.extractMsecs("at");

        // NOTE: The Channel type is only taken into account for the root source.
        if (type == WTrack::Channel)
        {
            timeZone = reader.extractString("timezone");

            QDateTime date = WControllerApplication::currentDateUtc(timeZone);

            currentTime = WControllerApplication::getMsecsWeek(date);

            duration = CONTROLLERMEDIA_CHANNEL_DURATION;
        }
        else
        {
            duration = WControllerPlaylist::vbmlDuration(root, start);

            if (duration)
            {
                extractSource(root, WControllerPlaylist::vbmlSeed(root, *node), baseUrl);

                if (source.isEmpty()) applyEmpty();

                return;
            }

            QList<int> starts;
            QList<int> durations;

            duration = 0;

            int startSource = start;

            QList<WYamlNode> children = WControllerPlaylist::vbmlSeed(root, *node);

            foreach (const WYamlNode & child, children)
            {
                duration += extractDuration(child, &starts, &durations, startSource);
            }

            if (duration == 0)
            {
                duration = -1;

                return;
            }

            extractSourceDuration(root, children, durations, starts, baseUrl);

            if (source.isEmpty()) applyEmpty();

            return;
        }
    }
    else start += reader.extractMsecs("at");

    extractSource(root, WControllerPlaylist::vbmlSeed(root, *node), baseUrl);

    if (source.isEmpty()) applyEmpty();
}

void WControllerMediaData::applyM3u(const QByteArray & array, const QString & url)
{
    QString content = Sk::readUtf8(array);

    // NOTE: When the m3u is too complex we let the player handle the parsing.
    if (content.contains("#EXT-X-MEDIA"))
    {
        medias.insert(WAbstractBackend::QualityDefault, url);

        return;
    }

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

void WControllerMediaData::applySource(const WYamlNodeBase & root,
                                       const WYamlNode     & node,
                                       const QString       & url,
                                       const QString       & baseUrl, int duration)
{
    timeB = timeA + duration;

    applyData(node, url, baseUrl);

    const WYamlNode * child = WControllerPlaylist::vbmlTemplate(root, node);

    if (child == NULL) return;

    if (source.isEmpty())
    {
        source = WControllerPlaylist::vbmlSourceSeed(child->extractString("source"),
                                                     child->extractString("seed"), baseUrl);
    }

    if (ambient.isEmpty())
    {
        ambient = WControllerPlaylist::vbmlSource(child->extractString("ambient"), baseUrl);
    }

    if (subtitles.isEmpty())
    {
        subtitles = WControllerPlaylist::vbmlSubtitles(*child, baseUrl);
    }
}

void WControllerMediaData::applyMedia(const WYamlNodeBase & root,
                                      const QString       & url, const QString & baseUrl)
{
    int durationSource = WControllerPlaylist::vbmlDuration(root, 0, -1);

    if (durationSource == -1)
    {
        applyData(root, url, baseUrl);

        return;
    }

    int at = root.extractMsecs("at");

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

    timeB = timeA + durationSource;

    applyData(root, url, baseUrl);
}

void WControllerMediaData::applyData(const WYamlNodeBase & node, const QString & url,
                                                                 const QString & baseUrl)
{
    source = WControllerPlaylist::vbmlSourceSeed(url, node.extractString("seed"), baseUrl);

    ambient = WControllerPlaylist::vbmlSource(node.extractString("ambient"), baseUrl);

    subtitles = WControllerPlaylist::vbmlSubtitles(node, baseUrl);
}

void WControllerMediaData::applyEmpty()
{
    if (currentTime >= duration) return;

    timeB = duration;

    start = 0;
}

int WControllerMediaData::extractDuration(const WYamlNode & child,
                                          QList<int>      * starts,
                                          QList<int>      * durations, int & start)
{
    int at = child.extractMsecs("at");

    int durationSource = WControllerPlaylist::vbmlDuration(child, at);

    durationSource -= start;

    if (durationSource <= 0)
    {
        start = -durationSource;

        starts   ->append(0);
        durations->append(0);

        return 0;
    }

    start += at;

    starts->append(start);

    durations->append(durationSource);

    if (durationSource > 0)
    {
        start = 0;

        return durationSource;
    }
    else
    {
        start = -durationSource;

        return 0;
    }
}

void WControllerMediaData::extractSource(const WYamlNodeBase    & root,
                                         const QList<WYamlNode> & children,
                                         const QString          & baseUrl)
{
    interpolateTime(children);

    for (int i = 0; i < children.length(); i++)
    {
        const WYamlNode & child = children.at(i);

        int at = child.extractMsecs("at");

        int durationSource = WControllerPlaylist::vbmlDuration(child, at);

        durationSource -= start;

        if (durationSource <= 0)
        {
            start = -durationSource;

            continue;
        }

        addChapter(child, timeA, baseUrl);

        int time = timeA + durationSource;

        if (currentTime >= time)
        {
            timeA = time;

            start = 0;

            continue;
        }

        start += at;

        typeSource = WControllerPlaylist::vbmlTrackType(child);

        const WYamlNode * node = child.at("source");

        if (node)
        {
            const QList<WYamlNode> & nodes = node->children;

            if (nodes.isEmpty())
            {
                applySource(root, child, node->value, baseUrl, durationSource);
            }
            else extractSource(root, WControllerPlaylist::vbmlSeed(child, *node), baseUrl);
        }
        else applySource(root, child, QString(), baseUrl, durationSource);

        time = timeB;

        for (int j = i + 1; j < children.length(); j++)
        {
            const WYamlNode & child = children.at(j);

            at = child.extractMsecs("at");

            durationSource = WControllerPlaylist::vbmlDuration(child, at);

            if (durationSource <= 0) continue;

            addChapter(child, time, baseUrl);

            time += durationSource;
        }

        return;
    }
}

void WControllerMediaData::extractSourceDuration(const WYamlNodeBase    & root,
                                                 const QList<WYamlNode> & children,
                                                 const QList<int>       & durations,
                                                 const QList<int>       & starts,
                                                 const QString          & baseUrl)
{
    interpolateTime(children);

    for (int i = 0; i < durations.length(); i++)
    {
        int durationSource = durations.at(i);

        if (durationSource <= 0) continue;

        const WYamlNode & child = children.at(i);

        addChapter(child, timeA, baseUrl);

        int time = timeA + durationSource;

        start = starts.at(i);

        if (currentTime >= time)
        {
            timeA = time;

            continue;
        }

        typeSource = WControllerPlaylist::vbmlTrackType(child);

        const WYamlNode * node = child.at("source");

        if (node)
        {
            const QList<WYamlNode> & nodes = node->children;

            if (nodes.isEmpty())
            {
                applySource(root, child, node->value, baseUrl, durationSource);
            }
            else extractSource(root, WControllerPlaylist::vbmlSeed(child, *node), baseUrl);
        }
        else applySource(root, child, QString(), baseUrl, durationSource);

        time = timeB;

        for (int j = i + 1; j < durations.length(); j++)
        {
            durationSource = durations.at(j);

            if (durationSource <= 0) continue;

            const WYamlNode & child = children.at(j);

            addChapter(child, time, baseUrl);

            time += durationSource;
        }

        return;
    }
}

int WControllerMediaData::extractSourceTimeline(const WYamlNodeBase                 & root,
                                                const QList<WControllerMediaObject> & timeline,
                                                const QString                       & baseUrl)
{
    for (int i = 0; i < timeline.count(); i++)
    {
        const WControllerMediaObject & object = timeline.at(i);

        WControllerMediaSource * media = object.media;

        if (media == NULL) continue;

        int durationSource = object.duration;

        if (durationSource <= 0) continue;

        int time = timeA + durationSource;

        if (currentTime >= time)
        {
            timeA = time;

            continue;
        }

        QString id = object.id;

        if (contextId.isEmpty())
        {
            contextId = id;
        }
        // NOTE: When the contextId do not match we consider that the currentTime is invalid.
        else if (contextId != id) break;

        start = object.at;

        const WYamlNode * child = media->node;

        typeSource = WControllerPlaylist::vbmlTrackType(*child);

        const WYamlNode * node = child->at("source");

        if (node)
        {
            const QList<WYamlNode> & nodes = node->children;

            if (nodes.isEmpty())
            {
                applySource(root, *child, node->value, baseUrl, durationSource);
            }
            else extractSource(root, WControllerPlaylist::vbmlSeed(*child, *node), baseUrl);
        }
        else applySource(root, *child, QString(), baseUrl, durationSource);

        if (source.isEmpty()) applyEmpty();

        return i;
    }

    return -1;
}

void WControllerMediaData::interpolateTime(const QList<WYamlNode> & children)
{
    if (typeSource != WTrack::Hub) return;

    //---------------------------------------------------------------------------------------------
    // NOTE: We consider a hub has an infinite length, so we interpolate the time cursor
    //       accordingly.

    int duration = WControllerPlaylist::vbmlDurationNodes(children);

    if (duration <= 0) return;

    int msec = currentTime - timeA + start;

    if (msec < duration) return;

    // NOTE: Parenthesis are required to avoid integer overflow.
    timeA += duration * (msec / duration) - start;

    start = 0;
}

void WControllerMediaData::addChapter(const WYamlNodeBase & node, int time,
                                      const QString       & baseUrl)
{
    WChapter chapter(time);

    chapter.setTitle(node.extractString("title"));

    chapter.setCover(WControllerPlaylist::vbmlSource(node.extractString("cover"), baseUrl));

    chapters.append(chapter);
}

int WControllerMediaData::updateCurrentTime(const WYamlNodeBase                 & root,
                                            const QList<WControllerMediaObject> & timeline,
                                            const QString                       & baseUrl)
{
    int index = -1;

    int time     = 0;
    int duration = 0;

    int gap = -1;

    for (int i = 0; i < timeline.count(); i++)
    {
        const WControllerMediaObject & object = timeline.at(i);

        WControllerMediaSource * media = object.media;

        if (media == NULL) continue;

        if (contextId != object.id)
        {
            duration += object.duration;

            continue;
        }

        int currentGap = qAbs(currentTime - duration);

        if (gap == -1 || currentGap <= gap)
        {
            gap = currentGap;

            time = duration;

            index = i;
        }

        duration += object.duration;
    }

    QString result = contextId;

    // NOTE: When we can't find a valid id we return the first media.
    if (index == -1)
    {
        for (int i = 0; i < timeline.count(); i++)
        {
            const WControllerMediaObject & object = timeline.at(i);

            WControllerMediaSource * media = object.media;

            if (media == NULL) continue;

            result = object.id;

            time = 0;

            index = i;

            break;
        }

        if (index == -1) return -1;
    }

    const WControllerMediaObject & object = timeline.at(index);

    currentTime = time;

    timeA = time;

    start = object.at;

    contextId = result;

    const WYamlNode * child = object.media->node;

    typeSource = WControllerPlaylist::vbmlTrackType(*child);

    const WYamlNode * node = child->at("source");

    if (node)
    {
        const QList<WYamlNode> & nodes = node->children;

        if (nodes.isEmpty())
        {
            applySource(root, *child, node->value, baseUrl, object.duration);
        }
        else extractSource(root, WControllerPlaylist::vbmlSeed(*child, *node), baseUrl);
    }
    else applySource(root, *child, QString(), baseUrl, object.duration);

    if (source.isEmpty()) applyEmpty();

    return index;
}

QString WControllerMediaData::cleanTimeline(QList<WControllerMediaObject> & timeline, int index,
                                            const QString                 & baseUrl)
{
    int gap = 0;

    int i = 0;

    while (i < timeline.count())
    {
        int at = getRedundancy(timeline, i);

        if (at == -1)
        {
            i++;

            continue;
        }

        int count = at - i;

        int from = at;

        for (int j = 0; j < count; j++)
        {
            if (from != index)
            {
                from++;

                continue;
            }

            //-------------------------------------------------------------------------------------
            // NOTE: We move our index to the prior index and adjust the duration accordingly.

            for (int k = 0; k < count; k++)
            {
                gap += timeline.at(index - k).duration;
            }

            index -= count;

            break;
        }

        for (int j = 0; j < count; j++)
        {
            const WControllerMediaObject & object = timeline.at(at);

            if (at < index)
            {
                gap += object.duration;

                index--;
            }

            timeline.removeAt(at);
        }

        i = 0;
    }

    if (gap)
    {
        currentTime -= gap;
        duration    -= gap;

        timeA -= gap;
        timeB -= gap;
    }

    //qDebug("CONTEXT AFTER %s %s", getContext(timeline).C_STR, contextId.C_STR);

    int time = 0;

    for (int i = 0; i < timeline.count(); i++)
    {
        const WControllerMediaObject & object = timeline.at(i);

        WControllerMediaSource * media = object.media;

        if (media == NULL) continue;

        const WYamlNode * node = media->node;

        WChapter chapter(time);

        QString title = node->extractString("title");

        if (title.isEmpty())
        {
            chapter.setTitle(media->id);
        }
        else chapter.setTitle(title);

        chapter.setCover(WControllerPlaylist::vbmlSource(node->extractString("cover"), baseUrl));

        chapters.append(chapter);

        time += object.duration;
    }

    return generateContext(timeline);
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* static */ QVariant WControllerMediaData::extractResult(const WYamlReader & reader,
                                                          const QString     & argument,
                                                          const QStringList & context,
                                                          const QString     & contextId)
{
    QStringList list = argument.split(',');

    if (list.isEmpty()) return QVariant();

    // NOTE: A routine name has a maximum length of 16 characters.
    QString routine = list.takeFirst().left(16).toUpper();

    WBackendUniversalScript script(reader.extractString(routine));

    if (script.isValid() == false) return QVariant();

    WBackendUniversalParameters parameters(script);

    if (list.isEmpty() == false)
    {
        parameters.add("argument", list.first());
        parameters.add("args",     list);
    }

    parameters.add("context", context);
    parameters.add("id",      contextId);

    return script.run(&parameters);
}

/* static */
QString WControllerMediaData::generateContext(const QList<WControllerMediaObject> & timeline)
{
    return WZipper::compressBase64(getContext(timeline).toUtf8());
}

/* static */
QString WControllerMediaData::getContext(const QList<WControllerMediaObject> & timeline)
{
    QString context;

    foreach (const WControllerMediaObject & object, timeline)
    {
        context.append(object.id + ',');
    }

    if (context.isEmpty()) return context;

    context.chop(1);

    return context;
}

/* static */
void WControllerMediaData::dumpTimeline(const QList<WControllerMediaObject> & timeline)
{
    QString context;

    foreach (const WControllerMediaObject & object, timeline)
    {
        context.append(object.id + ',');
    }

    if (context.isEmpty()) return;

    context.chop(1);

    qDebug("%s", context.C_STR);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

QStringList WControllerMediaData::getContext(const QList<WControllerMediaObject> & timeline,
                                             int                                   count) const
{
    QStringList result;

    for (int i = 0; i < count; i++)
    {
        result.append(timeline.at(i).id);
    }

    return result;
}

int WControllerMediaData::getRedundancy(const QList<WControllerMediaObject> & timeline,
                                        int                                   index) const
{
    QString id = timeline.at(index).id;

    index++;

    int indexB = getIndexFromId(timeline, id, index);

    if (indexB == -1) return -1;

    int indexC = indexB + 1;

    int count = indexC - index - 1;

    if (indexC + count > timeline.count()) return -1;

    while (count)
    {
        if (timeline.at(index).id != timeline.at(indexC).id) return -1;

        index++;

        indexC++;

        count--;
    }

    return indexB;
}

int WControllerMediaData::getIndexFromId(const QList<WControllerMediaObject> & timeline,
                                         const QString                       & id, int from) const
{
    while (from < timeline.count())
    {
        if (timeline.at(from).id == id) return from;

        from++;
    }

    return -1;
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
                                 const QString & urlBase,
                                 int             typeSource,
                                 int             currentTime,
                                 int             duration,
                                 int             timeA,
                                 int             start);

    Q_INVOKABLE void extractM3u(QIODevice * device, const QString & url);

signals:
    void loaded(QIODevice * device, const WControllerMediaData & data);
};

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerMediaReply::extractVbml(QIODevice     * device,
                                                          const QString & url,
                                                          const QString & urlBase,
                                                          int             typeSource,
                                                          int             currentTime,
                                                          int             duration,
                                                          int             timeA,
                                                          int             start)
{
    WControllerMediaData data;

    data.typeSource = static_cast<WTrack::Type> (typeSource);

    data.currentTime = currentTime;
    data.duration    = duration;

    data.timeA = timeA;

    data.start = start;

    data.applyVbml(WControllerFile::readAll(device), url, urlBase);

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

    methodVbml = meta->method(meta->indexOfMethod("extractVbml(QIODevice*,QString,QString,"
                                                  "int,int,int,int,int)"));

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

    WAbstractBackend::SourceMode mode = reply->_mode;

    // NOTE: The VBML uri is prioritized because it might contain an http url.
    if (WControllerPlaylist::urlIsVbmlUri(source))
    {
        if (source.startsWith("vbml://"))
        {
            // NOTE: We want to avoid large binary files.
            query.scope = WAbstractLoader::ScopeText;

            query.url = source.replace("vbml://", "https://");
        }
        else
        {
            query.type   = WBackendNetQuery::TypeVbml;
            query.target = WBackendNetQuery::TargetVbml;

            query.url = source;
        }
    }
    else
    {
        WBackendNet * backend = wControllerPlaylist->backendFromUrl(source);

        if (backend)
        {
            QString backendId = backend->id();

            query = backend->getQuerySource(source, mode);

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
    }

    query.mode = mode;

    WPrivateMediaData * media = new WPrivateMediaData;

    media->url       = url;
    media->urlSource = url;

    media->type       = WTrack::Unknown;
    media->typeSource = WTrack::Track;

    media->vbml = false;

    // NOTE: currentTime starts at zero to match the WControllerMediaData constraints.
    currentTime = qMax(0, currentTime);

    media->time        = currentTime;
    media->currentTime = currentTime;

    media->duration = -1;

    media->timeA =  0;
    media->timeB = -1;

    media->start = 0;

    media->backend = NULL;
    media->query   = query;
    media->reply   = NULL;

    media->replies.append(reply);

    medias.append(media);

    getData(media, &query);
}

void WControllerMediaPrivate::loadUrl(QIODevice               * device,
                                      const WBackendNetQuery  & query,
                                      const WPrivateMediaData & media) const
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
                                  Q_ARG(const QString &, media.url),
                                  Q_ARG(int,             media.typeSource),
                                  Q_ARG(int,             media.currentTime),
                                  Q_ARG(int,             media.duration),
                                  Q_ARG(int,             media.timeA),
                                  Q_ARG(int,             media.start));
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

    if (typeSource > WTrack::Track)
    {
        media->typeSource = typeSource;
    }

    media->chapters.append(data.chapters);

    if (data.ambient.isEmpty() == false)
    {
        media->ambient = data.ambient;
    }

    if (data.subtitles.isEmpty() == false)
    {
        media->subtitles = data.subtitles;
    }

    int timeB = data.timeB;

    if (timeB == -1) return;

    int duration = data.duration;

    media->timeA = data.timeA;

    media->start = data.start;

    if (duration == -1)
    {
        if (media->timeB == -1 || timeB < media->timeB)
        {
            media->timeB = timeB;
        }

        return;
    }

    if (data.timeZone.isEmpty() == false)
    {
        media->timeZone = data.timeZone;
    }

    media->currentTime = data.currentTime;

    media->duration = duration;

    media->timeB = timeB;

    media->context   = data.context;
    media->contextId = data.contextId;
}

void WControllerMediaPrivate::applyDataSlice(WPrivateMediaData        * media,
                                             const WPrivateMediaSlice & slice)
{
    WTrack::Type type = slice.type;

    if (media->type == WTrack::Unknown)
    {
        media->type = type;
    }

    WTrack::Type typeSource = slice.typeSource;

    if (typeSource > WTrack::Track)
    {
        media->typeSource = typeSource;
    }

    media->subtitles.append(slice.subtitles);

    if (media->timeB != -1)
    {
        applyChapters(media, slice.chapters);

        return;
    }

    media->chapters = slice.chapters;

    int duration = slice.duration;

    media->timeA = slice.timeA;

    media->start = slice.start;

    if (duration == -1)
    {
        media->timeB = slice.timeB;

        return;
    }

    if (slice.timeZone.isEmpty() == false)
    {
        media->timeZone = slice.timeZone;
    }

    media->duration = duration;

    media->timeB = slice.timeB;

    media->context   = slice.context;
    media->contextId = slice.contextId;
}

void WControllerMediaPrivate::applyChapters(WPrivateMediaData     * media,
                                            const QList<WChapter> & chapters)
{
    if (chapters.isEmpty()) return;

    int timeA = media->timeA;

    int duration = media->timeB - timeA;

    int start = media->start;

    foreach (const WChapter & chapter, chapters)
    {
        int time = chapter.time() - start;

        if (time < 0 || time >= duration) continue;

        media->chapters.append(chapter);

        // NOTE: We interpolate the track time to fit the current timeline.
        media->chapters.last().setTime(timeA + time);
    }
}

void WControllerMediaPrivate::applySource(WPrivateMediaData            * media,
                                          const WBackendNetSource      & source,
                                          WAbstractBackend::SourceMode   mode,
                                          bool                           cache)
{
    const QHash<WAbstractBackend::Quality, QString> & medias = source.medias;

    const QList<WChapter>  & chapters  = media->chapters;
    const QList<WSubtitle> & subtitles = media->subtitles;

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

        QString url       = media->url;
        QString urlSource = media->urlSource;

        WTrack::Type type       = media->type;
        WTrack::Type typeSource = media->typeSource;

        bool vbml = media->vbml;

        slice.urlSource = urlSource;

        slice.type       = type;
        slice.typeSource = typeSource;

        slice.vbml = vbml;

        slice.duration = -1;

        slice.timeA = -1;
        slice.timeB = -1;

        slice.start = 0;

        slice.medias = medias;
        slice.audios = audios;

        slice.chapters  = chapters;
        slice.subtitles = subtitles;

        slice.expiry = source.expiry;

        appendSlice(slice, url, mode);

        if (cache && urlSource != url)
        {
            slice.urlSource = QString();

            slice.type = typeSource;

            slice.vbml = false;

            slice.chapters  = QList<WChapter> ();
            slice.subtitles = QList<WSubtitle>();

            appendSlice(slice, urlSource, mode);
        }

        foreach (WMediaReply * reply, media->replies)
        {
            reply->_urlSource = urlSource;

            reply->_type       = type;
            reply->_typeSource = typeSource;

            reply->_vbml = vbml;

            reply->_medias = medias;
            reply->_audios = audios;

            reply->_chapters  = chapters;
            reply->_subtitles = subtitles;

            reply->_loaded = true;

            emit reply->loaded(reply);
        }
    }
    else
    {
        const QHash<WAbstractBackend::Quality, QString> & audios = source.audios;

        WPrivateMediaSlice slice;

        QString url       = media->url;
        QString urlSource = media->urlSource;

        WTrack::Type type       = media->type;
        WTrack::Type typeSource = media->typeSource;

        bool vbml = media->vbml;

        QString timeZone = media->timeZone;

        int currentTime = media->currentTime;
        int duration    = media->duration;

        int timeA = media->timeA;
        int timeB = media->timeB;

        int start = media->start;

        QString context   = media->context;
        QString contextId = media->contextId;

        QString ambient = media->ambient;

        slice.urlSource = urlSource;

        slice.type       = type;
        slice.typeSource = typeSource;

        slice.vbml = vbml;

        slice.timeZone = timeZone;

        slice.duration = duration;

        slice.timeA = timeA;
        slice.timeB = timeB;

        slice.start = start;

        slice.context   = context;
        slice.contextId = contextId;

        slice.medias = medias;
        slice.audios = audios;

        slice.chapters = chapters;

        slice.ambient = ambient;

        slice.subtitles = subtitles;

        slice.expiry = source.expiry;

        // NOTE: If the currentTime was updated we don't cache.
        if (media->time == currentTime)
        {
            appendSlice(slice, url, mode);

            if (cache && urlSource != url && medias.count())
            {
                slice.urlSource = QString();

                slice.type = typeSource;

                slice.vbml = false;

                slice.timeZone = QString();

                slice.duration = -1;

                slice.timeA = -1;
                slice.timeB = -1;

                slice.start = 0;

                slice.context   = QString();
                slice.contextId = QString();

                slice.chapters = QList<WChapter>();

                slice.ambient = QString();

                slice.subtitles = QList<WSubtitle>();

                appendSlice(slice, urlSource, mode);
            }
        }

        foreach (WMediaReply * reply, media->replies)
        {
            reply->_urlSource = urlSource;

            reply->_type       = type;
            reply->_typeSource = typeSource;

            reply->_vbml = vbml;

            reply->_timeZone = timeZone;

            reply->_currentTime = currentTime;
            reply->_duration    = duration;

            reply->_timeA = timeA;
            reply->_timeB = timeB;

            reply->_start = start;

            reply->_context   = context;
            reply->_contextId = contextId;

            reply->_medias = medias;
            reply->_audios = audios;

            reply->_chapters = chapters;

            reply->_ambient = ambient;

            reply->_subtitles = subtitles;

            reply->_loaded = true;

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

            // NOTE: We could check for doublons here, but these are unlikely so we favor
            //       performances over redundancy checking.

            /*int currentTime = slice.currentTime;

            int count = slices.count();

            for (int i = 0; i < count; i++)
            {
                const WPrivateMediaSlice & slice = slices.at(i);

                int timeA = slice.timeA;

                // NOTE: When the time is -1 it means the currentTime is irrelevant.
                if (timeA != -1
                    &&
                    (currentTime < timeA || currentTime >= slice.timeB)) continue;

                count--;

                // NOTE: We pop the slice at the top of the stack.
                slices.move(i, count);

                return;
            }*/

            while (slices.count() == CONTROLLERMEDIA_MAX_SLICES)
            {
                slices.removeLast();
            }

            slices.prepend(slice);
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

bool WControllerMediaPrivate::applyCache(WPrivateMediaData            * media,
                                         const QString                & url,
                                         WAbstractBackend::SourceMode   mode)
{
    WPrivateMediaSource * source = getSource(url);

    if (source == NULL) return false;

    const WPrivateMediaSlice * slice = getSlice(source, mode, media->currentTime);

    if (slice == NULL) return false;

    // NOTE: The typeSource has to be propagated corretly.

    applyDataSlice(media, *slice);

    WBackendNetSource backendSource;

    backendSource.medias = slice->medias;
    backendSource.audios = slice->audios;

    backendSource.expiry = slice->expiry;

    applySource(media, backendSource, mode, false);

    medias.removeOne(media);

    delete media;

    return true;
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

    query = backend->getQuerySource(query.url, query.mode);

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

        // NOTE: When the time is -1 it means the currentTime is irrelevant.
        if (timeA != -1
            &&
            (currentTime < timeA || currentTime >= slice.timeB)) continue;

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
        else loadUrl(reply, *backendQuery, *media);
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

    if (media->timeB == -1)
    {
        media->chapters = source.chapters;
    }
    else applyChapters(media, source.chapters);

    media->subtitles.append(source.subtitles);

    applySource(media, source, backendQuery.mode, true);

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

        if (indexNext == -1 || applyCache(media, origin, mode)) return;

        WBackendNet * backend = wControllerPlaylist->backendFromUrl(origin);

        if (backend)
        {
            QString backendId = backend->id();

            query = backend->getQuerySource(origin, mode);

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

    QString source = data.source;

    if (source.isEmpty())
    {
        WBackendNetSource backendSource;

        backendSource.medias = data.medias;

        applyData(media, data);

        applySource(media, backendSource, mode, true);

        medias.removeOne(media);

        delete media;

        return;
    }

    WBackendNet * backend = wControllerPlaylist->backendFromUrl(source);

    if (backend)
    {
        int indexNext = checkMax(media, query);

        if (indexNext == -1) return;

        QString backendId = backend->id();

        query = backend->getQuerySource(source, mode);

        backend->tryDelete();

        if (resolve(backendId, query))
        {
            query.indexNext = indexNext;

            // NOTE: We propagate the compatibility mode.
            query.mode = mode;

            media->urlSource = source;

            applyData(media, data);

            if (applyCache(media, source, mode)) return;

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

        if (applyCache(media, source, mode)) return;

        getData(media, &query);

        return;
    }
    else if (WControllerNetwork::urlIsHttp(source))
    {
        int indexNext = checkMax(media, query);

        if (indexNext == -1) return;

        query = WBackendNetQuery(source);

        // NOTE: We want to avoid large binary files.
        query.scope = WAbstractLoader::ScopeText;

        query.indexNext = indexNext;

        // NOTE: We propagate the compatibility mode.
        query.mode = mode;

        media->urlSource = source;

        applyData(media, data);

        if (applyCache(media, source, mode)) return;

        getData(media, &query);

        return;
    }

    media->urlSource = source;

    WBackendNetSource backendSource;

    // NOTE: If the parsing fails we add the current url as the default source.
    backendSource.medias.insert(WAbstractBackend::QualityDefault, source);

    applyData(media, data);

    applySource(media, backendSource, mode, true);

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

            reply->_currentTime = currentTime;

            reply->_duration = slice->duration;

            reply->_timeA = slice->timeA;
            reply->_timeB = slice->timeB;

            reply->_start = slice->start;

            reply->_context   = slice->context;
            reply->_contextId = slice->contextId;

            reply->_medias = slice->medias;
            reply->_audios = slice->audios;

            reply->_chapters = slice->chapters;

            reply->_ambient = slice->ambient;

            reply->_subtitles = slice->subtitles;

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
