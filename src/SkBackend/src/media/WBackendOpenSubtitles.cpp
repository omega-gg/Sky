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

// Sk includes
#include <WControllerApplication>
#include <WControllerNetwork>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WBackendNet_p>

class SK_BACKEND_EXPORT WBackendOpenSubtitlesPrivate : public WBackendNetPrivate
{
public:
    WBackendOpenSubtitlesPrivate(WBackendOpenSubtitles * p);

    void init();

public: // Functions
    QString getLanguage(const QString & language) const;

protected:
    W_DECLARE_PUBLIC(WBackendOpenSubtitles)
};

//-------------------------------------------------------------------------------------------------

WBackendOpenSubtitlesPrivate::WBackendOpenSubtitlesPrivate(WBackendOpenSubtitles * p)
    : WBackendNetPrivate(p) {}

void WBackendOpenSubtitlesPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

QString WBackendOpenSubtitlesPrivate::getLanguage(const QString & language) const
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
WBackendNetQuery WBackendOpenSubtitles::createQuery(const QString & method,
                                                    const QString & label, const QString & q) const
{
    WBackendNetQuery query;

    if (method == "subtitles")
    {
        Q_D(const WBackendOpenSubtitles);

        QString language = d->getLanguage(label);

        QString search = q.simplified();

        query.url = "https://www.opensubtitles.org/en/search/sublanguageid-" + language
                    +
                    "/subsumcd-1/subformat-srt/moviename-" + search.replace(' ', '+');

        query.data = search;
    }

    return query;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetFolder WBackendOpenSubtitles::extractFolder(const QByteArray       & data,
                                                       const WBackendNetQuery & query) const
{
    WBackendNetFolder reply;

    QString url = query.urlRedirect;

    // NOTE: We have been redirected to a subtitle page.
    if (url.contains("/subtitles/"))
    {
        WLibraryFolderItem item(WLibraryItem::Item, WLocalObject::Default);

        item.source = url;
        item.title  = query.data.toString();

        reply.items.append(item);

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

    return reply;
}

#endif // SK_NO_BACKENDOPENSUBTITLES
