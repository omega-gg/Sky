//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkCore.

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

#include "WControllerNetwork.h"

#ifndef SK_NO_CONTROLLERNETWORK

// Qt incudes
#include <QRegExp>
#include <QDateTime>

#ifdef QT_LATEST
#include <QUrlQuery>
#endif

// Sk incudes
#include <WControllerApplication>

W_INIT_CONTROLLER(WControllerNetwork)

//-------------------------------------------------------------------------------------------------
// Static variables

static const struct WControllerNetworkEntity
{
    const char * name;
    quint16      code;
}
CONTROLLERNETWORK_ENTITIES[258] =
{
    { "AElig",    0x00c6 },
    { "AMP",      38     },
    { "Aacute",   0x00c1 },
    { "Acirc",    0x00c2 },
    { "Agrave",   0x00c0 },
    { "Alpha",    0x0391 },
    { "Aring",    0x00c5 },
    { "Atilde",   0x00c3 },
    { "Auml",     0x00c4 },
    { "Beta",     0x0392 },
    { "Ccedil",   0x00c7 },
    { "Chi",      0x03a7 },
    { "Dagger",   0x2021 },
    { "Delta",    0x0394 },
    { "ETH",      0x00d0 },
    { "Eacute",   0x00c9 },
    { "Ecirc",    0x00ca },
    { "Egrave",   0x00c8 },
    { "Epsilon",  0x0395 },
    { "Eta",      0x0397 },
    { "Euml",     0x00cb },
    { "GT",       62     },
    { "Gamma",    0x0393 },
    { "Iacute",   0x00cd },
    { "Icirc",    0x00ce },
    { "Igrave",   0x00cc },
    { "Iota",     0x0399 },
    { "Iuml",     0x00cf },
    { "Kappa",    0x039a },
    { "LT",       60     },
    { "Lambda",   0x039b },
    { "Mu",       0x039c },
    { "Ntilde",   0x00d1 },
    { "Nu",       0x039d },
    { "OElig",    0x0152 },
    { "Oacute",   0x00d3 },
    { "Ocirc",    0x00d4 },
    { "Ograve",   0x00d2 },
    { "Omega",    0x03a9 },
    { "Omicron",  0x039f },
    { "Oslash",   0x00d8 },
    { "Otilde",   0x00d5 },
    { "Ouml",     0x00d6 },
    { "Phi",      0x03a6 },
    { "Pi",       0x03a0 },
    { "Prime",    0x2033 },
    { "Psi",      0x03a8 },
    { "QUOT",     34     },
    { "Rho",      0x03a1 },
    { "Scaron",   0x0160 },
    { "Sigma",    0x03a3 },
    { "THORN",    0x00de },
    { "Tau",      0x03a4 },
    { "Theta",    0x0398 },
    { "Uacute",   0x00da },
    { "Ucirc",    0x00db },
    { "Ugrave",   0x00d9 },
    { "Upsilon",  0x03a5 },
    { "Uuml",     0x00dc },
    { "Xi",       0x039e },
    { "Yacute",   0x00dd },
    { "Yuml",     0x0178 },
    { "Zeta",     0x0396 },
    { "aacute",   0x00e1 },
    { "acirc",    0x00e2 },
    { "acute",    0x00b4 },
    { "aelig",    0x00e6 },
    { "agrave",   0x00e0 },
    { "alefsym",  0x2135 },
    { "alpha",    0x03b1 },
    { "amp",      38     },
    { "and",      0x22a5 },
    { "ang",      0x2220 },
    { "apos",     0x0027 },
    { "aring",    0x00e5 },
    { "asymp",    0x2248 },
    { "atilde",   0x00e3 },
    { "auml",     0x00e4 },
    { "bdquo",    0x201e },
    { "beta",     0x03b2 },
    { "brvbar",   0x00a6 },
    { "bull",     0x2022 },
    { "cap",      0x2229 },
    { "ccedil",   0x00e7 },
    { "cedil",    0x00b8 },
    { "cent",     0x00a2 },
    { "chi",      0x03c7 },
    { "circ",     0x02c6 },
    { "clubs",    0x2663 },
    { "cong",     0x2245 },
    { "copy",     0x00a9 },
    { "crarr",    0x21b5 },
    { "cup",      0x222a },
    { "curren",   0x00a4 },
    { "dArr",     0x21d3 },
    { "dagger",   0x2020 },
    { "darr",     0x2193 },
    { "deg",      0x00b0 },
    { "delta",    0x03b4 },
    { "diams",    0x2666 },
    { "divide",   0x00f7 },
    { "eacute",   0x00e9 },
    { "ecirc",    0x00ea },
    { "egrave",   0x00e8 },
    { "empty",    0x2205 },
    { "emsp",     0x2003 },
    { "ensp",     0x2002 },
    { "epsilon",  0x03b5 },
    { "equiv",    0x2261 },
    { "eta",      0x03b7 },
    { "eth",      0x00f0 },
    { "euml",     0x00eb },
    { "euro",     0x20ac },
    { "exist",    0x2203 },
    { "fnof",     0x0192 },
    { "forall",   0x2200 },
    { "frac12",   0x00bd },
    { "frac14",   0x00bc },
    { "frac34",   0x00be },
    { "frasl",    0x2044 },
    { "gamma",    0x03b3 },
    { "ge",       0x2265 },
    { "gt",       62     },
    { "hArr",     0x21d4 },
    { "harr",     0x2194 },
    { "hearts",   0x2665 },
    { "hellip",   0x2026 },
    { "iacute",   0x00ed },
    { "icirc",    0x00ee },
    { "iexcl",    0x00a1 },
    { "igrave",   0x00ec },
    { "image",    0x2111 },
    { "infin",    0x221e },
    { "int",      0x222b },
    { "iota",     0x03b9 },
    { "iquest",   0x00bf },
    { "isin",     0x2208 },
    { "iuml",     0x00ef },
    { "kappa",    0x03ba },
    { "lArr",     0x21d0 },
    { "lambda",   0x03bb },
    { "lang",     0x2329 },
    { "laquo",    0x00ab },
    { "larr",     0x2190 },
    { "lceil",    0x2308 },
    { "ldquo",    0x201c },
    { "le",       0x2264 },
    { "lfloor",   0x230a },
    { "lowast",   0x2217 },
    { "loz",      0x25ca },
    { "lrm",      0x200e },
    { "lsaquo",   0x2039 },
    { "lsquo",    0x2018 },
    { "lt",       60     },
    { "macr",     0x00af },
    { "mdash",    0x2014 },
    { "micro",    0x00b5 },
    { "middot",   0x00b7 },
    { "minus",    0x2212 },
    { "mu",       0x03bc },
    { "nabla",    0x2207 },
    { "nbsp",     0x00a0 },
    { "ndash",    0x2013 },
    { "ne",       0x2260 },
    { "ni",       0x220b },
    { "not",      0x00ac },
    { "notin",    0x2209 },
    { "nsub",     0x2284 },
    { "ntilde",   0x00f1 },
    { "nu",       0x03bd },
    { "oacute",   0x00f3 },
    { "ocirc",    0x00f4 },
    { "oelig",    0x0153 },
    { "ograve",   0x00f2 },
    { "oline",    0x203e },
    { "omega",    0x03c9 },
    { "omicron",  0x03bf },
    { "oplus",    0x2295 },
    { "or",       0x22a6 },
    { "ordf",     0x00aa },
    { "ordm",     0x00ba },
    { "oslash",   0x00f8 },
    { "otilde",   0x00f5 },
    { "otimes",   0x2297 },
    { "ouml",     0x00f6 },
    { "para",     0x00b6 },
    { "part",     0x2202 },
    { "percnt",   0x0025 },
    { "permil",   0x2030 },
    { "perp",     0x22a5 },
    { "phi",      0x03c6 },
    { "pi",       0x03c0 },
    { "piv",      0x03d6 },
    { "plusmn",   0x00b1 },
    { "pound",    0x00a3 },
    { "prime",    0x2032 },
    { "prod",     0x220f },
    { "prop",     0x221d },
    { "psi",      0x03c8 },
    { "quot",     34     },
    { "rArr",     0x21d2 },
    { "radic",    0x221a },
    { "rang",     0x232a },
    { "raquo",    0x00bb },
    { "rarr",     0x2192 },
    { "rceil",    0x2309 },
    { "rdquo",    0x201d },
    { "real",     0x211c },
    { "reg",      0x00ae },
    { "rfloor",   0x230b },
    { "rho",      0x03c1 },
    { "rlm",      0x200f },
    { "rsaquo",   0x203a },
    { "rsquo",    0x2019 },
    { "sbquo",    0x201a },
    { "scaron",   0x0161 },
    { "sdot",     0x22c5 },
    { "sect",     0x00a7 },
    { "shy",      0x00ad },
    { "sigma",    0x03c3 },
    { "sigmaf",   0x03c2 },
    { "sim",      0x223c },
    { "spades",   0x2660 },
    { "sub",      0x2282 },
    { "sube",     0x2286 },
    { "sum",      0x2211 },
    { "sup",      0x2283 },
    { "sup1",     0x00b9 },
    { "sup2",     0x00b2 },
    { "sup3",     0x00b3 },
    { "supe",     0x2287 },
    { "szlig",    0x00df },
    { "tau",      0x03c4 },
    { "there4",   0x2234 },
    { "theta",    0x03b8 },
    { "thetasym", 0x03d1 },
    { "thinsp",   0x2009 },
    { "thorn",    0x00fe },
    { "tilde",    0x02dc },
    { "times",    0x00d7 },
    { "trade",    0x2122 },
    { "uArr",     0x21d1 },
    { "uacute",   0x00fa },
    { "uarr",     0x2191 },
    { "ucirc",    0x00fb },
    { "ugrave",   0x00f9 },
    { "uml",      0x00a8 },
    { "upsih",    0x03d2 },
    { "upsilon",  0x03c5 },
    { "uuml",     0x00fc },
    { "weierp",   0x2118 },
    { "xi",       0x03be },
    { "yacute",   0x00fd },
    { "yen",      0x00a5 },
    { "yuml",     0x00ff },
    { "zeta",     0x03b6 },
    { "zwj",      0x200d },
    { "zwnj",     0x200c }
};

