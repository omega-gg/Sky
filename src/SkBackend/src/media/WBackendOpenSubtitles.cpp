//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkBackend module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WBackendOpenSubtitles.h"

#ifndef SK_NO_BACKENDOPENSUBTITLES

// Qt includes
#include <QBuffer>

// Sk includes
#include <WControllerApplication>
#include <WControllerNetwork>
#include <WControllerPlaylist>
#include <WUnzipper>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int BACKENDOPENSUBTITLES_DELAY   = 1000;  //  1 second
static const int BACKENDOPENSUBTITLES_TIMEOUT = 10000; // 10 seconds

static const QString BACKENDOPENSUBTITLES_MATCH = "[,.:\\-_(){}\\[\\]]";

static const QString BACKENDOPENSUBTITLES_START = "^[({\\[]";
static const QString BACKENDOPENSUBTITLES_END   =  "[)}\\]]";

static const QString BACKENDOPENSUBTITLES_FILTER = "720p|1080p|x264|bluray";

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WBackendNet_p>

class SK_BACKEND_EXPORT WBackendOpenSubtitlesPrivate : public WBackendNetPrivate
{
public:
    WBackendOpenSubtitlesPrivate(WBackendOpenSubtitles * p);

    void init();

public: // Static functions
    static void applyQuery(WBackendNetFolder * reply, const QString & language,
                                                      const QString & search, int id);

    static void applyNextQuery(const WBackendNetQuery & query,
                               WBackendNetFolder      * reply, int skip);

    static QString getUrl(const QString & label, const QString & search);

    static QString getQuery(const QString & data);

    static QString getLanguage(const QString & language);

protected:
    W_DECLARE_PUBLIC(WBackendOpenSubtitles)
};

//-------------------------------------------------------------------------------------------------

WBackendOpenSubtitlesPrivate::WBackendOpenSubtitlesPrivate(WBackendOpenSubtitles * p)
    : WBackendNetPrivate(p) {}

void WBackendOpenSubtitlesPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private static functions
//-------------------------------------------------------------------------------------------------

/* static */ void WBackendOpenSubtitlesPrivate::applyQuery(WBackendNetFolder * reply,
                                                           const QString     & language,
                                                           const QString     & search,
                                                           int                 id)
{
    WBackendNetQuery * nextQuery = &(reply->nextQuery);

    QStringList list;

    list.append(language);
    list.append(search);

    nextQuery->url = WBackendOpenSubtitlesPrivate::getUrl(language, search);

    nextQuery->id = id;

    nextQuery->data = list;
}

//-------------------------------------------------------------------------------------------------

/* static */ void WBackendOpenSubtitlesPrivate::applyNextQuery(const WBackendNetQuery & query,
                                                               WBackendNetFolder      * reply,
                                                               int                      skip)
{
    reply->clearDuplicate = true;

    if (reply->items.count() > 4) return;

    QStringList list = query.data.toStringList();

    QString search = list.last();

    int index = Sk::indexAt(search, " ", skip);

    if (index == -1) return;

    search = search.mid(0, index - 1);

    WBackendOpenSubtitlesPrivate::applyQuery(reply, list.first(), search, query.id + 1);
}

//-------------------------------------------------------------------------------------------------

/* static */ QString WBackendOpenSubtitlesPrivate::getUrl(const QString & language,
                                                          const QString & search)
{
    QString query = search;

    query.replace(' ', '+');

    return "https://www.opensubtitles.org/en/search/sublanguageid-" + language
           +
           "/subsumcd-1/subformat-srt/moviename-" + query;
}

//-------------------------------------------------------------------------------------------------

/* static */ QString WBackendOpenSubtitlesPrivate::getQuery(const QString & data)
{
    QString result = data;

    if (result.contains(QRegExp(BACKENDOPENSUBTITLES_START)))
    {
        int index = result.indexOf(QRegExp(BACKENDOPENSUBTITLES_END));

        if (index != -1)
        {
            result = result.mid(index + 1);
        }
    }

    QStringList listA = result.split(' ');

    while (listA.count() && listA.first().contains(QRegExp(BACKENDOPENSUBTITLES_START)))
    {
        listA.removeFirst();
    }

    result = listA.join(" ");

    result = result.replace(QRegExp(BACKENDOPENSUBTITLES_MATCH), " ");

    listA = result.simplified().toLower().split(' ');

    while (listA.count() && listA.first().toInt())
    {
        listA.removeFirst();
    }

    QStringList listB;

    foreach (const QString & string, listA)
    {
        if (string.length() < 4 || string.contains(QRegExp(BACKENDOPENSUBTITLES_FILTER))) continue;

        listB.append(string);

        if (listB.count() == 5) break;
    }

    return listB.join(" ");
}