//-------------------------------------------------------------------------------------------------

static const ushort CONTROLLERNETWORK_EXTENDED[0xA0 - 0x80] =
{
    0x20ac, // 0x80
    0x0081, // 0x81 direct mapping
    0x201a, // 0x82
    0x0192, // 0x83
    0x201e, // 0x84
    0x2026, // 0x85
    0x2020, // 0x86
    0x2021, // 0x87
    0x02C6, // 0x88
    0x2030, // 0x89
    0x0160, // 0x8A
    0x2039, // 0x8B
    0x0152, // 0x8C
    0x008D, // 0x8D direct mapping
    0x017D, // 0x8E
    0x008F, // 0x8F direct mapping
    0x0090, // 0x90 direct mapping
    0x2018, // 0x91
    0x2019, // 0x92
    0x201C, // 0x93
    0X201D, // 0x94
    0x2022, // 0x95
    0x2013, // 0x96
    0x2014, // 0x97
    0x02DC, // 0x98
    0x2122, // 0x99
    0x0161, // 0x9A
    0x203A, // 0x9B
    0x0153, // 0x9C
    0x009D, // 0x9D direct mapping
    0x017E, // 0x9E
    0x0178  // 0x9F
};

//-------------------------------------------------------------------------------------------------
// Inline functions