//-------------------------------------------------------------------------------------------------

/* static */ QString WBackendOpenSubtitlesPrivate::getLanguage(const QString & language)
{
    if      (language == "afrikaans")   return "afr";
    else if (language == "albanian")    return "alb";
    else if (language == "arabic")      return "ara";
    else if (language == "aragonese")   return "arg";
    else if (language == "asturian")    return "ast";
    else if (language == "azerbaijani") return "aze";
    else if (language == "basque")      return "baq";
    else if (language == "belarusian")  return "bel";
    else if (language == "bosnian")     return "bos";
    else if (language == "breton")      return "bre";
    else if (language == "bulgarian")   return "bul";
    else if (language == "burmese")     return "bur";
    else if (language == "catalan")     return "cat";
    else if (language == "chinese #1")  return "chi";
    else if (language == "chinese #2")  return "zht";
    else if (language == "croatian")    return "hrv";
    else if (language == "czech")       return "cze";
    else if (language == "danish")      return "dan";
    else if (language == "dutch")       return "dut";
    else if (language == "english")     return "eng";
    else if (language == "esperanto")   return "epo";
    else if (language == "estonian")    return "est";
    else if (language == "finnish")     return "fin";
    else if (language == "french")      return "fre";
    else if (language == "gaelic")      return "gla";
    else if (language == "galician")    return "glg";
    else if (language == "georgian")    return "geo";
    else if (language == "german")      return "ger";
    else if (language == "greek")       return "ell";
    else if (language == "hebrew")      return "heb";
    else if (language == "hindi")       return "hin";
    else if (language == "hungarian")   return "hun";
    else if (language == "icelandic")   return "ice";
    else if (language == "indonesian")  return "ind";
    else if (language == "irish")       return "gle";
    else if (language == "italian")     return "ita";
    else if (language == "japanese")    return "jpn";
    else if (language == "kannada")     return "kan";
    else if (language == "kazakh")      return "kaz";
    else if (language == "khmer")       return "khm";
    else if (language == "korean")      return "kor";
    else if (language == "kurdish")     return "kur";
    else if (language == "latvian")     return "lav";
    else if (language == "lithuanian")  return "lit";
    else if (language == "macedonian")  return "mac";
    else if (language == "malay")       return "may";
    else if (language == "malayalam")   return "mal";
    else if (language == "norwegian")   return "nor";
    else if (language == "occitan")     return "oci";
    else if (language == "persian")     return "per";
    else if (language == "polish")      return "pol";
    else if (language == "portuguese")  return "por";
    else if (language == "romanian")    return "rum";
    else if (language == "russian")     return "rus";
    else if (language == "serbian")     return "scc";
    else if (language == "sinhala")     return "sin";
    else if (language == "slovak")      return "slo";
    else if (language == "slovenian")   return "slv";
    else if (language == "spanish")     return "spa";
    else if (language == "swedish")     return "swe";
    else if (language == "tamil")       return "tam";
    else if (language == "telugu")      return "tel";
    else if (language == "thai")        return "tha";
    else if (language == "turkish")     return "tur";
    else if (language == "ukranian")    return "urk";
    else if (language == "vietnamese")  return "vie";
    else                                return "eng";
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WBackendOpenSubtitles::WBackendOpenSubtitles()
    : WBackendNet(new WBackendOpenSubtitlesPrivate(this))
{
    Q_D(WBackendOpenSubtitles); d->init();
}

//-------------------------------------------------------------------------------------------------
// WBackendNet implementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendOpenSubtitles::getId() const
{
    return "opensubtitles";
}

/* Q_INVOKABLE virtual */ QString WBackendOpenSubtitles::getTitle() const
{
    return "OpenSubtitles";
}

//-------------------------------------------------------------------------------------------------
// WBackendNet reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendOpenSubtitles::checkValidUrl(const QString & url) const
{
    QString source = WControllerNetwork::removeUrlPrefix(url);

    return source.startsWith("opensubtitles.org");
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendOpenSubtitles::getQueryItem(const QString & url) const
{
    if (checkValidUrl(url) && url.contains("/subtitles/"))
    {
        WBackendNetQuery query;

        query.url = url;

        query.cookies = true;

        // FIXME OpenSubtitles: We have to delay our requests to avoid the captcha.
        query.delay   = BACKENDOPENSUBTITLES_DELAY;
        query.timeout = BACKENDOPENSUBTITLES_TIMEOUT;

        return query;
    }
    else return WBackendNetQuery();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendOpenSubtitles::createQuery(const QString & method,
                                                    const QString & label, const QString & q) const
{
    WBackendNetQuery query;

    if (method == "subtitles")
    {
        QString language = WBackendOpenSubtitlesPrivate::getLanguage(label);

        QString search = q.simplified();

        QStringList list;

        list.append(language);
        list.append(search);

        query.url = WBackendOpenSubtitlesPrivate::getUrl(language, search);

        query.data = list;

        query.cookies = true;

        // FIXME OpenSubtitles: We have to delay our requests to avoid the captcha.
        query.delay   = BACKENDOPENSUBTITLES_DELAY;
        query.timeout = BACKENDOPENSUBTITLES_TIMEOUT;
    }

    return query;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetFolder WBackendOpenSubtitles::extractFolder(const QByteArray       & data,
                                                       const WBackendNetQuery & query) const
{
    WBackendNetFolder reply;

    int id = query.id;

    QString url = query.urlRedirect;

    // NOTE: We have been redirected to a subtitle page.
    if (id == 0 && url.contains("/subtitles/"))
    {
        QStringList list = query.data.toStringList();

        QString search = list.last();

        WLibraryFolderItem item(WLibraryItem::Item, WLocalObject::Default);

        item.source = url;
        item.title  = search;

        reply.items.append(item);

        search = WBackendOpenSubtitlesPrivate::getQuery(search);

        WBackendOpenSubtitlesPrivate::applyQuery(&reply, list.first(), search, 1);

        return reply;
    }

    QString content = Sk::readUtf8(data);

    content = Sk::sliceIn(content, "<tbody>", "</tbody>");

    QStringList list = Sk::slices(content, "<tr ", "</tr>");

    foreach (const QString & string, list)
    {
        QString title = Sk::sliceIn(string, "<br />", "<br />");

        if (title.isEmpty()) continue;

        if (title.startsWith("<span "))
        {
            title = WControllerNetwork::extractAttribute(title, "title");
        }

        title = WControllerNetwork::htmlToUtf8(title).simplified();

        QString source = WControllerNetwork::extractAttribute(string, "href");

        WLibraryFolderItem item(WLibraryItem::Item, WLocalObject::Default);

        item.source = "https://www.opensubtitles.org" + source;
        item.title  = title;

        reply.items.append(item);
    }

    if (id == 1)
    {
        WBackendOpenSubtitlesPrivate::applyNextQuery(query, &reply, 2);
    }
    else if (id == 2)
    {
        WBackendOpenSubtitlesPrivate::applyNextQuery(query, &reply, 1);
    }
    else if (id == 3)
    {
        reply.clearDuplicate = true;
    }
    else if (reply.items.count() < 5)
    {
        QStringList list = query.data.toStringList();

        QString search = WBackendOpenSubtitlesPrivate::getQuery(list.last());

        WBackendOpenSubtitlesPrivate::applyQuery(&reply, list.first(), search, 1);
    }

    return reply;
}

/* Q_INVOKABLE virtual */
WBackendNetItem WBackendOpenSubtitles::extractItem(const QByteArray       & data,
                                                   const WBackendNetQuery & query) const
{
    WBackendNetItem reply;

    int id = query.id;

    if (id == 1)
    {
        QByteArray bytes = data;

        QBuffer buffer(&bytes);

        QStringList list = WUnzipper::getFileNames(&buffer);

        foreach (const QString & fileName, list)
        {
            QString extension = WControllerNetwork::extractUrlExtension(fileName);

            if (WControllerPlaylist::extensionIsSubtitle(extension) == false) continue;

            QByteArray bytes = WUnzipper::extractFile(&buffer, fileName);

            reply.data = bytes;

            reply.extension = extension;

            reply.cache = bytes;

            return reply;
        }
    }
    else
    {
        QString content = Sk::readUtf8(data);

        int index = content.indexOf("<a download");

        QString source = WControllerNetwork::extractAttribute(content, "href", index);

        index = content.indexOf("function dowSub", index);

        QString string = Sk::sliceIn(content, "vrf-", "'", index);

        source.replace("subtitleserve", "download/vrf-" + string);

        WBackendNetQuery * nextQuery = &(reply.nextQuery);

        nextQuery->url = "https://www.opensubtitles.org" + source;
        nextQuery->id  = 1;

        nextQuery->cookies = true;

        // FIXME OpenSubtitles: We have to delay our requests to avoid the captcha.
        nextQuery->delay   = BACKENDOPENSUBTITLES_DELAY;
        nextQuery->timeout = BACKENDOPENSUBTITLES_TIMEOUT;
    }

    return reply;
}

#endif // SK_NO_BACKENDOPENSUBTITLES