inline bool operator<(const QString & string, const WControllerNetworkEntity & entity)
{
    return string < entity.name;
}

inline bool operator<(const WControllerNetworkEntity & entity, const QString & string)
{
    return entity.name < string;
}

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WControllerNetworkPrivate::WControllerNetworkPrivate(WControllerNetwork * p)
    : WControllerPrivate(p) {}

/* virtual */ WControllerNetworkPrivate::~WControllerNetworkPrivate()
{
    socketTimer     .stop();
    socketRetryTimer.stop();

    socket.disconnect();

    socket.disconnectFromHost();

    W_CLEAR_CONTROLLER(WControllerNetwork);
}

//-------------------------------------------------------------------------------------------------

void WControllerNetworkPrivate::init()
{
    Q_Q(WControllerNetwork);

    checkConnected = false;
    connected      = false;

    socketTimer.setInterval(10000);

    QObject::connect(&socketTimer,      SIGNAL(timeout()), q, SLOT(onSocketTimeout()));
    QObject::connect(&socketRetryTimer, SIGNAL(timeout()), q, SLOT(onRetryTimeout ()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WControllerNetworkPrivate::checkConnection()
{
    if (socket.state() != QAbstractSocket::UnconnectedState) return;

    socket.connectToHost("www.google.com", 80);

    qDebug("Retrying");

    socketTimer.start();

    socketRetryTimer.stop();
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* static */ int WControllerNetworkPrivate::indexJsonEndA(const QString & text, int at)
{
    at = text.indexOf('"', at + 1);

    if (at == -1) return -1;

    while (Sk::checkEscaped(text, at))
    {
        at = text.indexOf('"', at + 1);

        if (at == -1) return -1;
    }

    return at;
}

/* static */ int WControllerNetworkPrivate::indexJsonEndB(const QString & text, int at,
                                                          const QChar   & charA)
{
    QChar charB;

    if (charA == '{')
    {
        charB = '}';
    }
    else if (charA == '[')
    {
        charB = ']';
    }
    else
    {
        int index = text.indexOf(QRegExp("[,\"}\\]]"), at);

        if (index == -1)
        {
             return text.length();
        }
        else return index;
    }

    at++;

    while (at < text.length())
    {
        int indexA = text.indexOf('"', at);

        if (indexA != -1)
        {
            while (Sk::checkEscaped(text, indexA))
            {
                indexA = text.indexOf('"', indexA + 1);

                if (indexA == -1) break;
            }
        }

        int indexB = text.indexOf(charA, at);

        if (indexB != -1 && (indexA == -1 || indexB < indexA))
        {
            indexA = indexB;
        }

        indexB = text.indexOf(charB, at);

        if (indexA != -1 && indexA < indexB)
        {
            QChar character = text.at(indexA);

            if (character == '"')
            {
                 at = indexJsonEndA(text, indexA);
            }
            else at = indexJsonEndB(text, indexA, character);

            if (at == -1) return -1;

            at++;
        }
        else return indexB;
    }

    return -1;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WControllerNetworkPrivate::onStateChanged(QAbstractSocket::SocketState state)
{
    if (state == QAbstractSocket::ConnectedState)
    {
        socketTimer.stop();

        socketRetryTimer.start(60000);

        if (connected == false)
        {
            Q_Q(WControllerNetwork);

            connected = true;

            qDebug("WControllerNetwork: Connected");

            emit q->connectedChanged(true);
        }
    }
    else if (state == QAbstractSocket::UnconnectedState)
    {
        socketTimer.stop();

        socketRetryTimer.start(5000);

        if (connected)
        {
            Q_Q(WControllerNetwork);

            connected = false;

            qDebug("WControllerNetwork: Disconnected");

            emit q->connectedChanged(false);
        }
    }
}

void WControllerNetworkPrivate::onSocketTimeout()
{
    qDebug("WControllerNetwork: Connection timeout");

    socketTimer.stop();

    socket.abort();

    socketRetryTimer.start(5000);

    if (connected)
    {
        Q_Q(WControllerNetwork);

        connected = false;

        emit q->connectedChanged(connected);
    }
}

void WControllerNetworkPrivate::onRetryTimeout()
{
    socketRetryTimer.stop();

    if (connected)
    {
        Q_Q(WControllerNetwork);

        socket.disconnect();

        socket.disconnectFromHost();

        checkConnection();

        QObject::connect(&socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                         q,       SLOT(onStateChanged(QAbstractSocket::SocketState)));
    }
    else
    {
        qDebug("WControllerNetwork: Retrying connection");

        checkConnection();
    }
}

//-------------------------------------------------------------------------------------------------
// Private ctor / dtor
//-------------------------------------------------------------------------------------------------

WControllerNetwork::WControllerNetwork() : WController(new WControllerNetworkPrivate(this)) {}

//-------------------------------------------------------------------------------------------------
// Initialize
//-------------------------------------------------------------------------------------------------

/* virtual */ void WControllerNetwork::init()
{
    Q_D(WControllerNetwork); d->init();
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ bool WControllerNetwork::textIsUri(const QString & text)
{
#ifdef Q_OS_WIN
    if (textIsUrl(text) || text.startsWith('/') || text.startsWith('\\'))
#else
    if (textIsUrl(text) || text.startsWith('/'))
#endif
    {
        return true;
    }
    else if (text.contains(':'))
    {
        if (text.length() > 1)
        {
            if (text.at(0).isLetter() || text.contains(' ') == false)
            {
                return true;
            }
        }

        return false;
    }
    else if (text.contains('.') && text.contains(' ') == false)
    {
        return true;
    }
    else return false;
}

/* Q_INVOKABLE static */ bool WControllerNetwork::textIsUrl(const QString & text)
{
     if (urlIsFile(text) || urlIsHttp(text))
     {
          return true;
     }
     else return false;
}

/* Q_INVOKABLE static */ bool WControllerNetwork::textIsIp(const QString & text)
{
    foreach (const QChar & character, text)
    {
        if (character.isDigit() == false && character != '.' && character != ':')
        {
            return false;
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ bool WControllerNetwork::urlIsFile(const QString & string)
{
    if (string.startsWith("file://"))
    {
         return true;
    }
    else return false;
}

/* Q_INVOKABLE static */ bool WControllerNetwork::urlIsHttp(const QString & string)
{
    if (string.startsWith("http://") || string.startsWith("https://"))
    {
         return true;
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::urlName(const QUrl & url)
{
    QString name = url.host();

    if (name.isEmpty())
    {
        return WControllerNetwork::decodeUrl(url.toString());
    }

    if (name.startsWith("www."))
    {
        name.remove(0, 4);
    }

    return name;
}

/* Q_INVOKABLE static */ QString WControllerNetwork::urlTitle(const QUrl & url)
{
    QString title = url.host();

    int count = title.count('.');

    if (count == 1)
    {
        title = Sk::sliceIn(title, "", ".");

        return Sk::capitalize(title);
    }
    else if (count == 2 || count == 3)
    {
        title = Sk::sliceIn(title, ".", ".");

        return Sk::capitalize(title);
    }
    else return title;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::urlScheme(const QString & string)
{
    QString result;

    int index = 0;

    while (index < string.length())
    {
        QChar character = string.at(index);

        if (character.isLetterOrNumber() == false)
        {
            if (character == ':')
            {
                 return result;
            }
            else return QString();
        }
        else result.append(character);

        index++;
    }

    return QString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::generateUrl(const QString & string,
                                                                 const QString & baseUrl)
{
    if (urlScheme(string).isEmpty() == false)
    {
        return string;
    }
    else if (string.startsWith("www."))
    {
        QString result = string;

        result.prepend("http://");

        return result;
    }
    else if (string.startsWith("//"))
    {
        QString result = string;

        result.prepend("http:");

        return result;
    }
    else if (baseUrl.isEmpty())
    {
        return string;
    }
    else if (baseUrl.endsWith('/'))
    {
         return resolveUrl(string, baseUrl);
    }
    else return resolveUrl(string, baseUrl + '/');
}

/* Q_INVOKABLE static */ QString WControllerNetwork::resolveUrl(const QString & string,
                                                                const QString & baseUrl)
{
    return QUrl(baseUrl).resolved(string).toString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::encodeUrl(const QString & string)
{
    return QUrl::toPercentEncoding(string);
}

/* Q_INVOKABLE static */ QUrl WControllerNetwork::encodedUrl(const QString & string)
{
    return QUrl::fromEncoded(string.toUtf8());
}

/* Q_INVOKABLE static */ QString WControllerNetwork::decodeUrl(const QString & string)
{
    return QUrl::fromPercentEncoding(string.toUtf8());
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::extractBaseUrl(const QString & string)
{
    QString url = removeUrlExtension(string);

    int index = url.lastIndexOf('/');

    if (index == -1) return string;

    int indexUrl = url.indexOf("//");

    if (indexUrl != -1 && index < indexUrl + 2)
    {
        return url;
    }
    else if (url.indexOf('.', index + 1) != -1)
    {
        url = url.mid(0, index);
    }

    if (url.endsWith('/'))
    {
        url.chop(1);
    }

    return url;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::extractUrlHost(const QString & string)
{
    int index = string.indexOf("//");

    if (index == -1)
    {
         index = string.indexOf("/");
    }
    else index = string.indexOf("/", index + 2);

    if (index == -1)
    {
         return string;
    }
    else return string.mid(0, index);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractUrlPath(const QString & string)
{
    int index = string.indexOf("//");

    if (index != -1)
    {
        index = string.indexOf("/", index + 2);

        if (index != -1)
        {
            index++;

            return string.mid(index);
        }
        else return QString();
    }
    else if (string.startsWith("/"))
    {
         return string.mid(1);
    }
    else return string;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::extractUrlValue(const QUrl    & url,
                                                                     const QString & key)
{
#ifdef QT_4
    return url.queryItemValue(key);
#else
    return QUrlQuery(url).queryItemValue(key);
#endif
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ int WControllerNetwork::indexUrlElement(const QString & string,
                                                                 int             from)
{
    return string.indexOf(QRegExp("[/\\?&#]"), from);
}

/* Q_INVOKABLE static */ int WControllerNetwork::indexUrlElement(const QString & string,
                                                                 int             index,
                                                                 int             from)
{
    int at = string.length();

    while (index)
    {
        at = indexUrlElement(string, from);

        if (at == -1)
        {
            return string.length();
        }

        from = at + 1;

        index--;
    }

    return at;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::extractUrlElement(const QString & string,
                                                                       int             from)
{
    int to = indexUrlElement(string, from);

    if (to == -1)
    {
         return string.mid(from);
    }
    else return string.mid(from, to - from);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractUrlElement(const QString & string,
                                                                       int             index,
                                                                       int             from)
{
    int to = indexUrlElement(string, index, from);

    return string.mid(from, to - from);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractUrlElements(const QString & string,
                                                                        int             count,
                                                                        int             from)
{
    if (count < 1) return QString();

    int at = from;

    int index = 0;

    while (count)
    {
        index = indexUrlElement(string, at);

        if (index == -1)
        {
            return string.mid(from);
        }

        at = index + 1;

        count--;
    }

    return string.mid(from, index - from);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::extractUrlFileName(const QString & string)
{
    QString fileName = removeUrlExtension(string);

    int index = fileName.lastIndexOf('/');

    if (index == -1) return fileName;

    int indexUrl = fileName.indexOf("//");

    if (indexUrl != -1 && index < indexUrl + 2)
    {
         return QString();
    }
    else return fileName.mid(index + 1);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractUrlExtension(const QString & string)
{
    QString fileName = extractUrlFileName(string);

    int index = fileName.lastIndexOf('.');

    if (index != -1)
    {
        QString extension = fileName.mid(index + 1);

        foreach (const QChar & character, extension)
        {
            if (character.isLetterOrNumber() == false)
            {
                return QString();
            }
        }

        return extension.toLower();
    }
    else return QString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::extractFragmentValue(const QString & string,
                                                                          const QString & key)
{
    int indexA = string.lastIndexOf('#');

    if (indexA == -1) return QString();

    indexA = string.indexOf(key + '=', indexA + 1);

    if (indexA == -1) return QString();

    indexA += key.length() + 1;

    int indexB = string.indexOf('&', indexA);

    if (indexB == -1)
    {
         return string.mid(indexA);
    }
    else return string.mid(indexA, indexB - indexA);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::applyFragmentValue(const QString & string,
                                                                        const QString & key,
                                                                        const QString & value)
{
    int indexA = string.lastIndexOf('#');

    if (indexA == -1)
    {
        return string + '#' + key + '=' + value;
    }

    indexA = string.indexOf(key + '=', indexA + 1);

    if (indexA == -1)
    {
        return string + '&' + key + '=' + value;
    }

    indexA += key.length() + 1;

    int indexB = string.indexOf('&', indexA);

    QString result = string;

    if (indexB == -1)
    {
         result.replace(indexA, result.length() - indexA, value);
    }
    else result.replace(indexA, indexB - indexA, value);

    return result;
}

/* Q_INVOKABLE static */ QString WControllerNetwork::removeFragmentValue(const QString & string,
                                                                         const QString & key)
{
    int indexA = string.lastIndexOf('#');

    if (indexA == -1) return string;

    indexA = string.indexOf(key + '=', indexA + 1);

    if (indexA == -1) return string;

    int indexB = string.indexOf('&', indexA);

    QString result = string;

    if (indexB == -1)
    {
        // NOTE: We have to remove the '#' or '&' prior to the key.
        indexA--;

         result.remove(indexA, result.length() - indexA);
    }
    else
    {
        // NOTE: We have to remove the '&' after the value.
        indexB++;

        result.remove(indexA, indexB - indexA);
    }

    return result;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::removeUrlPrefix(const QString & string)
{
    int index = string.indexOf("//");

    if (index != -1)
    {
        index += 2;

        while (index < string.length() && string.at(index) == '/')
        {
            index++;
        }
    }

    int indexHost = string.indexOf("www.", index);

    if (indexHost != -1 && indexHost == index)
    {
        indexHost += 4;

        return string.mid(indexHost);
    }
    else if (index == -1)
    {
        return string;
    }
    else return string.mid(index);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::removeUrlExtension(const QString & string)
{
    int index = string.indexOf(QRegExp("[\\?#]"));

    if (index == -1)
    {
         return string;
    }
    else return string.mid(0, index);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::removeUrlFragment(const QString & string)
{
    int index = string.indexOf('#');

    if (index == -1)
    {
         return string;
    }
    else return string.mid(0, index);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::removeFileExtension(const QString & string)
{
    int index = string.lastIndexOf('.');

    if (index == -1)
    {
         return string;
    }
    else return string.mid(0, index);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::extractIpBase(const QString & string)
{
    int index = string.indexOf(':');

    if (index == -1)
    {
         return string;
    }
    else return string.mid(0, index);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractIpPort(const QString & string)
{
    int index = string.lastIndexOf(':');

    if (index != -1)
    {
        index++;

        return string.mid(index);
    }
    else return QString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ void WControllerNetwork::appendUrls(QStringList   * list,
                                                             const QString & text,
                                                             const QString & tag,
                                                             const QString & attribute,
                                                             const QString & baseUrl,
                                                             const QString & pattern)
{
    Q_ASSERT(list);

    QString matchA = '<' + tag + ' ';

    QString matchB;

    if (pattern.isEmpty())
    {
         matchB = ' ' + attribute + '=';
    }
    else matchB = ' ' + attribute + "=\"" + pattern;

    int length = attribute.length() + 3;

    int index = text.indexOf(matchA);

    while (index != -1)
    {
        int indexA = text.indexOf(matchB, index);

        int indexB = text.indexOf('>', index);

        if (indexA != -1 && indexA < indexB)
        {
            QString url = extractUrlAt(text, baseUrl, indexA + length);

            if (url.isEmpty() == false && list->contains(url) == false)
            {
                list->append(url);
            }
        }

        index = text.indexOf(matchA, index + 1);
    }
}

/* Q_INVOKABLE static */ QStringList WControllerNetwork::extractUrls(const QString & text,
                                                                     const QString & tag,
                                                                     const QString & attribute,
                                                                     const QString & baseUrl,
                                                                     const QString & pattern)
{
    QStringList urls;

    appendUrls(&urls, text, tag, attribute, baseUrl, pattern);

    return urls;
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractUrlAt(const QString & text,
                                                                  const QString & baseUrl,
                                                                  int             index)
{
    QString url;

    for (int i = index; i < text.length() && text[i] != '"' && text[i] != '\''; i++)
    {
        url.append(text[i]);
    }

    return generateUrl(url, baseUrl);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::getUrlRedirect(const QUrl & origin,
                                                                    const QUrl & redirect)
{
    QString source;

    if (redirect.isRelative())
    {
         source = origin.resolved(redirect).toString();
    }
    else source = redirect.toString();

    int index = source.lastIndexOf("http://");

    if (index > 0)
    {
         return source.mid(index);
    }
    else return source;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QUrl WControllerNetwork::getUrlOAuth(const QUrl       & url,
                                                              const QByteArray & httpMethod,
                                                              const QByteArray & key,
                                                              const QByteArray & secret,
                                                              const QByteArray & token)
{
    QByteArray nonce = sk->generateRandomString(16);

    uint time = QDateTime::currentDateTime().toTime_t();

    QUrl source(url);

#ifdef QT_4
    source.addQueryItem("oauth_consumer_key", key);
    source.addQueryItem("oauth_nonce",        nonce);
    source.addQueryItem("oauth_timestamp",    QByteArray::number(time));

    if (token.isEmpty() == false)
    {
        source.addQueryItem("oauth_token", token);
    }

    source.addQueryItem("oauth_signature_method", "HMAC-SHA1");
#else
    QUrlQuery query(source);

    query.addQueryItem("oauth_consumer_key", key);
    query.addQueryItem("oauth_nonce",        nonce);
    query.addQueryItem("oauth_timestamp",    QByteArray::number(time));

    if (token.isEmpty() == false)
    {
        query.addQueryItem("oauth_token", token);
    }

    query.addQueryItem("oauth_signature_method", "HMAC-SHA1");
#endif

    //---------------------------------------------------------------------------------------------

    QByteArray signature;

#ifdef QT_4
    QList<QPair<QByteArray, QByteArray> > list = source.encodedQueryItems();
#else
    QList<QPair<QString, QString> > list = query.queryItems(QUrl::FullyEncoded);
#endif

    std::sort(list.begin(), list.end());

    for (int i = 0; i < list.count(); i++)
    {
#ifdef QT_4
        QPair<QByteArray, QByteArray> pair = list.at(i);
#else
        QPair<QString, QString> pair = list.at(i);
#endif

        signature.append(pair.first);
        signature.append('=');
        signature.append(pair.second);
        signature.append('&');
    }

    signature.chop(1);

    signature = signature.toPercentEncoding();

    //---------------------------------------------------------------------------------------------

    QByteArray baseUrl = WControllerNetwork::extractBaseUrl(url.toString()).toLatin1();

    signature.prepend(httpMethod.toUpper() + '&' + baseUrl.toPercentEncoding() + '&');

    QByteArray fullKey = secret.toPercentEncoding() + '&' + token.toPercentEncoding();

    signature = sk->generateHmacSha1(signature, fullKey);

    //---------------------------------------------------------------------------------------------

#ifdef QT_4
    source.addEncodedQueryItem("oauth_signature", signature.toPercentEncoding());
#else
    query.addQueryItem("oauth_signature", signature.toPercentEncoding());

    source.setQuery(query);
#endif

    return source;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::htmlToUtf8(const QString & string)
{
    QString result;

    for (int i = 0; i < string.length(); i++)
    {
        QChar character = string.at(i);

        if (character == '<')
        {
            while (i < string.length() && string.at(i) != '>')
            {
                i++;
            }
        }
        else if (character == '&')
        {
             result.append(extractEntityAt(string, &i));
        }
        else result.append(character);
    }

    return Sk::unicodeToUtf8(result);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString & WControllerNetwork::fixHtml(QString & text)
{
    return text.replace("\\/", "/");
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractHtml(const QString & text)
{
    QString result = text;

    return fixHtml(result);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::extractHead(const QString & html)
{
    QString head = Sk::sliceIn(html, "<head>", "</head");

    if (head.isEmpty())
    {
         return Sk::sliceIn(html, "<html", "</head");
    }
    else return head;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::extractCharset(const QString & html)
{
    QString head = extractHead(html);

    QStringList tags = Sk::slicesIn(head, "<meta", ">");

    foreach (QString tag, tags)
    {
        fixAttributes(tag);

        int index = indexAttribute(tag, "charset");

        if (index == -1)
        {
            index = indexAttribute(tag, "http-equiv");

            if (index == -1) continue;

            QString http = extractAttributeAt(tag, index);

            if (http.toLower() == "content-type")
            {
                QString charset = extractAttribute(tag, "content");

                return charset.mid(charset.indexOf("charset=") + 8);
            }
        }
        else return extractAttributeAt(tag, index);
    }

    return QString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::extractTitle(const QString & head)
{
    QString title = extractValueUtf8(head, "title");

    if (title.isEmpty())
    {
         return QString();
    }
    else return title.simplified();
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractImage(const QString & head)
{
    QString cover = extractNodeAttribute(head, "=\"og:image\"", "content");

    if (cover.isEmpty())
    {
         return extractNodeAttribute(head, "=\"image_src", "href");
    }
    else return cover;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::extractEntityAt(const QString & text,
                                                                     int             at)
{
    return extractEntityAt(text, &at);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractEntityAt(const QString & text,
                                                                     int           * index)
{
    QString entity;

    int & at = *index;

    int from = at;

    at++;

    while (at < text.length())
    {
        QChar character = text.at(at);

        if (character.isSpace() || at - from > 8)
        {
            at = from;

            return QString('&');
        }
        else if (character == ';') break;

        entity.append(character);

        at++;
    }

    const WControllerNetworkEntity * start = &(CONTROLLERNETWORK_ENTITIES[0]);
    const WControllerNetworkEntity * end   = &(CONTROLLERNETWORK_ENTITIES[258]);

    const WControllerNetworkEntity * resolved = qBinaryFind(start, end, entity);

    if (resolved != end)
    {
        return QString(resolved->code);
    }

    if (entity.length() > 1 && entity.at(0) == '#')
    {
        entity.remove(0, 1);

        int base;

        if (entity.at(0).toLower() == 'x')
        {
            entity.remove(0, 1);

            base = 16;
        }
        else base = 10;

        bool ok;

        uint number = entity.toUInt(&ok, base);

        if (ok)
        {
            if (number >= 0x80 && number < (0x80 + sizeof(CONTROLLERNETWORK_EXTENDED)
                                                   /
                                                   sizeof(CONTROLLERNETWORK_EXTENDED[0])))
            {
                number = CONTROLLERNETWORK_EXTENDED[number - 0x80];
            }

            if (QChar::requiresSurrogates(number))
            {
                 return QString(QChar::highSurrogate(number) + QChar::lowSurrogate(number));
            }
            else return QString(number);
        }
    }

    at = from;

    return QString('&');
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ int WControllerNetwork::indexNode(const QString & text,
                                                           const QString & pattern, int from)
{
    int indexA = text.indexOf(pattern, from);

    if (indexA == -1) return -1;

    int indexB = text.indexOf('"', from);

    while (indexB > 0 && Sk::checkEscaped(text, indexB))
    {
        indexB = text.indexOf('"', indexB + 1);
    }

    if (indexB != -1 && indexB < indexA)
    {
        return indexNode(text, pattern, indexB + 1);
    }
    else return indexA + pattern.length();
}

/* Q_INVOKABLE static */ int WControllerNetwork::indexNodeBegin(const QString & text, int at)
{
    if (at < 0 || at >= text.length())
    {
        return -1;
    }

    while (at)
    {
        QChar character = text.at(at);

        if (character == '"')
        {
            int index = at - 1;

            if (index != -1)
            {
                character = text.at(index);

                if (character != '=' && character != '\\')
                {
                    return indexNodeBegin(text, index - 1);
                }
            }
        }
        else if (character == '<') return at;

        at--;
    }

    return -1;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::extractNodeAt(const QString & text,
                                                                   const QString & pattern, int at)
{
    int index = text.indexOf(pattern, at);

    if (index == -1)
    {
         return QString();
    }
    else return text.mid(at, index - at);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */
QString WControllerNetwork::extractNodeAttribute(const QString & text, const QString & pattern,
                                                                       const QString & attribute)
{
    int at = text.indexOf(pattern);

    if (at == -1)
    {
         return QString();
    }
    else return extractNodeAttributeAt(text, attribute, at);
}

/* Q_INVOKABLE static */
QString WControllerNetwork::extractNodeAttributeAt(const QString & text,
                                                   const QString & attribute, int at)
{
    at = indexNodeBegin(text, at);

    if (at == -1)
    {
         return QString();
    }
    else return extractAttribute(text, attribute, at);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ int WControllerNetwork::indexValue(const QString & text, int from)
{
    return indexNode(text, ">", from);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractValue(const QString & text, int from)
{
    from = indexValue(text, from);

    if (from == -1)
    {
         return QString();
    }
    else return extractValueAt(text, from);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractValue(const QString & text,
                                                                  const QString & name, int from)
{
    from = text.indexOf('<' + name, from);

    if (from == -1)
    {
         return QString();
    }
    else return extractValue(text, from + 1 + name.length());
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractValueAt(const QString & text, int at)
{
    return extractNodeAt(text, "</", at);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::extractValueUtf8(const QString & text,
                                                                      int             from)
{
    from = indexValue(text, from);

    if (from == -1)
    {
         return QString();
    }
    else return extractValueUtf8At(text, from);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractValueUtf8(const QString & text,
                                                                      const QString & name,
                                                                      int             from)
{
    from = text.indexOf('<' + name, from);

    if (from == -1)
    {
         return QString();
    }
    else return extractValueUtf8(text, from + 1 + name.length());
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractValueUtf8At(const QString & text,
                                                                        int             at)
{
    QString result = extractNodeAt(text, "</", at);

    return htmlToUtf8(result);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString & WControllerNetwork::fixAttributes(QString & text)
{
    text.replace("'", "\"");

    int index = text.indexOf('=');

    while (index != -1)
    {
        index++;

        while (index < text.length() && text.at(index).isSpace())
        {
            index++;
        }

        if (index != text.length() && text.at(index) == '"')
        {
            index = text.indexOf('"', index + 1);

            if (index == -1) return text;
        }
        else
        {
            text.insert(index, '"');

            index = text.indexOf(' ', index + 1);

            if (index == -1)
            {
                text.append('"');

                return text;
            }
            else text.insert(index, '"');
        }

        index = text.indexOf('=', index + 1);
    }

    return text;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ int WControllerNetwork::indexAttribute(const QString & text,
                                                                const QString & attribute,
                                                                int             from)
{
    return indexNode(text, attribute + "=\"", from);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractAttribute(const QString & text,
                                                                      const QString & attribute,
                                                                      int             from)
{
    from = indexAttribute(text, attribute, from);

    if (from == -1)
    {
         return QString();
    }
    else return extractAttributeAt(text, from);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractAttributeAt(const QString & text,
                                                                        int             at)
{
    return extractNodeAt(text, "\"", at);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */
QString WControllerNetwork::extractAttributeUtf8(const QString & text,
                                                 const QString & attribute, int from)
{
    from = indexAttribute(text, attribute, from);

    if (from == -1)
    {
         return QString();
    }
    else return extractAttributeUtf8At(text, from);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractAttributeUtf8At(const QString & text,
                                                                            int             at)
{
    QString result = extractNodeAt(text, "\"", at);

    return htmlToUtf8(result);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ int WControllerNetwork::indexJson(const QString & text,
                                                           const QString & attribute, int from)
{
    QString match = Sk::quote(attribute);

    int index = text.indexOf(match, from);

    if (index == -1) return -1;

    index = text.indexOf(':', index + match.length());

    if (index == -1) return -1;

    index++;

    while (index < text.length() && text.at(index) == ' ')
    {
        index++;
    }

    return index;
}

/* Q_INVOKABLE static */ int WControllerNetwork::indexJsonEnd(const QString & text, int at)
{
    if (at < 0 || at >= text.length())
    {
        return -1;
    }

    QChar charA = text.at(at);

    if (charA == '"')
    {
         return WControllerNetworkPrivate::indexJsonEndA(text, at);
    }
    else return WControllerNetworkPrivate::indexJsonEndB(text, at, charA);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::extractJson(const QString & text)
{
    return extractJsonAt(text, 0);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractJson(const QString & text,
                                                                 const QString & attribute,
                                                                 int             from)
{
    from = indexJson(text, attribute, from);

    if (from == -1)
    {
         return QString();
    }
    else return extractJsonAt(text, from);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractJsonAt(const QString & text, int at)
{
    int index = indexJsonEnd(text, at);

    if (index == -1) return QString();

    QChar character = text.at(at);

    if (character == '"')
    {
        QString result = text.mid(at + 1, index - at - 1);

        result.replace("\\\"", "\"");
        result.replace("\\\\", "\\");

        return result;
    }
    else if (character == '{' || character == '[')
    {
         return text.mid(at + 1, index - at - 1);
    }
    else return text.mid(at, index - at);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::extractJsonUtf8(const QString & text)
{
    return extractJsonUtf8At(text, 0);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractJsonUtf8(const QString & text,
                                                                     const QString & attribute,
                                                                     int             from)
{
    from = indexJson(text, attribute, from);

    if (from == -1)
    {
         return QString();
    }
    else return extractJsonUtf8At(text, from);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractJsonUtf8At(const QString & text,
                                                                       int             at)
{
    QString result = extractJsonAt(text, at);

    return htmlToUtf8(result);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::extractJsonHtml(const QString & text)
{
    return extractJsonHtmlAt(text, 0);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractJsonHtml(const QString & text,
                                                                     const QString & attribute,
                                                                     int             from)
{
    from = indexJson(text, attribute, from);

    if (from == -1)
    {
         return QString();
    }
    else return extractJsonHtmlAt(text, from);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractJsonHtmlAt(const QString & text,
                                                                       int             at)
{
    QString result = extractJsonAt(text, at);

    return fixHtml(result);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QStringList WControllerNetwork::splitJson(const QString & text, int from)
{
    QStringList list;

    QChar character;

    QRegExp regExp("[{\"]");

    from = text.indexOf(regExp, from);

    if (from == -1) return list;

    if (regExp.capturedTexts().first() == "{")
    {
         character = '{';
    }
    else character = '"';

    while (1)
    {
        int index = indexJsonEnd(text, from);

        if (index == -1) return list;

        QString result = text.mid(from + 1, index - from - 1);

        list.append(result);

        from = text.indexOf(character, index + 1);

        if (from == -1) return list;
    }

    return list;
}

/* Q_INVOKABLE static */ QString WControllerNetwork::stripJson(const QString & text,
                                                               const QString & attribute, int from)
{
    QString match = Sk::quote(attribute);

    from = text.indexOf(match, from);

    if (from == -1)
    {
        return QString();
    }

    int start = text.indexOf(':', from + match.length());

    if (start == -1)
    {
        return QString();
    }

    start++;

    while (start < text.length() && text.at(start) == ' ')
    {
        start++;
    }

    int end = indexJsonEnd(text, start);

    if (end == -1)
    {
        return QString();
    }

    end++;

    if (text.at(end) == ',') end++;

    return text.mid(0, from) + text.mid(end);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerNetwork::extractScript(const QString & text,
                                                                   const QString & name,
                                                                   int             from)
{
    int at = text.indexOf(name, from);

    if (at == -1) return QString();

    int index = indexJsonEnd(text, text.indexOf('{', at));

    if (index == -1)
    {
         return QString();
    }
    else return text.mid(at, index - at + 1);
}

/* Q_INVOKABLE static */ QString WControllerNetwork::extractScriptHtml(const QString & text,
                                                                       const QString & name,
                                                                       int             from)
{
    QString result = extractScript(text, name, from);

    return fixHtml(result);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WControllerNetwork::isConnected() const
{
    Q_D(const WControllerNetwork); return d->connected;
}

//-------------------------------------------------------------------------------------------------

bool WControllerNetwork::isCheckingConnected() const
{
    Q_D(const WControllerNetwork); return d->checkConnected;
}

void WControllerNetwork::setCheckConnected(bool check)
{
    Q_D(WControllerNetwork);

    if (d->checkConnected == check) return;

    d->checkConnected = check;

    if (d->checkConnected == false)
    {
        d->socketTimer     .stop();
        d->socketRetryTimer.stop();

        d->socket.disconnect();

        d->socket.disconnectFromHost();
    }
    else
    {
        d->checkConnection();

        connect(&d->socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                this,       SLOT(onStateChanged(QAbstractSocket::SocketState)));
    }

    emit checkConnectedChanged(d->checkConnected);
}

#endif // SK_NO_CONTROLLERNETWORK
