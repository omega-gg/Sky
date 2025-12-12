//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkBackend.

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

#include "WBackendUniversal.h"

#ifndef SK_NO_BACKENDUNIVERSAL

// Qt includes
#include <QCoreApplication>
#include <QBuffer>
#include <QThread>
#ifdef QT_4
#include <QScriptEngine>
#else
#include <QUrlQuery>
#include <QJSEngine>
#endif

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>
#include <WControllerNetwork>
#include <WControllerDownload>
#include <WControllerPlaylist>
#include <WRegExp>
#include <WCache>
#include <WBackendCache>
#include <WYamlReader>
#include <WUnzipper>

// Forward declarations
class WBackendUniversalEngine;

//-------------------------------------------------------------------------------------------------
// Static variables

static const QString BACKENDUNIVERSAL_FUNCTIONS = \
    "^(IF|"
    "AND|"
    "OR|"
    "ELSE|"
    "ELIF|"
    "FI|"
    "FOREACH|"
    "CONTINUE|"
    "BREAK|"
    "END|"
    "RETURN|"
    "EQUAL|"
    "NOT_EQUAL|"
    "LESSER|"
    "GREATER|"
    "LESSER_EQUAL|"
    "GREATER_EQUAL|"
    "SET|"
    "NUMBER|"
    "TIME|"
    "DATE|"
    "ADD|"
    "SUB|"
    "MULTIPLY|"
    "DIVIDE|"
    "GET_CHAR|"
    "PREPEND|"
    "APPEND|"
    "REMOVE|"
    "CHOP|"
    "REPLACE|"
    "MID|"
    "SPLIT|"
    "JOIN|"
    "LOWER|"
    "SIMPLIFY|"
    "READ|"
    "LATIN_TO_UTF8|"
    "LENGTH|"
    "COUNT|"
    "COUNT_STRING|"
    "INDEX_OF|"
    "INDEX_END|"
    "INDEX_SKIP|"
    "LAST_INDEX_OF|"
    "LAST_INDEX_END|"
    "CONTAIN|"
    "START_WITH|"
    "END_WITH|"
    "SLICE|"
    "SLICE_IN|"
    "SLICES|"
    "SLICES_IN|"
    "LIST|"
    "LIST_GET|"
    "LIST_SET|"
    "LIST_TAKE_AT|"
    "LIST_APPEND|"
    "LIST_INSERT|"
    "LIST_REMOVE|"
    "LIST_REMOVE_AT|"
    "LIST_INDEX|"
    "LIST_CONTAIN|"
    "HASH_GET|"
    "HASH_SET|"
    "HASH_REMOVE|"
    "REGEXP|"
    "REGEXP_CAP|"
    "EXTENSION_IS_MEDIA|"
    "EXTENSION_IS_VIDEO|"
    "EXTENSION_IS_AUDIO|"
    "EXTENSION_IS_SUBTITLE|"
    "URL_GENERATE|"
    "URL_ENCODE|"
    "URL_DECODE|"
    "URL_NAME|"
    "URL_VALUE|"
    "URL_FRAGMENT|"
    "URL_PATH|"
    "URL_ELEMENT|"
    "URL_ELEMENTS|"
    "URL_EXTENSION|"
    "URL_ADD_QUERY|"
    "URL_REMOVE_PREFIX|"
    "URL_REMOVE_EXTENSION|"
    "HTML_TO_UTF8|"
    "HTML_EXTRACT|"
    "HTML_TRACKS|"
    "HTML_PLAYLISTS|"
    "HTML_ATTRIBUTE|"
    "HTML_ATTRIBUTE_AT|"
    "HTML_ATTRIBUTE_UTF8|"
    "HTML_ATTRIBUTE_UTF8_AT|"
    "JSON_EXTRACT|"
    "JSON_EXTRACT_UTF8|"
    "JSON_EXTRACT_HTML|"
    "JSON_SPLIT|"
    "JS_EXTRACT|"
    "JS_CALL|"
    "JS_CALLS|"
    "ZIP_FILENAMES|"
    "ZIP_EXTRACT_FILE|"
#ifndef SK_NO_TORRENT
    "TORRENT_STRING_AFTER|"
    "TORRENT_INTEGER_AFTER|"
    "TORRENT_LIST_AFTER|"
    "TORRENT_ITEMS|"
    "TORRENT_FOLDERS|"
#endif
    "PRINT)$";

static const int BACKENDUNIVERSAL_MAX_NODES = 300;
static const int BACKENDUNIVERSAL_MAX_LOOP  = 1000;

static const int BACKENDUNIVERSAL_MAX_QUERIES = 50;

static const int BACKENDUNIVERSAL_DEPTH_IF   = 1;
static const int BACKENDUNIVERSAL_DEPTH_LOOP = 2;

static const int BACKENDUNIVERSAL_TIMEOUT_LOAD   = 60000; // 1 minute
static const int BACKENDUNIVERSAL_TIMEOUT_SCRIPT =  5000; // 5 seconds

//-------------------------------------------------------------------------------------------------

typedef QVariant (*function)(const WBackendUniversalNode *, WBackendUniversalParameters *);

static QHash<QString, function> hash;

static WBackendCache * cache = NULL;

static WBackendUniversalEngine * backendEngine = NULL;

//=================================================================================================
// WBackendUniversalEngine
//=================================================================================================

class SK_BACKEND_EXPORT WBackendUniversalEngine : public QObject
{
    Q_OBJECT

public:
    WBackendUniversalEngine();

    /* virtual */ ~WBackendUniversalEngine();

public: // Static functions
    Q_INVOKABLE static QVariantList call(const QString & script, const QVariantList & arguments);

private: // Slots
    Q_INVOKABLE void onCall(const QString & script, const QVariantList & arguments);

public: // Variables
    QThread thread;

    QMetaMethod method;

    QVariantList variants;

    bool loaded;
};

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WBackendUniversalEngine::WBackendUniversalEngine()
{
    const QMetaObject * meta = metaObject();

    method = meta->method(meta->indexOfMethod("onCall(QString,QVariantList)"));

    loaded = false;

    moveToThread(&thread);

    thread.start();
}

/* virtual */ WBackendUniversalEngine::~WBackendUniversalEngine()
{
    thread.quit();
    thread.wait();
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QVariantList WBackendUniversalEngine::call(const QString      & script,
                                                                    const QVariantList & arguments)
{
    if (backendEngine)
    {
        backendEngine->loaded = false;
    }
    else backendEngine = new WBackendUniversalEngine;

    backendEngine->method.invoke(backendEngine, Q_ARG(const QString      &, script),
                                                Q_ARG(const QVariantList &, arguments));

    QTime time = QTime::currentTime().addMSecs(BACKENDUNIVERSAL_TIMEOUT_SCRIPT);

    while (time > QTime::currentTime() && backendEngine->loaded == false)
    {
        QCoreApplication::processEvents();
    }

    QVariantList results;

    // NOTE: If loading takes too long we terminate the thread.
    if (backendEngine->loaded == false)
    {
        backendEngine->thread.terminate();

        delete backendEngine;

        backendEngine = NULL;

        return results;
    }

    results = backendEngine->variants;

    backendEngine->variants.clear();

    return results;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WBackendUniversalEngine::onCall(const QString      & script,
                                                       const QVariantList & arguments)
{
#ifdef QT_4
    QScriptEngine engine;

    QScriptValue value = engine.evaluate(script);
#else
    QJSEngine engine;

    QJSValue value = engine.evaluate(script);
#endif

    variants.clear();

    if (arguments.isEmpty() == false)
    {
        foreach (const QVariant & variant, arguments)
        {
#ifdef QT_4
            QScriptValueList values;
#else
            QJSValueList values;
#endif

            foreach (const QString & string, variant.toStringList())
            {
                values.append(string);
            }

#ifdef QT_4
            variants.append(value.call(QScriptValue(), values).toVariant());
#else
            variants.append(value.call(values).toVariant());
#endif
        }
    }
    else variants.append(value.call().toVariant());

    loaded = true;
}

//=================================================================================================
// Static functions
//=================================================================================================

inline QVariant equal(const WBackendUniversalNode * node,
                      WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("EQUAL");
#endif

    if (node->nodes.count() < 2) return false;

    QString stringA = node->getString(parameters, 0);
    QString stringB = node->getString(parameters, 1);

    return (stringA == stringB);
}

inline QVariant notEqual(const WBackendUniversalNode * node,
                         WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("NOT_EQUAL");
#endif

    if (node->nodes.count() < 2) return false;

    QString stringA = node->getString(parameters, 0);
    QString stringB = node->getString(parameters, 1);

    return (stringA != stringB);
}

//-------------------------------------------------------------------------------------------------

inline QVariant lesser(const WBackendUniversalNode * node,
                       WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("LESSER");
#endif

    if (node->nodes.count() < 2) return false;

    qreal valueA = node->getReal(parameters, 0);
    qreal valueB = node->getReal(parameters, 1);

    return (valueA < valueB);
}

inline QVariant greater(const WBackendUniversalNode * node,
                        WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("GREATER");
#endif

    if (node->nodes.count() < 2) return false;

    qreal valueA = node->getReal(parameters, 0);
    qreal valueB = node->getReal(parameters, 1);

    return (valueA > valueB);
}

//-------------------------------------------------------------------------------------------------

inline QVariant lesserEqual(const WBackendUniversalNode * node,
                            WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("LESSER_EQUAL");
#endif

    if (node->nodes.count() < 2) return false;

    qreal valueA = node->getReal(parameters, 0);
    qreal valueB = node->getReal(parameters, 1);

    return (valueA <= valueB);
}

inline QVariant greaterEqual(const WBackendUniversalNode * node,
                             WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("GREATER_EQUAL");
#endif

    if (node->nodes.count() < 2) return false;

    qreal valueA = node->getReal(parameters, 0);
    qreal valueB = node->getReal(parameters, 1);

    return (valueA >= valueB);
}

//-------------------------------------------------------------------------------------------------

inline QVariant set(const WBackendUniversalNode * node,
                    WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("SET");
#endif

    int count = node->nodes.count();

    if (count < 2) return false;

    QVariant * key = node->getKey(parameters, 0);

    if (key == NULL) return false;

    if (count != 2)
    {
        QVariantList list;

        for (int i = 1; i < count; i++)
        {
            list.append(node->getVariant(parameters, i));
        }

        *key = list;
    }
    else *key = node->getVariant(parameters, 1);

    return true;
}

//-------------------------------------------------------------------------------------------------

inline QVariant number(const WBackendUniversalNode * node,
                       WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("NUMBER");
#endif

    if (node->nodes.count() < 1) return 0;

    return node->getReal(parameters, 0);
}

inline QVariant time(const WBackendUniversalNode * node,
                     WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("TIME");
#endif

    int count = node->nodes.count();

    if (count == 0)
    {
        return Sk::getMsecs(QTime::currentTime());
    }

    if (count < 2) return QTime();

    QTime time = QTime::fromString(node->getString(parameters, 0),
                                   node->getString(parameters, 1));

    return Sk::getMsecs(time);
}

inline QVariant date(const WBackendUniversalNode * node,
                     WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("DATE");
#endif

    int count = node->nodes.count();

    if (count == 0)
    {
        return Sk::dateToSecs(QDateTime::currentDateTime());
    }

    if (count < 2) return QDateTime();

    QDateTime date = QDateTime::fromString(node->getString(parameters, 0),
                                           node->getString(parameters, 1));

    if (date.isValid() == false)
    {
        return QDateTime();
    }

    return Sk::dateToSecs(date);
}

//-------------------------------------------------------------------------------------------------

inline QVariant add(const WBackendUniversalNode * node,
                    WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("ADD");
#endif

    if (node->nodes.count() < 2) return false;

    qreal valueA = node->getReal(parameters, 0);
    qreal valueB = node->getReal(parameters, 1);

    return (valueA + valueB);
}

inline QVariant sub(const WBackendUniversalNode * node,
                    WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("SUB");
#endif

    if (node->nodes.count() < 2) return false;

    qreal valueA = node->getReal(parameters, 0);
    qreal valueB = node->getReal(parameters, 1);

    return (valueA - valueB);
}

inline QVariant multiply(const WBackendUniversalNode * node,
                         WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("MULTIPLY");
#endif

    if (node->nodes.count() < 2) return false;

    qreal valueA = node->getReal(parameters, 0);
    qreal valueB = node->getReal(parameters, 1);

    return (valueA * valueB);
}

inline QVariant divide(const WBackendUniversalNode * node,
                       WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("DIVIDE");
#endif

    if (node->nodes.count() < 2) return false;

    qreal valueA = node->getReal(parameters, 0);
    qreal valueB = node->getReal(parameters, 1);

    return (valueA / valueB);
}

//-------------------------------------------------------------------------------------------------

inline QVariant getChar(const WBackendUniversalNode * node,
                        WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("GET_CHAR");
#endif

    if (node->nodes.count() < 2) return QChar();

    QString string = node->getString(parameters, 0);

    int index = node->getInt(parameters, 1);

    if (index < 0 || index >= string.length())
    {
         return QChar();
    }
    else return string.at(index);
}

//-------------------------------------------------------------------------------------------------

inline QVariant prepend(const WBackendUniversalNode * node,
                        WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("PREPEND");
#endif

    int count = node->nodes.count();

    if (count < 2) return false;

    QString string = node->getString(parameters, 0);

    for (int i = 1; i < count; i++)
    {
        string.prepend(node->getString(parameters, i));
    }

    return string;
}

inline QVariant append(const WBackendUniversalNode * node,
                       WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("APPEND");
#endif

    int count = node->nodes.count();

    if (count < 2) return false;

    QString string = node->getString(parameters, 0);

    for (int i = 1; i < count; i++)
    {
        string.append(node->getString(parameters, i));
    }

    return string;
}

//-------------------------------------------------------------------------------------------------

inline QVariant remove(const WBackendUniversalNode * node,
                       WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("REMOVE");
#endif

    int count = node->nodes.count();

    if (count < 2) return QString();

    QString string = node->getString(parameters, 0);

    if (count == 2)
    {
         return string.remove(node->getString(parameters, 1));
    }
    else return string.remove(node->getInt(parameters, 1), node->getInt(parameters, 2));
}

//-------------------------------------------------------------------------------------------------

inline QVariant chop(const WBackendUniversalNode * node, WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("CHOP");
#endif

    if (node->nodes.count() < 2) return QString();

    QString string = node->getString(parameters, 0);

    string.chop(node->getInt(parameters, 1));

    return string;
}

//-------------------------------------------------------------------------------------------------

inline QVariant replace(const WBackendUniversalNode * node,
                        WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("REPLACE");
#endif

    if (node->nodes.count() < 3) return QString();

    QVariant variant = node->getVariant(parameters, 1);

    if (WRegExp::isRegExp(variant))
    {
         return node->getString(parameters, 0).replace(WRegExp::fromVariant(variant),
                                                       node->getString(parameters, 2));
    }
    else return node->getString(parameters, 0).replace(variant.toString(),
                                                       node->getString(parameters, 2));
}

//-------------------------------------------------------------------------------------------------

inline QVariant mid(const WBackendUniversalNode * node,
                    WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("MID");
#endif

    int count = node->nodes.count();

    if (count < 2) return QString();

    QString string = node->getString(parameters, 0);

    if (count == 2)
    {
         return string.mid(node->getInt(parameters, 1));
    }
    else return string.mid(node->getInt(parameters, 1), node->getInt(parameters, 2));
}

//-------------------------------------------------------------------------------------------------

inline QVariant split(const WBackendUniversalNode * node, WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("SPLIT");
#endif

    int count = node->nodes.count();

    if (count < 2) return QString();

    QString string = node->getString(parameters, 0);

    return string.split(node->getString(parameters, 1));
}

inline QVariant join(const WBackendUniversalNode * node, WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("JOIN");
#endif

    int count = node->nodes.count();

    if (count < 2) return QString();

    QStringList list = node->getStringList(parameters, 0);

    return list.join(node->getString(parameters, 1));
}

//-------------------------------------------------------------------------------------------------

inline QVariant lower(const WBackendUniversalNode * node,
                      WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("LOWER");
#endif

    if (node->nodes.count() < 1) return QString();

    return node->getString(parameters, 0).toLower();
}

inline QVariant simplify(const WBackendUniversalNode * node,
                         WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("SIMPLIFY");
#endif

    if (node->nodes.count() < 1) return QString();

    return node->getString(parameters, 0).simplified();
}

//-------------------------------------------------------------------------------------------------

inline QVariant read(const WBackendUniversalNode * node,
                     WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("READ");
#endif

    if (node->nodes.count() < 2) return QString();

    QString codec = node->getString(parameters, 1).toLower();

    if (codec == "ascii")
    {
         return Sk::readAscii(node->getByteArray(parameters, 0));
    }
    else return Sk::readCodec(node->getByteArray(parameters, 0), codec);
}

//-------------------------------------------------------------------------------------------------

inline QVariant latinToUtf8(const WBackendUniversalNode * node,
                            WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("LATIN_TO_UTF8");
#endif

    if (node->nodes.count() < 1) return QString();

    return Sk::latinToUtf8(node->getString(parameters, 0));
}

//-------------------------------------------------------------------------------------------------

inline QVariant length(const WBackendUniversalNode * node,
                       WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("LENGTH");
#endif

    if (node->nodes.count() < 1) return 0;

    return node->getString(parameters, 0).length();
}

inline QVariant count(const WBackendUniversalNode * node,
                      WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("COUNT");
#endif

    if (node->nodes.count() < 1) return 0;

    QVariant variant = node->getVariant(parameters, 0);

#ifdef QT_OLD
    if (variant.type() == QVariant::Hash)
#else
    if (variant.typeId() == QMetaType::QVariantHash)
#endif
    {
         return variant.toHash().count();
    }
    else return variant.toList().count();
}

inline QVariant countString(const WBackendUniversalNode * node,
                            WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("COUNT_STRING");
#endif

    if (node->nodes.count() < 2) return 0;

    return node->getString(parameters, 0).count(node->getString(parameters, 1));
}

//-------------------------------------------------------------------------------------------------

inline QVariant indexOf(const WBackendUniversalNode * node,
                        WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("INDEX_OF");
#endif

    int count = node->nodes.count();

    if (count < 2) return -1;

    QString string = node->getString(parameters, 0);

    QVariant variant = node->getVariant(parameters, 1);

    if (WRegExp::isRegExp(variant))
    {
        if (count == 2)
        {
             return string.indexOf(WRegExp::fromVariant(variant));
        }
        else return string.indexOf(WRegExp::fromVariant(variant), node->getInt(parameters, 2));
    }
    else if (count == 2)
    {
         return string.indexOf(variant.toString());
    }
    else return string.indexOf(variant.toString(), node->getInt(parameters, 2));
}

//-------------------------------------------------------------------------------------------------

inline QVariant indexEnd(const WBackendUniversalNode * node,
                         WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("INDEX_END");
#endif

    int count = node->nodes.count();

    if (count < 2) return -1;

    QString stringA = node->getString(parameters, 0);

    QVariant variant = node->getVariant(parameters, 1);

    int index;

    if (WRegExp::isRegExp(variant))
    {
        WRegExp regExp = WRegExp::fromVariant(variant);

        if (count == 2)
        {
             index = regExp.indexIn(stringA);
        }
        else index = regExp.indexIn(stringA, node->getInt(parameters, 2));

        if (index == -1)
        {
             return -1;
        }
        else return index + regExp.matchedLength();
    }
    else
    {
        QString stringB = variant.toString();

        if (count == 2)
        {
             index = stringA.indexOf(stringB);
        }
        else index = stringA.indexOf(stringB, node->getInt(parameters, 2));

        if (index == -1)
        {
             return -1;
        }
        else return index + stringB.length();
    }
}

//-------------------------------------------------------------------------------------------------

inline QVariant indexSkip(const WBackendUniversalNode * node,
                          WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("INDEX_SKIP");
#endif

    int count = node->nodes.count();

    if (count < 3) return -1;

    return Sk::indexSkip(node->getString(parameters, 0),
                         node->getString(parameters, 1), node->getInt(parameters, 2));
}

//-------------------------------------------------------------------------------------------------

inline QVariant lastIndexOf(const WBackendUniversalNode * node,
                            WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("LAST_INDEX_OF");
#endif

    int count = node->nodes.count();

    if (count < 2) return -1;

    QString string = node->getString(parameters, 0);

    QVariant variant = node->getVariant(parameters, 1);

    if (WRegExp::isRegExp(variant))
    {
        if (count == 2)
        {
             return string.lastIndexOf(WRegExp::fromVariant(variant));
        }
        else return string.lastIndexOf(WRegExp::fromVariant(variant), node->getInt(parameters, 2));
    }
    else if (count == 2)
    {
         return string.lastIndexOf(variant.toString());
    }
    else return string.lastIndexOf(variant.toString(), node->getInt(parameters, 2));
}

//-------------------------------------------------------------------------------------------------

inline QVariant lastIndexEnd(const WBackendUniversalNode * node,
                             WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("LAST_INDEX_END");
#endif

    int count = node->nodes.count();

    if (count < 2) return -1;

    QString stringA = node->getString(parameters, 0);

    QVariant variant = node->getVariant(parameters, 1);

    int index;

    if (WRegExp::isRegExp(variant))
    {
        WRegExp regExp = WRegExp::fromVariant(variant);

        int index;

        if (count == 2)
        {
             index = regExp.lastIndexIn(stringA);
        }
        else index = regExp.lastIndexIn(stringA, node->getInt(parameters, 2));

        if (index == -1)
        {
             return -1;
        }
        else return index + regExp.matchedLength();
    }
    else
    {
        QString stringB = variant.toString();

        if (count == 2)
        {
             index = stringA.lastIndexOf(stringB);
        }
        else index = stringA.lastIndexOf(stringB, node->getInt(parameters, 2));

        if (index == -1)
        {
             return -1;
        }
        else return index + stringB.length();
    }
}

//-------------------------------------------------------------------------------------------------

inline QVariant contain(const WBackendUniversalNode * node,
                        WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("CONTAIN");
#endif

    if (node->nodes.count() < 2) return false;

    QString string = node->getString(parameters, 0);

    QVariant variant = node->getVariant(parameters, 1);

    if (WRegExp::isRegExp(variant))
    {
#ifdef QT_4
        // FIXME Qt4: The code does not compile without the 'QVariant'.
        return QVariant(string.contains(WRegExp::fromVariant(variant)));
#else
        return string.contains(WRegExp::fromVariant(variant));
#endif
    }
    else
    {
#ifdef QT_4
        // FIXME Qt4: The code does not compile without the 'QVariant'.
        return QVariant(string.contains(variant.toString()));
#else
        return string.contains(variant.toString());
#endif
    }
}

//-------------------------------------------------------------------------------------------------

inline QVariant startWith(const WBackendUniversalNode * node,
                          WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("START_WITH");
#endif

    if (node->nodes.count() < 2) return false;

    QString string = node->getString(parameters, 0);

    QVariant variant = node->getVariant(parameters, 1);

    if (WRegExp::isRegExp(variant))
    {
        if (string.indexOf(WRegExp::fromVariant(variant)) == 0)
        {
             return true;
        }
        else return false;
    }
    else return string.startsWith(variant.toString());
}

//-------------------------------------------------------------------------------------------------

inline QVariant endWith(const WBackendUniversalNode * node,
                        WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("END_WITH");
#endif

    if (node->nodes.count() < 2) return false;

    QString string = node->getString(parameters, 0);

    QVariant variant = node->getVariant(parameters, 1);

    if (WRegExp::isRegExp(variant))
    {
        WRegExp regExp = WRegExp::fromVariant(variant);

        int index = regExp.indexIn(string);

        if (string.length() == (index + regExp.matchedLength()))
        {
             return true;
        }
        else return false;
    }
    else return string.endsWith(variant.toString());
}

//-------------------------------------------------------------------------------------------------

inline QVariant slice(const WBackendUniversalNode * node,
                      WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("SLICE");
#endif

    int count = node->nodes.count();

    if (count < 3) return QString();

    QVariantList list;

    list.append(node->getVariant(parameters, 1));
    list.append(node->getVariant(parameters, 2));

    QList<WRegExp> regExps = node->getRegExps(list);

    if (regExps.isEmpty())
    {
        if (count == 3)
        {
             return Sk::slice(node->getString(parameters, 0), list.at(0).toString(),
                                                              list.at(1).toString());
        }
        else return Sk::slice(node->getString(parameters, 0), list.at(0).toString(),
                                                              list.at(1).toString(),
                                                              node->getInt(parameters, 3));
    }
    else if (count == 3)
    {
         return Sk::slice(node->getString(parameters, 0), regExps.at(0), regExps.at(1));
    }
    else return Sk::slice(node->getString(parameters, 0), regExps.at(0), regExps.at(1),
                          node->getInt(parameters, 3));
}

//-------------------------------------------------------------------------------------------------

inline QVariant sliceIn(const WBackendUniversalNode * node,
                        WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("SLICE_IN");
#endif

    int count = node->nodes.count();

    if (count < 3) return QString();

    QVariantList list;

    list.append(node->getVariant(parameters, 1));
    list.append(node->getVariant(parameters, 2));

    QList<WRegExp> regExps = node->getRegExps(list);

    if (regExps.isEmpty())
    {
        if (count == 3)
        {
             return Sk::sliceIn(node->getString(parameters, 0), list.at(0).toString(),
                                                                list.at(1).toString());
        }
        else return Sk::sliceIn(node->getString(parameters, 0), list.at(0).toString(),
                                                                list.at(1).toString(),
                                                                node->getInt(parameters, 3));
    }
    else if (count == 3)
    {
         return Sk::sliceIn(node->getString(parameters, 0), regExps.at(0), regExps.at(1));
    }
    else return Sk::sliceIn(node->getString(parameters, 0), regExps.at(0), regExps.at(1),
                            node->getInt(parameters, 3));
}

//-------------------------------------------------------------------------------------------------

inline QVariant slices(const WBackendUniversalNode * node,
                       WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("SLICES");
#endif

    int count = node->nodes.count();

    if (count < 3) return QVariant();

    QVariantList list;

    list.append(node->getVariant(parameters, 1));
    list.append(node->getVariant(parameters, 2));

    QList<WRegExp> regExps = node->getRegExps(list);

    if (regExps.isEmpty())
    {
        if (count == 3)
        {
             return node->variants(Sk::slices(node->getString(parameters, 0),
                                              node->getString(parameters, 1),
                                              node->getString(parameters, 2)));
        }
        else return node->variants(Sk::slices(node->getString(parameters, 0),
                                              node->getString(parameters, 1),
                                              node->getString(parameters, 2),
                                              node->getInt   (parameters, 3)));
    }
    else if (count == 3)
    {
         return node->variants(Sk::slices(node->getString(parameters, 0), regExps.at(0),
                                          regExps.at(1)));
    }
    else return node->variants(Sk::slices(node->getString(parameters, 0), regExps.at(0),
                                          regExps.at(1), node->getInt(parameters, 3)));
}

inline QVariant slicesIn(const WBackendUniversalNode * node,
                         WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("SLICES_IN");
#endif

    int count = node->nodes.count();

    if (count < 3) return QVariant();

    QVariantList list;

    list.append(node->getVariant(parameters, 1));
    list.append(node->getVariant(parameters, 2));

    QList<WRegExp> regExps = node->getRegExps(list);

    if (regExps.isEmpty())
    {
        if (count == 3)
        {
             return node->variants(Sk::slicesIn(node->getString(parameters, 0),
                                                node->getString(parameters, 1),
                                                node->getString(parameters, 2)));
        }
        else return node->variants(Sk::slicesIn(node->getString(parameters, 0),
                                                node->getString(parameters, 1),
                                                node->getString(parameters, 2),
                                                node->getInt   (parameters, 3)));
    }
    else if (count == 3)
    {
         return Sk::slicesIn(node->getString(parameters, 0), regExps.at(0), regExps.at(1));
    }
    else return Sk::slicesIn(node->getString(parameters, 0), regExps.at(0), regExps.at(1),
                             node->getInt(parameters, 3));
}

//-------------------------------------------------------------------------------------------------
// List

inline QVariant list(const WBackendUniversalNode * node,
                     WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("LIST");
#endif

    QVariantList list;

    for (int i = 0; i < node->nodes.count(); i++)
    {
        list.append(node->getVariant(parameters, i));
    }

    return list;
}

//-------------------------------------------------------------------------------------------------

inline QVariant listGet(const WBackendUniversalNode * node,
                        WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("LIST_GET");
#endif

    if (node->nodes.count() < 2) return QVariant();

    QVariantList list = node->getList(parameters, 0);

    int index = node->getInt(parameters, 1);

    if (index < 0 || index >= list.count())
    {
         return QVariant();
    }
    else return list.at(index);
}

inline QVariant listSet(const WBackendUniversalNode * node,
                        WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("LIST_SET");
#endif

    int count = node->nodes.count();

    if (count < 3) return false;

    QVariant * key = node->getKey(parameters, 0);

    if (key == NULL) return false;

    QVariantList list = key->toList();

    if (count != 3)
    {
        QVariantList variants;

        for (int i = 2; i < count; i++)
        {
            variants.append(node->getVariant(parameters, i));
        }

        list.replace(node->getInt(parameters, 1), variants);
    }
    else list.replace(node->getInt(parameters, 1), node->getVariant(parameters, 2));

    *key = list;

    return true;
}

inline QVariant listTakeAt(const WBackendUniversalNode * node,
                           WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("LIST_TAKE_AT");
#endif

    if (node->nodes.count() < 2) return QVariant();

    QVariant * key = node->getKey(parameters, 0);

    if (key == NULL) return QVariant();

    QVariantList list = key->toList();

    int index = node->getInt(parameters, 1);

    if (index < 0 || index >= list.count())
    {
        return QVariant();
    }

    QVariant value = list.takeAt(index);

    *key = list;

    return value;
}

//-------------------------------------------------------------------------------------------------

inline QVariant listAppend(const WBackendUniversalNode * node,
                           WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("LIST_APPEND");
#endif

    if (node->nodes.count() < 2) return false;

    QVariant * key = node->getKey(parameters, 0);

    if (key == NULL) return false;

    QVariantList list = key->toList();

    list.append(node->getVariant(parameters, 1));

    *key = list;

    return true;
}

inline QVariant listInsert(const WBackendUniversalNode * node,
                           WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("LIST_INSERT");
#endif

    if (node->nodes.count() < 3) return false;

    QVariant * key = node->getKey(parameters, 0);

    if (key == NULL) return false;

    QVariantList list = key->toList();

    list.insert(node->getInt(parameters, 1), node->getVariant(parameters, 2));

    *key = list;

    return true;
}

inline QVariant listRemove(const WBackendUniversalNode * node,
                           WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("LIST_REMOVE");
#endif

    if (node->nodes.count() < 2) return false;

    QVariant * key = node->getKey(parameters, 0);

    if (key == NULL) return false;

    QVariantList list = key->toList();

    list.removeOne(node->getVariant(parameters, 1));

    *key = list;

    return true;
}

inline QVariant listRemoveAt(const WBackendUniversalNode * node,
                             WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("LIST_REMOVE_AT");
#endif

    if (node->nodes.count() < 2) return false;

    QVariant * key = node->getKey(parameters, 0);

    if (key == NULL) return false;

    QVariantList list = key->toList();

    list.removeAt(node->getInt(parameters, 1));

    *key = list;

    return true;
}

//-------------------------------------------------------------------------------------------------

inline QVariant listIndex(const WBackendUniversalNode * node,
                          WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("LIST_INDEX");
#endif

    int count = node->nodes.count();

    if (count < 2) return -1;

    QStringList list = node->getStringList(parameters, 0);

    if (count == 2)
    {
         return list.indexOf(node->getString(parameters, 1));
    }
    else return list.indexOf(node->getString(parameters, 1), node->getInt(parameters, 2));
}

inline QVariant listContain(const WBackendUniversalNode * node,
                            WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("LIST_CONTAIN");
#endif

    if (node->nodes.count() < 2) return false;

    QVariantList list = node->getList(parameters, 0);

#ifdef QT_4
    // FIXME Qt4: The code does not compile without the 'QVariant'.
    return QVariant(list.contains(node->getVariant(parameters, 1)));
#else
    return list.contains(node->getVariant(parameters, 1));
#endif
}

//-------------------------------------------------------------------------------------------------
// Hash

inline QVariant hashGet(const WBackendUniversalNode * node,
                        WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("HASH_GET");
#endif

    if (node->nodes.count() < 2) return QVariant();

    QHash<QString, QVariant> hash = node->getHash(parameters, 0);

    return hash.value(node->getString(parameters, 1));
}

inline QVariant hashSet(const WBackendUniversalNode * node,
                        WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("HASH_SET");
#endif

    int count = node->nodes.count();

    if (count < 3) return false;

    QVariant * key = node->getKey(parameters, 0);

    if (key == NULL) return false;

    QHash<QString, QVariant> hash = key->toHash();

    if (count != 3)
    {
        QVariantList list;

        for (int i = 2; i < count; i++)
        {
            list.append(node->getVariant(parameters, i));
        }

        hash.insert(node->getString(parameters, 1), list);
    }
    else hash.insert(node->getString(parameters, 1), node->getVariant(parameters, 2));

    *key = hash;

    return true;
}

inline QVariant hashRemove(const WBackendUniversalNode * node,
                           WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("HASH_REMOVE");
#endif

    if (node->nodes.count() < 2) return false;

    QVariant * key = node->getKey(parameters, 0);

    if (key == NULL) return false;

    QHash<QString, QVariant> hash = key->toHash();

    hash.remove(node->getString(parameters, 1));

    *key = hash;

    return true;
}

//-------------------------------------------------------------------------------------------------
// RegExp

inline QVariant regExp(const WBackendUniversalNode * node,
                       WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("REGEXP");
#endif

    if (node->nodes.count() < 1) return QVariant();

    return WRegExp(node->getString(parameters, 0));
}

inline QVariant regExpCap(const WBackendUniversalNode * node,
                          WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("REGEXP_CAP");
#endif

    int count = node->nodes.count();

    if (count < 3) return QString();

    if (count == 3)
    {
        return Sk::regExpCap(node->getString(parameters, 0),
                             WRegExp(node->getString(parameters, 1)),
                             node->getInt(parameters, 2));
    }
    else return Sk::regExpCap(node->getString(parameters, 0),
                              WRegExp(node->getString(parameters, 1)),
                              node->getInt(parameters, 2), node->getInt(parameters, 3));
}

//-------------------------------------------------------------------------------------------------
// Extension

inline QVariant extensionIsMedia(const WBackendUniversalNode * node,
                                 WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("EXTENSION_IS_MEDIA");
#endif

    if (node->nodes.count() < 1) return false;

    return WControllerPlaylist::extensionIsMedia(node->getString(parameters, 0));
}

inline QVariant extensionIsVideo(const WBackendUniversalNode * node,
                                 WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("EXTENSION_IS_VIDEO");
#endif

    if (node->nodes.count() < 1) return false;

    return WControllerPlaylist::extensionIsVideo(node->getString(parameters, 0));
}

inline QVariant extensionIsAudio(const WBackendUniversalNode * node,
                                 WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("EXTENSION_IS_AUDIO");
#endif

    if (node->nodes.count() < 1) return false;

    return WControllerPlaylist::extensionIsAudio(node->getString(parameters, 0));
}

inline QVariant extensionIsSubtitle(const WBackendUniversalNode * node,
                                    WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("EXTENSION_IS_SUBTITLE");
#endif

    if (node->nodes.count() < 1) return false;

    return WControllerPlaylist::extensionIsSubtitle(node->getString(parameters, 0));
}

//-------------------------------------------------------------------------------------------------
// Url

inline QVariant urlGenerate(const WBackendUniversalNode * node,
                            WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("URL_GENERATE");
#endif

    int count = node->nodes.count();

    if (count < 1) return QString();

    if (count == 1)
    {
         return WControllerNetwork::generateUrl(node->getString(parameters, 0));
    }
    else return WControllerNetwork::generateUrl(node->getString(parameters, 0),
                                                node->getString(parameters, 1));
}

inline QVariant urlEncode(const WBackendUniversalNode * node,
                          WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("URL_ENCODE");
#endif

    if (node->nodes.count() < 1) return QString();

    return WControllerNetwork::encodeUrl(node->getString(parameters, 0));
}

inline QVariant urlDecode(const WBackendUniversalNode * node,
                          WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("URL_DECODE");
#endif

    if (node->nodes.count() < 1) return QString();

    return WControllerNetwork::decodeUrl(node->getString(parameters, 0));
}

//-------------------------------------------------------------------------------------------------

inline QVariant urlName(const WBackendUniversalNode * node,
                        WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("URL_NAME");
#endif

    if (node->nodes.count() < 1) return QString();

    return WControllerNetwork::urlName(node->getString(parameters, 0));
}

inline QVariant urlValue(const WBackendUniversalNode * node,
                         WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("URL_VALUE");
#endif

    if (node->nodes.count() < 2) return QString();

    return WControllerNetwork::extractUrlValue(node->getString(parameters, 0),
                                               node->getString(parameters, 1));
}

inline QVariant urlFragment(const WBackendUniversalNode * node,
                            WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("URL_FRAGMENT");
#endif

    if (node->nodes.count() < 1) return QString();

    return QUrl(node->getString(parameters, 0)).fragment();
}

//-------------------------------------------------------------------------------------------------

inline QVariant urlPath(const WBackendUniversalNode * node,
                               WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("URL_PATH");
#endif

    if (node->nodes.count() < 1) return QString();

    return WControllerNetwork::extractUrlPath(node->getString(parameters, 0));
}

inline QVariant urlElement(const WBackendUniversalNode * node,
                           WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("URL_ELEMENT");
#endif

    int count = node->nodes.count();

    if (count < 2) return QString();

    if (count == 2)
    {
         return WControllerNetwork::extractUrlElement(node->getString(parameters, 0),
                                                      node->getInt(parameters, 1));
    }
    else return WControllerNetwork::extractUrlElement(node->getString(parameters, 0),
                                                      node->getInt(parameters, 1),
                                                      node->getInt(parameters, 2));
}

inline QVariant urlElements(const WBackendUniversalNode * node,
                            WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("URL_ELEMENTS");
#endif

    int count = node->nodes.count();

    if (count < 2) return QString();

    if (count == 2)
    {
         return WControllerNetwork::extractUrlElements(node->getString(parameters, 0),
                                                       node->getInt(parameters, 1));
    }
    else return WControllerNetwork::extractUrlElements(node->getString(parameters, 0),
                                                       node->getInt(parameters, 1),
                                                       node->getInt(parameters, 2));
}

inline QVariant urlExtension(const WBackendUniversalNode * node,
                             WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("URL_EXTENSION");
#endif

    if (node->nodes.count() < 1) return QString();

    return WControllerNetwork::extractUrlExtension(node->getString(parameters, 0));
}

//-------------------------------------------------------------------------------------------------

inline QVariant urlAddQuery(const WBackendUniversalNode * node,
                            WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("URL_ADD_QUERY");
#endif

    if (node->nodes.count() < 3) return QString();

    QVariant * key = node->getKey(parameters, 0);

    if (key == NULL) return QString();

    QUrl url(key->toString());

#ifdef QT_4
    url.addQueryItem(node->getString(parameters, 1), node->getString(parameters, 2));
#else
    QUrlQuery urlQuery(url);

    urlQuery.addQueryItem(node->getString(parameters, 1), node->getString(parameters, 2));

    url.setQuery(urlQuery);
#endif

    *key = url;

    return *key;
}

//-------------------------------------------------------------------------------------------------

inline QVariant urlRemovePrefix(const WBackendUniversalNode * node,
                                WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("URL_REMOVE_PREFIX");
#endif

    if (node->nodes.count() < 1) return QString();

    return WControllerNetwork::removeUrlPrefix(node->getString(parameters, 0));
}

inline QVariant urlRemoveExtension(const WBackendUniversalNode * node,
                                   WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("URL_REMOVE_EXTENSION");
#endif

    if (node->nodes.count() < 1) return QString();

    return WControllerNetwork::removeUrlExtension(node->getString(parameters, 0));
}

//-------------------------------------------------------------------------------------------------
// Html

inline QVariant htmlToUtf8(const WBackendUniversalNode * node,
                           WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("HTML_TO_UTF8");
#endif

    if (node->nodes.count() < 1) return QString();

    return WControllerNetwork::htmlToUtf8(node->getString(parameters, 0));
}

//-------------------------------------------------------------------------------------------------

inline QVariant htmlExtract(const WBackendUniversalNode * node,
                            WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("HTML_EXTRACT");
#endif

    int count = node->nodes.count();

    if (count < 2) return QVariant();

    WControllerPlaylistData data;

    if (count != 2)
    {
        node->applySlice(&data, node->getStringList(parameters, 2));
    }

    data.applyHtml(node->getByteArray(parameters, 0), node->getString(parameters, 1));

    return node->getHtml(data);
}

//-------------------------------------------------------------------------------------------------

inline QVariant htmlTracks(const WBackendUniversalNode * node,
                           WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("HTML_TRACKS");
#endif

    int count = node->nodes.count();

    if (count < 2) return QVariant();

    WControllerPlaylistData data;

    if (count != 2)
    {
        node->applySlice(&data, node->getStringList(parameters, 2));
    }

    data.applyHtml(node->getByteArray(parameters, 0), node->getString(parameters, 1));

    return WControllerPlaylist::extractTracks(data);
}

inline QVariant htmlPlaylists(const WBackendUniversalNode * node,
                              WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("HTML_PLAYLISTS");
#endif

    int count = node->nodes.count();

    if (count < 2) return QVariant();

    WControllerPlaylistData data;

    if (count != 2)
    {
        node->applySlice(&data, node->getStringList(parameters, 2));
    }

    data.applyHtml(node->getByteArray(parameters, 0), node->getString(parameters, 1));

    QList<WControllerPlaylistUrl> urls = WControllerPlaylist::extractPlaylists(data);

    return node->getPlaylists(urls);
}

//-------------------------------------------------------------------------------------------------

inline QVariant htmlAttribute(const WBackendUniversalNode * node,
                              WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("HTML_ATTRIBUTE");
#endif

    int count = node->nodes.count();

    if (count < 2) return QString();

    if (count == 2)
    {
         return WControllerNetwork::extractAttribute(node->getString(parameters, 0),
                                                     node->getString(parameters, 1));
    }
    else return WControllerNetwork::extractAttribute(node->getString(parameters, 0),
                                                     node->getString(parameters, 1),
                                                     node->getInt(parameters, 2));
}

inline QVariant htmlAttributeAt(const WBackendUniversalNode * node,
                                WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("HTML_ATTRIBUTE_AT");
#endif

    if (node->nodes.count() < 2) return QString();

    return WControllerNetwork::extractAttributeAt(node->getString(parameters, 0),
                                                  node->getInt(parameters, 1));
}

inline QVariant htmlAttributeUtf8(const WBackendUniversalNode * node,
                                  WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("HTML_ATTRIBUTE_UTF8");
#endif

    int count = node->nodes.count();

    if (count < 2) return QString();

    if (count == 2)
    {
         return WControllerNetwork::extractAttributeUtf8(node->getString(parameters, 0),
                                                         node->getString(parameters, 1));
    }
    else return WControllerNetwork::extractAttributeUtf8(node->getString(parameters, 0),
                                                         node->getString(parameters, 1),
                                                         node->getInt(parameters, 2));
}

inline QVariant htmlAttributeUtf8At(const WBackendUniversalNode * node,
                                    WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("HTML_ATTRIBUTE_UTF8_AT");
#endif

    if (node->nodes.count() < 2) return QString();

    return WControllerNetwork::extractAttributeUtf8At(node->getString(parameters, 0),
                                                      node->getInt(parameters, 1));
}

//-------------------------------------------------------------------------------------------------
// Json

inline QVariant jsonExtract(const WBackendUniversalNode * node,
                            WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("JSON_EXTRACT");
#endif

    int count = node->nodes.count();

    if (count < 1) return QString();

    if (count == 1)
    {
        return WControllerNetwork::extractJson(node->getString(parameters, 0));
    }
    else if (count == 2)
    {
        return WControllerNetwork::extractJson(node->getString(parameters, 0),
                                               node->getString(parameters, 1));
    }
    else return WControllerNetwork::extractJson(node->getString(parameters, 0),
                                                node->getString(parameters, 1),
                                                node->getInt(parameters, 2));
}

inline QVariant jsonExtractUtf8(const WBackendUniversalNode * node,
                                WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("JSON_EXTRACT_UTF8");
#endif

    int count = node->nodes.count();

    if (count < 1) return QString();

    if (count == 1)
    {
        return WControllerNetwork::extractJsonUtf8(node->getString(parameters, 0));
    }
    else if (count == 2)
    {
        return WControllerNetwork::extractJsonUtf8(node->getString(parameters, 0),
                                                   node->getString(parameters, 1));
    }
    else return WControllerNetwork::extractJsonUtf8(node->getString(parameters, 0),
                                                    node->getString(parameters, 1),
                                                    node->getInt(parameters, 2));
}

inline QVariant jsonExtractHtml(const WBackendUniversalNode * node,
                                WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("JSON_EXTRACT_HTML");
#endif

    int count = node->nodes.count();

    if (count < 1) return QString();

    if (count == 1)
    {
        return WControllerNetwork::extractJsonHtml(node->getString(parameters, 0));
    }
    else if (count == 2)
    {
        return WControllerNetwork::extractJsonHtml(node->getString(parameters, 0),
                                                   node->getString(parameters, 1));
    }
    else return WControllerNetwork::extractJsonHtml(node->getString(parameters, 0),
                                                    node->getString(parameters, 1),
                                                    node->getInt(parameters, 2));
}

//-------------------------------------------------------------------------------------------------

inline QVariant jsonSplit(const WBackendUniversalNode * node,
                          WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("JSON_SPLIT");
#endif

    int count = node->nodes.count();

    if (count < 1) return QVariant();

    if (count == 1)
    {
         return node->variants(WControllerNetwork::splitJson(node->getString(parameters, 0)));
    }
    else return node->variants(WControllerNetwork::splitJson(node->getString(parameters, 0),
                                                             node->getInt(parameters, 1)));
}

//-------------------------------------------------------------------------------------------------
// JavaScript

inline QVariant jsExtract(const WBackendUniversalNode * node,
                          WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("JS_EXTRACT");
#endif

    int count = node->nodes.count();

    if (count < 2) return QString();

    if (count == 2)
    {
        return WControllerNetwork::extractScript(node->getString(parameters, 0),
                                                 node->getString(parameters, 1));
    }
    else return WControllerNetwork::extractScript(node->getString(parameters, 0),
                                                  node->getString(parameters, 1),
                                                  node->getInt(parameters, 2));
}

//-------------------------------------------------------------------------------------------------

inline QVariant jsCall(const WBackendUniversalNode * node,
                       WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("JS_CALL");
#endif

    int count = node->nodes.count();

    if (count < 1) return QVariant();

    QVariantList variants;

    if (count != 1)
    {
        QStringList list;

        for (int i = 1; i < count; i++)
        {
            list.append(node->getString(parameters, i));
        }

        variants.append(list);
    }

    return WBackendUniversalEngine::call(node->getString(parameters, 0), variants).first();
}

inline QVariant jsCalls(const WBackendUniversalNode * node,
                        WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("JS_CALLS");
#endif

    int count = node->nodes.count();

    if (count < 2) return QVariant();

    QVariantList variants = WBackendUniversalEngine::call(node->getString(parameters, 0),
                                                          node->getList  (parameters, 1));

    return variants;
}

//-------------------------------------------------------------------------------------------------
// Zip

inline QVariant zipFileNames(const WBackendUniversalNode * node,
                             WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("ZIP_FILENAMES");
#endif

    int count = node->nodes.count();

    if (count < 1) return QString();

    QBuffer buffer;

    buffer.setData(node->getByteArray(parameters, 0));

    return WUnzipper::getFileNames(&buffer);
}

inline QVariant zipExtractFile(const WBackendUniversalNode * node,
                               WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("ZIP_EXTRACT_FILE");
#endif

    int count = node->nodes.count();

    if (count < 2) return QString();

    QBuffer buffer;

    buffer.setData(node->getByteArray(parameters, 0));

    return WUnzipper::extractFile(&buffer, node->getString(parameters, 1));
}

#ifndef SK_NO_TORRENT

//-------------------------------------------------------------------------------------------------
// Torrent

inline QVariant torrentStringAfter(const WBackendUniversalNode * node,
                                   WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("TORRENT_STRING_AFTER");
#endif

    int count = node->nodes.count();

    if (count < 2) return QString();

    if (count == 2)
    {
         return WControllerNetwork::stringBencodeAfter(node->getString(parameters, 0),
                                                       node->getString(parameters, 1));
    }
    else return WControllerNetwork::stringBencodeAfter(node->getString(parameters, 0),
                                                       node->getString(parameters, 1),
                                                       node->getInt(parameters, 2));
}

inline QVariant torrentIntegerAfter(const WBackendUniversalNode * node,
                                    WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("TORRENT_INTEGER_AFTER");
#endif

    int count = node->nodes.count();

    if (count < 2) return -1;

    if (count == 2)
    {
         return WControllerNetwork::integerBencodeAfter(node->getString(parameters, 0),
                                                        node->getString(parameters, 1));
    }
    else return WControllerNetwork::integerBencodeAfter(node->getString(parameters, 0),
                                                        node->getString(parameters, 1),
                                                        node->getInt(parameters, 2));
}

inline QVariant torrentListAfter(const WBackendUniversalNode * node,
                                 WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("TORRENT_LIST_AFTER");
#endif

    int count = node->nodes.count();

    if (count < 2) return QString();

    if (count == 2)
    {
         return WControllerNetwork::listBencodeAfter(node->getString(parameters, 0),
                                                     node->getString(parameters, 1));
    }
    else return WControllerNetwork::listBencodeAfter(node->getString(parameters, 0),
                                                     node->getString(parameters, 1),
                                                     node->getInt(parameters, 2));
}

//-------------------------------------------------------------------------------------------------

inline QVariant torrentItems(const WBackendUniversalNode * node,
                             WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("TORRENT_ITEMS");
#endif

    if (node->nodes.count() < 1) return QVariant();

    QString data = node->getString(parameters, 0);

    QList<WTorrentItemData> items = WControllerNetwork::torrentItems(data);

    return node->getTorrentItems(items);
}

//-------------------------------------------------------------------------------------------------

inline QVariant torrentFolders(const WBackendUniversalNode * node,
                               WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("TORRENT_FOLDERS");
#endif

    if (node->nodes.count() < 1) return QVariant();

    QList<WTorrentItemData> items = node->torrentItems(parameters, 0);

    QList<WTorrentItemFolder> folder = WControllerNetwork::torrentFolders(items);

    return node->getTorrentFolders(folder);
}

#endif

//-------------------------------------------------------------------------------------------------

inline QVariant print(const WBackendUniversalNode * node, WBackendUniversalParameters * parameters)
{
    int count = node->nodes.count();

    if (count < 1) return QVariant();

    QString string;

    string.append(node->getString(parameters, 0));

    for (int i = 1; i < count; i++)
    {
        string.append(' ' + node->getString(parameters, i));
    }

    qDebug("PRINT [%s]", string.C_STR);

    return QVariant();
}

//-------------------------------------------------------------------------------------------------

void populateHash()
{
    if (hash.isEmpty() == false) return;

    hash.insert("EQUAL",                  equal);
    hash.insert("NOT_EQUAL",              notEqual);
    hash.insert("LESSER",                 lesser);
    hash.insert("GREATER",                greater);
    hash.insert("LESSER_EQUAL",           lesserEqual);
    hash.insert("GREATER_EQUAL",          greaterEqual);
    hash.insert("SET",                    set);
    hash.insert("NUMBER",                 number);
    hash.insert("TIME",                   time);
    hash.insert("DATE",                   date);
    hash.insert("ADD",                    add);
    hash.insert("SUB",                    sub);
    hash.insert("MULTIPLY",               multiply);
    hash.insert("DIVIDE",                 divide);
    hash.insert("GET_CHAR",               getChar);
    hash.insert("PREPEND",                prepend);
    hash.insert("APPEND",                 append);
    hash.insert("REMOVE",                 remove);
    hash.insert("CHOP",                   chop);
    hash.insert("REPLACE",                replace);
    hash.insert("MID",                    mid);
    hash.insert("SPLIT",                  split);
    hash.insert("JOIN",                   join);
    hash.insert("LOWER",                  lower);
    hash.insert("SIMPLIFY",               simplify);
    hash.insert("READ",                   read);
    hash.insert("LATIN_TO_UTF8",          latinToUtf8);
    hash.insert("LENGTH",                 length);
    hash.insert("COUNT",                  count);
    hash.insert("COUNT_STRING",           countString);
    hash.insert("INDEX_OF",               indexOf);
    hash.insert("INDEX_END",              indexEnd);
    hash.insert("INDEX_SKIP",             indexSkip);
    hash.insert("LAST_INDEX_OF",          lastIndexOf);
    hash.insert("LAST_INDEX_END",         lastIndexEnd);
    hash.insert("CONTAIN",                contain);
    hash.insert("START_WITH",             startWith);
    hash.insert("END_WITH",               endWith);
    hash.insert("SLICE",                  slice);
    hash.insert("SLICE_IN",               sliceIn);
    hash.insert("SLICES",                 slices);
    hash.insert("SLICES_IN",              slicesIn);
    hash.insert("LIST",                   list);
    hash.insert("LIST_GET",               listGet);
    hash.insert("LIST_SET",               listSet);
    hash.insert("LIST_TAKE_AT",           listTakeAt);
    hash.insert("LIST_APPEND",            listAppend);
    hash.insert("LIST_INSERT",            listInsert);
    hash.insert("LIST_REMOVE",            listRemove);
    hash.insert("LIST_REMOVE_AT",         listRemoveAt);
    hash.insert("LIST_INDEX",             listIndex);
    hash.insert("LIST_CONTAIN",           listContain);
    hash.insert("HASH_GET",               hashGet);
    hash.insert("HASH_SET",               hashSet);
    hash.insert("HASH_REMOVE",            hashRemove);
    hash.insert("REGEXP",                 regExp);
    hash.insert("REGEXP_CAP",             regExpCap);
    hash.insert("EXTENSION_IS_MEDIA",     extensionIsMedia);
    hash.insert("EXTENSION_IS_VIDEO",     extensionIsVideo);
    hash.insert("EXTENSION_IS_AUDIO",     extensionIsAudio);
    hash.insert("EXTENSION_IS_SUBTITLE",  extensionIsSubtitle);
    hash.insert("URL_GENERATE",           urlGenerate);
    hash.insert("URL_ENCODE",             urlEncode);
    hash.insert("URL_DECODE",             urlDecode);
    hash.insert("URL_NAME",               urlName);
    hash.insert("URL_VALUE",              urlValue);
    hash.insert("URL_FRAGMENT",           urlFragment);
    hash.insert("URL_PATH",               urlPath);
    hash.insert("URL_ELEMENT",            urlElement);
    hash.insert("URL_ELEMENTS",           urlElements);
    hash.insert("URL_EXTENSION",          urlExtension);
    hash.insert("URL_ADD_QUERY",          urlAddQuery);
    hash.insert("URL_REMOVE_PREFIX",      urlRemovePrefix);
    hash.insert("URL_REMOVE_EXTENSION",   urlRemoveExtension);
    hash.insert("HTML_TO_UTF8",           htmlToUtf8);
    hash.insert("HTML_EXTRACT",           htmlExtract);
    hash.insert("HTML_TRACKS",            htmlTracks);
    hash.insert("HTML_PLAYLISTS",         htmlPlaylists);
    hash.insert("HTML_ATTRIBUTE",         htmlAttribute);
    hash.insert("HTML_ATTRIBUTE_AT",      htmlAttributeAt);
    hash.insert("HTML_ATTRIBUTE_UTF8",    htmlAttributeUtf8);
    hash.insert("HTML_ATTRIBUTE_UTF8_AT", htmlAttributeUtf8At);
    hash.insert("JSON_EXTRACT",           jsonExtract);
    hash.insert("JSON_EXTRACT_UTF8",      jsonExtractUtf8);
    hash.insert("JSON_EXTRACT_HTML",      jsonExtractHtml);
    hash.insert("JSON_SPLIT",             jsonSplit);
    hash.insert("JS_EXTRACT",             jsExtract);
    hash.insert("JS_CALL",                jsCall);
    hash.insert("JS_CALLS",               jsCalls);
    hash.insert("ZIP_FILENAMES",          zipFileNames);
    hash.insert("ZIP_EXTRACT_FILE",       zipExtractFile);
#ifndef SK_NO_TORRENT
    hash.insert("TORRENT_STRING_AFTER",   torrentStringAfter);
    hash.insert("TORRENT_INTEGER_AFTER",  torrentIntegerAfter);
    hash.insert("TORRENT_LIST_AFTER",     torrentListAfter);
    hash.insert("TORRENT_ITEMS",          torrentItems);
    hash.insert("TORRENT_FOLDERS",        torrentFolders);
#endif
    hash.insert("PRINT",                  print);

    cache = new WBackendCache(sk);
}

//=================================================================================================
// WBackendUniversalLoop
//=================================================================================================

struct WBackendUniversalLoop
{
    int index;

    QVariantList list;

    QVariant * value;
};

//=================================================================================================
// WBackendUniversalQuery
//=================================================================================================

class WBackendUniversalQuery : public QObject
{
    Q_OBJECT

public: // Interface
    Q_INVOKABLE void extract(const QByteArray & array);

private: // Functions
    WBackendUniversalData::Engines extractEngines(const WYamlReader & reader,
                                                  const QString     & key) const;

    QList<WLibraryFolderItem> extractItems(const WYamlReader & reader) const;

    QString extractValue(const WYamlNode & node, const QString & key) const;

signals:
    void loaded(const WBackendUniversalData & data);
};

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WBackendUniversalQuery::extract(const QByteArray & array)
{
    WBackendUniversalData data;

    QString content = Sk::readBml(array);

    //reader.dump();

    //---------------------------------------------------------------------------------------------
    // Api

    QString api = WControllerPlaylist::vbmlVersion(content);

    if (Sk::versionIsHigher(WControllerPlaylist::versionApi(), api))
    {
        WControllerPlaylist::vbmlPatch(content, api);

        extract(content.toUtf8());

        return;
    }

    if (Sk::versionIsLower(WControllerPlaylist::versionApi(), api))
    {
        qWarning("WBackendUniversalQuery::extract: The required API is too high.");
    }

    WYamlReader reader(content.toUtf8());

    //---------------------------------------------------------------------------------------------
    // Settings

    data.origin = reader.extractString("origin");

    data.api = api;

    data.version = reader.extractString("version");

    WBackendUniversalData::Engines engines = extractEngines(reader, "search");

    data.engines = engines;

    if (engines.testFlag(WBackendUniversalData::Tracks))
    {
        data.isSearchEngine = true;
    }

    if (engines.testFlag(WBackendUniversalData::CoverAudio)
        ||
        engines.testFlag(WBackendUniversalData::CoverVideo))
    {
        data.isSearchCover = true;
    }

    data.title = reader.extractString("title");

    data.host = reader.extractString("host");

    QString cover = reader.extractString("cover");

    QString baseUrl = WControllerNetwork::extractBaseUrl(data.origin);

    data.cover = WControllerPlaylist::vbmlSource(cover, baseUrl);

    data.hub = reader.extractString("hub");

    data.items = extractItems(reader);

    data.validate = reader.extractString("validate");

    //---------------------------------------------------------------------------------------------
    // Interface

    data.trackId     = reader.extractString("TRACK_ID");
    data.trackOutput = reader.extractString("TRACK_OUTPUT");

    data.playlistInfo = reader.extractString("PLAYLIST_INFO");

    data.urlTrack    = reader.extractString("URL_TRACK");
    data.urlPlaylist = reader.extractString("URL_PLAYLIST");

    data.querySource   = reader.extractString("QUERY_SOURCE");
    data.queryTrack    = reader.extractString("QUERY_TRACK");
    data.queryPlaylist = reader.extractString("QUERY_PLAYLIST");
    data.queryFolder   = reader.extractString("QUERY_FOLDER");
    data.queryItem     = reader.extractString("QUERY_ITEM");

    data.createQuery = reader.extractString("CREATE_QUERY");

    data.extractSource   = reader.extractString("EXTRACT_SOURCE");
    data.extractTrack    = reader.extractString("EXTRACT_TRACK");
    data.extractPlaylist = reader.extractString("EXTRACT_PLAYLIST");
    data.extractFolder   = reader.extractString("EXTRACT_FOLDER");
    data.extractItem     = reader.extractString("EXTRACT_ITEM");

    data.queryFailed = reader.extractString("QUERY_FAILED");

    data.applySource   = reader.extractString("APPLY_SOURCE");
    data.applyTrack    = reader.extractString("APPLY_TRACK");
    data.applyPlaylist = reader.extractString("APPLY_PLAYLIST");
    data.applyFolder   = reader.extractString("APPLY_FOLDER");
    data.applyItem     = reader.extractString("APPLY_ITEM");

    //---------------------------------------------------------------------------------------------

    emit loaded(data);

    deleteLater();
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

WBackendUniversalData::Engines
WBackendUniversalQuery::extractEngines(const WYamlReader & reader, const QString & key) const
{
    QString string = reader.extractString(key);

    WRegExp regExp("tracks|coverAudio|coverVideo");

    if (regExp.indexIn(string) == -1)
    {
        return WBackendUniversalData::None;
    }

    WBackendUniversalData::Engines engines;

    foreach (const QString & match, regExp.capturedTexts())
    {
        if (match == "tracks")
        {
            engines |= WBackendUniversalData::Tracks;
        }
        else if (match == "coverAudio")
        {
            engines |= WBackendUniversalData::CoverAudio;
        }
        else // if (match == "coverVideo")
        {
            engines |= WBackendUniversalData::CoverVideo;
        }
    }

    return engines;
}

//-------------------------------------------------------------------------------------------------

QList<WLibraryFolderItem> WBackendUniversalQuery::extractItems(const WYamlReader & reader) const
{
    QList<WLibraryFolderItem> list;

    const WYamlNode * node = reader.at("items");

    if (node == NULL) return list;

    foreach (const WYamlNode & node, node->children)
    {
        WLibraryFolderItem item;

        if (node.key == "folder")
        {
             item.type = WLibraryItem::FolderSearch;
        }
        else item.type = WLibraryItem::PlaylistSearch;

        QString title = extractValue(node, "title");
        QString label = extractValue(node, "label");

        if (title.isEmpty())
        {
            title = label;
        }
        else if (label.isEmpty())
        {
            label = title.toLower();
        }

        item.title = title;
        item.label = label;

        list.append(item);
    }

    return list;
}

//-------------------------------------------------------------------------------------------------

QString WBackendUniversalQuery::extractValue(const WYamlNode & node, const QString & key) const
{
    const WYamlNode * child = node.at(key);

    if (child)
    {
         return child->value;
    }
    else return QString();
}

//=================================================================================================
// WBackendUniversalNode
//=================================================================================================

WBackendUniversalNode::WBackendUniversalNode() {}

WBackendUniversalNode::WBackendUniversalNode(Type type)
{
    this->type = type;
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

QVariant WBackendUniversalNode::run(WBackendUniversalParameters * parameters) const
{
    function call = hash.value(data);

    return call(this, parameters);
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalNode::dump(int indent) const
{
    QString string;

    for (int i = 0; i < indent; i++)
    {
        string.append(' ');
    }

    if (type == WBackendUniversalNode::String)
    {
        qDebug("%sSTRING [%s]", string.C_STR, data.C_STR);
    }
    else if (type == WBackendUniversalNode::Variable)
    {
        qDebug("%sVARIABLE [%s]", string.C_STR, data.C_STR);
    }
    else if (type == WBackendUniversalNode::Function)
    {
        qDebug("%sFUNCTION [%s]", string.C_STR, data.C_STR);

        indent += 4;

        foreach (const WBackendUniversalNode & node, nodes)
        {
            node.dump(indent + 4);
        }
    }
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalNode::applySlice(WControllerPlaylistData * data,
                                       const QStringList       & list) const
{
    foreach (const QString & string, list)
    {
        int index = string.indexOf('|');

        if (index == -1)
        {
             data->addSlice(string);
        }
        else data->addSlice(string.mid(0, index), string.mid(index + 1));
    }
}

//-------------------------------------------------------------------------------------------------

QVariant WBackendUniversalNode::getVariant(WBackendUniversalParameters * parameters,
                                           int                           index) const
{
    const WBackendUniversalNode & node = nodes.at(index);

    if (node.type == String)
    {
        return node.data;
    }
    else if (node.type == Variable)
    {
        QString data = node.data;

        const QVariant * variant = parameters->valueConst(data);

        if (variant)
        {
             return *variant;
        }
        else return data;
    }
    else // if (node.type == Function)
    {
        return node.run(parameters);
    }
}

//-------------------------------------------------------------------------------------------------

int WBackendUniversalNode::getInt(WBackendUniversalParameters * parameters, int index) const
{
    return getVariant(parameters, index).toInt();
}

qreal WBackendUniversalNode::getReal(WBackendUniversalParameters * parameters, int index) const
{
    return getVariant(parameters, index).toReal();
}

//-------------------------------------------------------------------------------------------------

QByteArray WBackendUniversalNode::getByteArray(WBackendUniversalParameters * parameters,
                                               int                           index) const
{
    return getVariant(parameters, index).toByteArray();
}

QString WBackendUniversalNode::getString(WBackendUniversalParameters * parameters,
                                         int                           index) const
{
    return getVariant(parameters, index).toString();
}

//-------------------------------------------------------------------------------------------------

QVariantList
WBackendUniversalNode::getList(WBackendUniversalParameters * parameters, int index) const
{
    return getVariant(parameters, index).toList();
}

//-------------------------------------------------------------------------------------------------

#ifndef SK_NO_TORRENT

QVariantList
WBackendUniversalNode::getTorrentItems(const QList<WTorrentItemData> & items) const
{
    QVariantList list;

    foreach (const WTorrentItemData & item, items)
    {
        QHash<QString, QVariant> hash;

        hash.insert("id", item.id);

        hash.insert("path", item.path);
        hash.insert("name", item.name);

        hash.insert("index", item.index);

        list.append(hash);
    }

    return list;
}

QVariantList
WBackendUniversalNode::getTorrentFolders(const QList<WTorrentItemFolder> & folders) const
{
    QVariantList list;

    foreach (const WTorrentItemFolder & folder, folders)
    {
        QHash<QString, QVariant> hash;

        hash.insert("items", getTorrentItems(folder.items));

        list.append(hash);
    }

    return list;
}

#endif

//-------------------------------------------------------------------------------------------------

QStringList
WBackendUniversalNode::getStringList(WBackendUniversalParameters * parameters, int index) const
{
    return getVariant(parameters, index).toStringList();
}

//-------------------------------------------------------------------------------------------------

QList<WRegExp> WBackendUniversalNode::getRegExps(const QVariantList & variants) const
{
    foreach (const QVariant & variant, variants)
    {
        if (WRegExp::isRegExp(variant) == false) continue;

        QList<WRegExp> regExps;

        foreach (const QVariant & variant, variants)
        {
            if (WRegExp::isRegExp(variant))
            {
                 regExps.append(WRegExp::fromVariant(variant));
            }
            else regExps.append(WRegExp(variant.toString()));
        }

        return regExps;
    }

    return QList<WRegExp>();
}

//-------------------------------------------------------------------------------------------------

QHash<QString, QVariant>
WBackendUniversalNode::getHash(WBackendUniversalParameters * parameters, int index) const
{
    return getVariant(parameters, index).toHash();
}

//-------------------------------------------------------------------------------------------------

QHash<QString, QVariant> WBackendUniversalNode::getHtml(const WControllerPlaylistData & data) const
{
    QHash<QString, QVariant> hash;

    hash.insert("title", data.title);
    hash.insert("cover", data.cover);

    QVariantList list;

    foreach (const WControllerPlaylistSource & source, data.sources)
    {
        QHash<QString, QVariant> variant;

        variant.insert("url",   source.url);
        variant.insert("title", source.title);

        list.append(variant);
    }

    hash.insert("sources", list);

    list.clear();

    foreach (const WControllerPlaylistMedia & media, data.medias)
    {
        QHash<QString, QVariant> variant;

        variant.insert("url", media.url);

        variant.insert("title", media.title);
        variant.insert("cover", media.cover);

        list.append(variant);
    }

    hash.insert("medias", list);

    return hash;
}

QVariantList WBackendUniversalNode::getPlaylists(const QList<WControllerPlaylistUrl> & urls) const
{
    QVariantList list;

    foreach (const WControllerPlaylistUrl & url, urls)
    {
        QHash<QString, QVariant> variant;

        if (url.type == WLibraryItem::PlaylistFeed)
        {
             variant.insert("type", "feed");
        }
        else variant.insert("type", "playlist");

        variant.insert("url", url.url);

        list.append(variant);
    }

    return list;
}

//-------------------------------------------------------------------------------------------------

QVariant * WBackendUniversalNode::getKey(WBackendUniversalParameters * parameters, int index) const
{
    const WBackendUniversalNode & node = nodes.at(index);

    if (node.type == Variable)
    {
         return parameters->value(node.data);
    }
    else return NULL;
}

//-------------------------------------------------------------------------------------------------

const QVariantList WBackendUniversalNode::variants(const QStringList & list) const
{
    QVariantList variants;

    foreach (const QString & string, list)
    {
        variants.append(string);
    }

    return variants;
}

//-------------------------------------------------------------------------------------------------

#ifndef SK_NO_TORRENT

QList<WTorrentItemData>
WBackendUniversalNode::torrentItems(WBackendUniversalParameters * parameters, int index) const
{
    QList<WTorrentItemData> items;

    QVariantList list = getList(parameters, index);

    foreach (const QVariant & variant, list)
    {
        QHash<QString, QVariant> hash = variant.toHash();

        WTorrentItemData item;

        item.id = hash.value("id").toInt();

        item.path = hash.value("path").toString();
        item.name = hash.value("name").toString();

        item.index = hash.value("index").toInt();

        items.append(item);
    }

    return items;
}

#endif

//=================================================================================================
// WBackendUniversalScript
//=================================================================================================

WBackendUniversalScript::WBackendUniversalScript(const QString & data)
{
    // NOTE: We need to populate the hash here for the standalone use cases.
    populateHash();

    load(data);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

bool WBackendUniversalScript::isValid() const
{
    if (nodes.isEmpty())
    {
         return false;
    }
    else return true;
}

//-------------------------------------------------------------------------------------------------

QVariant WBackendUniversalScript::run(WBackendUniversalParameters * parameters) const
{
    QList<int> conditions;

    int condition = -1;

    QList<WBackendUniversalLoop> loops;

    int index = 0;

    while (index < nodes.count())
    {
        const WBackendUniversalNode & node = nodes.at(index);

        const QString & data = node.data;

        if (data == "IF")
        {
            if (condition == 0)
            {
#ifdef SK_BACKEND_LOG
                qDebug("SKIP IF");
#endif

                skipCondition(&index);

                continue;
            }

            if (condition != -1)
            {
                if (condition > BACKENDUNIVERSAL_DEPTH_IF)
                {
                    qWarning("WBackendUniversalScript::run: Maximum 'IF' depth reached.");

                    return QVariant();
                }

                conditions.append(condition);
            }

            condition = getCondition(parameters, node, &index);

            skipOperators(&index);

#ifdef SK_BACKEND_LOG
            qDebug("IF %d", condition);
#endif

            continue;
        }
        else if (data == "ELSE")
        {
            if (condition == -1)
            {
                qWarning("WBackendUniversalScript::run: Invalid 'ELSE' statement.");

                return QVariant();
            }

            if (condition == 1)
            {
                condition = 0;

#ifdef SK_BACKEND_LOG
                qDebug("ELSE SKIP");
#endif

                if (skipCondition(&index))
                {
                    if (conditions.isEmpty())
                    {
                         condition = -1;
                    }
                    else condition = conditions.takeLast();
                }

                continue;
            }
            else
            {
#ifdef SK_BACKEND_LOG
                qDebug("ELSE");
#endif

                condition = 1;
            }
        }
        else if (data == "ELIF")
        {
            if (condition == -1)
            {
                qWarning("WBackendUniversalScript::run: Invalid 'ELIF' statement.");

                return QVariant();
            }

            if (condition == 1)
            {
#ifdef SK_BACKEND_LOG
                qDebug("ELIF SKIP");
#endif

                if (skipCondition(&index))
                {
                    if (conditions.isEmpty())
                    {
                         condition = -1;
                    }
                    else condition = conditions.takeLast();
                }
            }
            else
            {
#ifdef SK_BACKEND_LOG
                qDebug("ELIF");
#endif

                condition = getCondition(parameters, node, &index);

                skipOperators(&index);
            }

            continue;
        }
        else if (data == "FI")
        {
            if (condition == -1)
            {
                qWarning("WBackendUniversalScript::run: Invalid 'FI' statement.");

                return QVariant();
            }

#ifdef SK_BACKEND_LOG
            qDebug("FI %d", condition);
#endif

            if (conditions.isEmpty())
            {
                 condition = -1;
            }
            else condition = conditions.takeLast();
        }
        else if (condition == 0)
        {
#ifdef SK_BACKEND_LOG
            qDebug("SKIP");
#endif

            index++;

            continue;
        }
        else if (data == "FOREACH")
        {
            int count = node.nodes.count();

            if (count < 2)
            {
                qWarning("WBackendUniversalScript::run: Invalid 'FOREACH' statement.");

                return QVariant();
            }

            QVariant * value = node.getKey(parameters, 1);

            if (value)
            {
                WBackendUniversalLoop loop;

                QVariantList list = node.getList(parameters, 0);

#ifdef SK_BACKEND_LOG
                qDebug("FOREACH %d", list.count());
#endif
                if (list.isEmpty())
                {
                    skipLoop(&index);

                    continue;
                }
                else
                {
                    if (loops.count() > BACKENDUNIVERSAL_DEPTH_LOOP)
                    {
                        qWarning("WBackendUniversalScript::run: Maximum 'FOREACH' depth reached.");

                        return QVariant();
                    }

                    *value = list.takeFirst();

#ifdef SK_BACKEND_LOG
                    qDebug("LOOP %s", value->toString().C_STR);
#endif

                    loop.index = index + 1;

                    if (count == 3)
                    {
                        count = node.getInt(parameters, 2) - 1;

                        if (count > list.count())
                        {
                            count = list.count();
                        }

                        for (int i = 0; i < count; i++)
                        {
                            loop.list.append(list.at(i));
                        }
                    }
                    else if (count > BACKENDUNIVERSAL_MAX_LOOP)
                    {
                        for (int i = 0; i < BACKENDUNIVERSAL_MAX_LOOP; i++)
                        {
                            loop.list.append(list.at(i));
                        }
                    }
                    else loop.list = list;

                    loop.value = value;

                    loops.append(loop);
                }
            }
            else
            {
                qWarning("WBackendUniversalScript::run: Invalid 'FOREACH' parameters.");

                return QVariant();
            }
        }
        else if (data == "CONTINUE")
        {
#ifdef SK_BACKEND_LOG
            qDebug("CONTINUE");
#endif

            if (skipEnd(&index) == false)
            {
                qWarning("WBackendUniversalScript::run: Invalid 'CONTINUE' statement.");

                return QVariant();
            }
            else continue;
        }
        else if (data == "BREAK")
        {
#ifdef SK_BACKEND_LOG
            qDebug("BREAK");
#endif

            if (skipEnd(&index) == false)
            {
                qWarning("WBackendUniversalScript::run: Invalid 'BREAK' statement.");

                return QVariant();
            }
            else
            {
                loops.removeLast();

                index++;

                continue;
            }

            return QVariant();
        }
        else if (data == "END")
        {
            if (loops.isEmpty())
            {
                qWarning("WBackendUniversalScript::run: Invalid 'END' statement.");

                return QVariant();
            }

            WBackendUniversalLoop & loop = loops.last();

            QVariantList * list = &(loop.list);

            if (list->isEmpty() == false)
            {
                *(loop.value) = list->takeFirst();

#ifdef SK_BACKEND_LOG
                qDebug("LOOP %s", loop.value->toString().C_STR);
#endif

                index = loop.index;

                continue;
            }
            else
            {
                loops.removeLast();

#ifdef SK_BACKEND_LOG
                qDebug("END %d", loops.count());
#endif
            }
        }
        else if (data == "RETURN")
        {
#ifdef SK_BACKEND_LOG
            qDebug("RETURN");
#endif

            if (node.nodes.count())
            {
                 return node.getVariant(parameters, 0);
            }
            else return true;
        }
        else node.run(parameters);

        index++;
    }

    return QVariant();
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalScript::dump() const
{
    foreach (const WBackendUniversalNode & node, nodes)
    {
        node.dump();
    }
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBackendUniversalScript::load(const QString & data)
{
    if (data.isEmpty()) return;

    WRegExp regExp(BACKENDUNIVERSAL_FUNCTIONS);

    QString content = data;

    while (content.isEmpty() == false)
    {
        QString line = Sk::extractLine(&content);

        QString string = line;

        while (string.isEmpty() == false)
        {
            Sk::skipSpaces(&string);

            QString word = extractWord(&string);

            if (word.startsWith('#')) break;

            if (regExp.exactMatch(word))
            {
                WBackendUniversalNode node(WBackendUniversalNode::Function);

                node.data = word;

                if (loadParameters(&node, &string, regExp) == false)
                {
                    Sk::skipSpaces(&line);

                    qWarning("WBackendUniversalScript::load: Invalid parameters [%s].", line.C_STR);

                    nodes.clear();

                    return;
                }
                else if (nodes.count() > BACKENDUNIVERSAL_MAX_NODES)
                {
                    Sk::skipSpaces(&line);

                    qWarning("WBackendUniversalScript::load: Maximum nodes reached [%s].",
                             line.C_STR);

                    nodes.clear();

                    return;
                }

                nodes.append(node);
            }
            else
            {
                Sk::skipSpaces(&line);

                qWarning("WBackendUniversalScript::load: Invalid function [%s].", line.C_STR);

                nodes.clear();

                return;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

bool WBackendUniversalScript::loadParameters(WBackendUniversalNode * node,
                                             QString               * string,
                                             const WRegExp         & regExp) const
{
    Sk::skipSpaces(string);

    if (string->isEmpty()) return true;

    QChar character = string->at(0);

    if (character == ')' || character == '#') return true;

    if (character == '(')
    {
        string->remove(0, 1);

        WBackendUniversalNode child;

        if (loadFunction(&child, string, regExp))
        {
             child.type = WBackendUniversalNode::Function;
        }
        else child.type = WBackendUniversalNode::Variable;

        if (string->isEmpty())
        {
            qWarning("WBackendUniversalScript::loadFunction: Missing parenthesis [%s].",
                     string->C_STR);

            return false;
        }

        node->nodes.append(child);

        string->remove(0, 1);
    }
    else if (character == '"')
    {
        WBackendUniversalNode child(WBackendUniversalNode::String);

        child.data = extractString(string, '"');

        node->nodes.append(child);
    }
    else if (character == '\'')
    {
        WBackendUniversalNode child(WBackendUniversalNode::String);

        child.data = extractString(string, '\'');

        node->nodes.append(child);
    }
    else
    {
        WBackendUniversalNode child(WBackendUniversalNode::Variable);

        child.data = extractWord(string);

        node->nodes.append(child);
    }

    return loadParameters(node, string, regExp);
}

bool WBackendUniversalScript::loadFunction(WBackendUniversalNode * node,
                                           QString               * string,
                                           const WRegExp         & regExp) const
{
    QString word = extractWord(string);

    node->data = word;

    if (regExp.exactMatch(word))
    {
         return loadParameters(node, string, regExp);
    }
    else return false;
}

//-------------------------------------------------------------------------------------------------

QString WBackendUniversalScript::extractWord(QString * string) const
{
    return Sk::extractText(string, WRegExp("[\\s()]"));
}

QString WBackendUniversalScript::extractString(QString * string, const QChar & character) const
{
    QString result;

    int index = string->indexOf(character, 1);

    if (index == -1)
    {
        qWarning("WBackendUniversalScript::extractString: Missing closing quote [%s].",
                 string->C_STR);

        string->clear();
    }
    else
    {
        while (Sk::checkEscaped(*string, index))
        {
            int at = string->indexOf(character, index + 1);

            if (at == -1) break;

            index = at;
        }

        result = string->mid(1, index - 1);

        result.replace("\\n",  "\n");
        result.replace("\\/n", "\\n");

        result.replace("\\\"", "\"");
        result.replace("\\\\", "\\");

        string->remove(0, index + 1);
    }

    return result;
}

//-------------------------------------------------------------------------------------------------

bool WBackendUniversalScript::skipCondition(int * index) const
{
    int count = 0;

    (*index)++;

    while (*index < nodes.count())
    {
        const QString & data = nodes.at(*index).data;

        if (data == "IF")
        {
            count++;
        }
        else if (data == "FI")
        {
            if (count == 0)
            {
                (*index)++;

                return true;
            }
            else count--;
        }

        (*index)++;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalScript::skipLoop(int * index) const
{
    int count = 0;

    (*index)++;

    while (*index < nodes.count())
    {
        const QString & data = nodes.at(*index).data;

        if (data == "FOREACH")
        {
            count++;
        }
        else if (data == "END")
        {
            if (count == 0)
            {
                (*index)++;

                return;
            }
            else count--;
        }

        (*index)++;
    }
}

bool WBackendUniversalScript::skipEnd(int * index) const
{
    int count = 0;

    (*index)++;

    while (*index < nodes.count())
    {
        const QString & data = nodes.at(*index).data;

        if (data == "FOREACH")
        {
            count++;
        }
        else if (data == "END")
        {
            if (count)
            {
                count--;
            }
            else return true;
        }

        (*index)++;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalScript::skipOperators(int * index) const
{
    while (*index < nodes.count())
    {
        const QString & data = nodes.at(*index).data;

        if (data != "AND" && data != "OR") return;

        (*index)++;
    }
}

//-------------------------------------------------------------------------------------------------

bool WBackendUniversalScript::getCondition(WBackendUniversalParameters * parameters,
                                           const WBackendUniversalNode & node, int * index) const
{
    QString string = node.getString(parameters, 0);

    (*index)++;

    if (string == "true" || (string != "false" && string.toInt()))
    {
        while (*index < nodes.count())
        {
            const WBackendUniversalNode & node = nodes.at(*index);

            const QString & data = node.data;

            if (data == "AND")
            {
                return getCondition(parameters, node, index);
            }
            else if (data != "OR")
            {
                return true;
            }

            (*index)++;
        }

        return true;
    }
    else
    {
        if (*index == nodes.count()) return false;

        const WBackendUniversalNode & node = nodes.at(*index);

        if (node.data == "OR")
        {
             return getCondition(parameters, node, index);
        }
        else return false;
    }
}

//=================================================================================================
// WBackendUniversalParameters
//=================================================================================================

WBackendUniversalParameters::WBackendUniversalParameters(const WBackendUniversalScript & script,
                                                         const QVariant                & global)
{
    QStringList list;

    foreach (const WBackendUniversalNode & node, script.nodes)
    {
        extract(&list, node);
    }

    foreach (const QString & string, list)
    {
        if (string.at(0).isLower() && string != "false" && string != "true")
        {
#ifdef SK_BACKEND_LOG
            qDebug("PARAMETER [%s]", string.C_STR);
#endif

            parameters.insert(string, QVariant());
        }
    }

    parameters.insert("global", global);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

void WBackendUniversalParameters::add(const QString & name, const QVariant & value)
{
    parameters.insert(name, value);
}

//-------------------------------------------------------------------------------------------------

QVariant * WBackendUniversalParameters::value(const QString & name)
{
    QHash<QString, QVariant>::iterator i = parameters.find(name);

    if (i == parameters.end())
    {
         return NULL;
    }
    else return &(i.value());
}

const QVariant * WBackendUniversalParameters::valueConst(const QString & name) const
{
    QHash<QString, QVariant>::const_iterator i = parameters.find(name);

    if (i == parameters.end())
    {
         return NULL;
    }
    else return &(i.value());
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBackendUniversalParameters::extract(QStringList * list, const WBackendUniversalNode & node)
{
    if (node.type == WBackendUniversalNode::Variable)
    {
        const QString & data = node.data;

        if (list->contains(data) == false)
        {
            list->append(node.data);
        }

        return;
    }

    foreach (const WBackendUniversalNode & node, node.nodes)
    {
        if (node.type == WBackendUniversalNode::Variable)
        {
            const QString & data = node.data;

            if (list->contains(data) == false)
            {
                list->append(node.data);
            }
        }
        else extract(list, node);
    }
}

//=================================================================================================
// WBackendUniversalPrivate
//=================================================================================================

WBackendUniversalPrivate::WBackendUniversalPrivate(WBackendUniversal * p)
    : WBackendNetPrivate(p) {}

///* virtual */ WBackendUniversalPrivate::~WBackendUniversalPrivate()
//{
//    cache->removeScripts(id);
//}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::init(const QString & id, const QString & source)
{
    remote = NULL;

    loaded = false;

    this->id     = id;
    this->source = source;

    qRegisterMetaType<WBackendUniversalData>("WBackendUniversalData");

    const QMetaObject * meta = WBackendUniversalQuery().metaObject();

    method = meta->method(meta->indexOfMethod("extract(QByteArray)"));

    populateHash();

    load();
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::load()
{
    Q_Q(WBackendUniversal);

    remote = wControllerDownload->getData(source, BACKENDUNIVERSAL_TIMEOUT_LOAD);

    QObject::connect(remote, SIGNAL(loaded(WRemoteData *)), q, SLOT(onLoad()));
}

void WBackendUniversalPrivate::loadData(const QByteArray & array)
{
    Q_Q(WBackendUniversal);

    WBackendUniversalQuery * query = new WBackendUniversalQuery;

    QObject::connect(query, SIGNAL(loaded(WBackendUniversalData)),
                     q,     SLOT(onData(WBackendUniversalData)));

    query->moveToThread(wControllerPlaylist->thread());

    method.invoke(query, Q_ARG(const QByteArray &, array));
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::applyLoaded()
{
    Q_Q(WBackendUniversal);

    if (loaded == false)
    {
        loaded = true;

        emit q->loadedChanged();
    }

    emit q->loaded();
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::runQuery(WBackendNetQuery * query, const QString & name,
                                                                  const QString & source,
                                                                  const QString & url) const
{
    WBackendUniversalScript * script = cache->getScript(id + name, source);

    if (script == NULL) return;

    WBackendUniversalParameters parameters(*script, global);

    query->url = url;

    applyQueryParameters(&parameters, *query);

    if (script->run(&parameters).toBool())
    {
        applyQueryResults(&parameters, query);
    }
    else query->url = QString();
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::applyQueryParameters(WBackendUniversalParameters * parameters,
                                                    const WBackendNetQuery      & query) const
{
    parameters->add("type");

    parameters->add("backend", query.backend);

    parameters->add("url",         query.url);
    parameters->add("urlRedirect", query.urlRedirect);

    parameters->add("id", query.id);

    parameters->add("mode", WAbstractBackend::modeToString(query.mode));

    parameters->add("queryData", query.data);

    parameters->add("queryMethod", query.method);
    parameters->add("header",      query.header);
    parameters->add("body",        query.body);

    parameters->add("clearItems", query.clearItems);
    parameters->add("cookies",    query.cookies);
    parameters->add("skipError",  query.skipError);

    parameters->add("delay",   query.delay);
    parameters->add("timeout", query.timeout);
}

void WBackendUniversalPrivate::applyQueryResults(WBackendUniversalParameters * parameters,
                                                 WBackendNetQuery            * query) const
{
    query->type = getTypeQuery(parameters->value("type")->toString());

    QString backend = parameters->value("backend")->toString();

    // NOTE: By default, we apply the current backend id.
    if (backend.isEmpty()) query->backend = id;
    else                   query->backend = backend;

    query->url = parameters->value("url")->toString();

    query->id = parameters->value("id")->toInt();

    query->data = *(parameters->value("queryData"));

    query->method = parameters->value("queryMethod")->toString();
    query->header = parameters->value("header")     ->toString();
    query->body   = parameters->value("body")       ->toString();

    query->clearItems = parameters->value("clearItems")->toBool();
    query->cookies    = parameters->value("cookies")   ->toBool();
    query->skipError  = parameters->value("skipError") ->toBool();

    query->delay   = parameters->value("delay")  ->toInt();
    query->timeout = parameters->value("timeout")->toInt();
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::applySourceParameters(WBackendUniversalParameters * parameters,
                                                     const QByteArray            & data,
                                                     const WBackendNetQuery      & query,
                                                     const WBackendNetSource     & reply) const
{
    parameters->add("data", data);

    applyQueryParameters(parameters, query);

    parameters->add("valid", reply.valid);

    parameters->add("medias");
    parameters->add("audios");

    parameters->add("chapters");
    parameters->add("subtitles");

    parameters->add("options");

    parameters->add("expiry", reply.expiry);

    parameters->add("next");

    parameters->add("reload");
}

void WBackendUniversalPrivate::applySourceResults(WBackendUniversalParameters * parameters,
                                                  WBackendNetSource           * reply) const
{
    reply->valid = parameters->value("valid")->toBool();

    applyQualities(&(reply->medias), parameters->value("medias"));
    applyQualities(&(reply->audios), parameters->value("audios"));

    applyChapters (&(reply->chapters),  parameters->value("chapters"));
    applySubtitles(&(reply->subtitles), parameters->value("subtitles"));

    reply->options = parameters->value("options")->toStringList();

    reply->expiry = getDate(*(parameters->value("expiry")));

    applyQueries(&(reply->nextQueries), parameters->value("next"));

    reply->backup = *(parameters->value("global"));

    reply->reload = parameters->value("reload")->toBool();
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::applyTrackParameters(WBackendUniversalParameters * parameters,
                                                    const QByteArray            & data,
                                                    const WBackendNetQuery      & query,
                                                    const WBackendNetTrack      & reply) const
{
    parameters->add("data", data);

    applyQueryParameters(parameters, query);

    parameters->add("valid", reply.valid);

    parameters->add("cache");

    parameters->add("next");

    parameters->add("reload");

    parameters->add("type");

    parameters->add("state");

    parameters->add("source");

    parameters->add("title");
    parameters->add("cover");

    parameters->add("author");
    parameters->add("feed");

    parameters->add("duration", reply.track.duration());

    parameters->add("date");
}

void WBackendUniversalPrivate::applyTrackResults(WBackendUniversalParameters * parameters,
                                                 WBackendNetTrack            * reply) const
{
    reply->valid = parameters->value("valid")->toBool();

    reply->cache = parameters->value("cache")->toByteArray();

    applyQueries(&(reply->nextQueries), parameters->value("next"));

    reply->backup = *(parameters->value("global"));

    reply->reload = parameters->value("reload")->toBool();

    WTrack & track = reply->track;

    track.setType(getTypeTrack(parameters->value("type")->toString()));

    track.setState(getStateTrack(parameters->value("state")->toString()));

    track.setSource(parameters->value("source")->toString());

    track.setTitle(parameters->value("title")->toString());
    track.setCover(parameters->value("cover")->toString());

    track.setAuthor(parameters->value("author")->toString());
    track.setFeed  (parameters->value("feed")  ->toString());

    track.setDuration(parameters->value("duration")->toInt());

    track.setDate(getDate(*(parameters->value("date"))));
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::applyPlaylistParameters(WBackendUniversalParameters * parameters,
                                                       const QByteArray            & data,
                                                       const WBackendNetQuery      & query,
                                                       const WBackendNetPlaylist   & reply) const
{
    parameters->add("data", data);

    applyQueryParameters(parameters, query);

    parameters->add("valid", reply.valid);

    parameters->add("cache");

    parameters->add("next");

    parameters->add("reload");

    parameters->add("title");
    parameters->add("cover");

    parameters->add("tracks");

    parameters->add("currentIndex", reply.currentIndex);

    parameters->add("clearDuplicate", reply.clearDuplicate);
}

void WBackendUniversalPrivate::applyPlaylistResults(WBackendUniversalParameters * parameters,
                                                    WBackendNetPlaylist         * reply) const
{
    reply->valid = parameters->value("valid")->toBool();

    reply->cache = parameters->value("cache")->toByteArray();

    applyQueries(&(reply->nextQueries), parameters->value("next"));

    reply->backup = *(parameters->value("global"));

    reply->reload = parameters->value("reload")->toBool();

    reply->title = parameters->value("title")->toString();
    reply->cover = parameters->value("cover")->toString();

    QVariantList list = parameters->value("tracks")->toList();

    for (int i = 0; i < list.count(); i++)
    {
        applyTrack(&(reply->tracks), list.at(i));
    }

    reply->currentIndex = parameters->value("currentIndex")->toInt();

    reply->clearDuplicate = parameters->value("clearDuplicate")->toBool();
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::applyFolderParameters(WBackendUniversalParameters * parameters,
                                                     const QByteArray            & data,
                                                     const WBackendNetQuery      & query,
                                                     const WBackendNetFolder     & reply) const
{
    parameters->add("data", data);

    applyQueryParameters(parameters, query);

    parameters->add("valid", reply.valid);

    parameters->add("cache");

    parameters->add("next");

    parameters->add("reload");

    parameters->add("title");
    parameters->add("cover");

    parameters->add("items");

    parameters->add("clearDuplicate", reply.clearDuplicate);
    parameters->add("scanItems",      reply.scanItems);
}

void WBackendUniversalPrivate::applyFolderResults(WBackendUniversalParameters * parameters,
                                                  WBackendNetFolder           * reply) const
{
    reply->valid = parameters->value("valid")->toBool();

    reply->cache = parameters->value("cache")->toByteArray();

    applyQueries(&(reply->nextQueries), parameters->value("next"));

    reply->backup = *(parameters->value("global"));

    reply->reload = parameters->value("reload")->toBool();

    reply->title = parameters->value("title")->toString();
    reply->cover = parameters->value("cover")->toString();

    QVariantList list = parameters->value("items")->toList();

    for (int i = 0; i < list.count(); i++)
    {
        applyItem(&(reply->items), list.at(i));
    }

    reply->clearDuplicate = parameters->value("clearDuplicate")->toBool();
    reply->scanItems      = parameters->value("scanItems")     ->toBool();
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::applyItemParameters(WBackendUniversalParameters * parameters,
                                                   const QByteArray            & data,
                                                   const WBackendNetQuery      & query,
                                                   const WBackendNetItem       & reply) const
{
    parameters->add("data", data);

    applyQueryParameters(parameters, query);

    parameters->add("valid", reply.valid);

    parameters->add("cache");

    parameters->add("next");

    parameters->add("reload");

    parameters->add("itemData");
    parameters->add("extension");
}

void WBackendUniversalPrivate::applyItemResults(WBackendUniversalParameters * parameters,
                                                WBackendNetItem             * reply) const
{
    reply->valid = parameters->value("valid")->toBool();

    reply->cache = parameters->value("cache")->toByteArray();

    applyQueries(&(reply->nextQueries), parameters->value("next"));

    reply->backup = *(parameters->value("global"));

    reply->reload = parameters->value("reload")->toBool();

    reply->data = parameters->value("itemData")->toByteArray();

    reply->extension = parameters->value("extension")->toString();
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::applyTrack(QList<WTrack>  * tracks,
                                          const QVariant & value) const
{
    QHash<QString, QVariant> hash = value.toHash();

    WTrack track;

    track.setType(getTypeTrack(hash.value("type").toString()));

    track.setState(getStateTrack(hash.value("state").toString()));

    track.setSource(hash.value("source").toString());

    track.setTitle(hash.value("title").toString());
    track.setCover(hash.value("cover").toString());

    track.setAuthor(hash.value("author").toString());
    track.setFeed  (hash.value("feed")  .toString());

    const QVariant * variant = getVariant(&hash, "duration");

    if (variant) track.setDuration(variant->toInt());

    track.setDate(getDate(hash.value("date")));

    tracks->append(track);
}

void WBackendUniversalPrivate::applyItem(QList<WLibraryFolderItem> * items,
                                         const QVariant            & value) const
{
    QHash<QString, QVariant> hash = value.toHash();

    WLibraryFolderItem item(getType (hash.value("type") .toString()),
                            getState(hash.value("state").toString()));

    item.source = hash.value("source").toString();

    item.title = hash.value("title").toString();
    item.cover = hash.value("cover").toString();

    item.label = hash.value("label").toString();

    items->append(item);
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::applyQueries(QList<WBackendNetQuery> * queries,
                                            QVariant * value) const
{
    // NOTE: If it's a list we extract each query. Otherwise it's a single query.
#ifdef QT_OLD
    if (value->type() == QVariant::List)
#else
    if (value->typeId() == QMetaType::QVariantList)
#endif
    {
        QVariantList list = value->toList();

        for (int i = 0; i < list.count(); i++)
        {
            const QVariant & variant = list.at(i);

            queries->append(getQuery(variant));

            if (i == BACKENDUNIVERSAL_MAX_QUERIES)
            {
                qWarning("WBackendUniversalPrivate::applyQueries: Maximum queries reached.");

                return;
            }
        }
    }
    else queries->append(getQuery(*value));
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate
     ::applyQualities(QHash<WAbstractBackend::Quality, QString> * qualities,
                      QVariant                                  * value) const
{
    QHashIterator<QString, QVariant> i(value->toHash());

    while (i.hasNext())
    {
        i.next();

        qualities->insert(getQuality(i.key()), i.value().toString());
    }
}

void WBackendUniversalPrivate::applyChapters(QList<WChapter> * chapters, QVariant * value) const
{
    QVariantList list = value->toList();

    for (int i = 0; i < list.count(); i++)
    {
        QHash<QString, QVariant> hash = list.at(i).toHash();

        int time = Sk::extractMsecs(hash.value("time").toString(), -1);

        if (time == -1) continue;

        WChapter chapter(time);

        chapter.setTitle(hash.value("title").toString());

        chapters->append(chapter);
    }
}

void WBackendUniversalPrivate::applySubtitles(QList<WSubtitle> * subtitles, QVariant * value) const
{
    QVariantList list = value->toList();

    for (int i = 0; i < list.count(); i++)
    {
        QHash<QString, QVariant> hash = list.at(i).toHash();

        QString source = hash.value("source").toString();

        if (source.isEmpty()) continue;

        WSubtitle subtitle(source, hash.value("title") .toString());

        subtitles->append(subtitle);
    }
}

//-------------------------------------------------------------------------------------------------

WBackendNetQuery WBackendUniversalPrivate::getQuery(const QVariant & value) const
{
    WBackendNetQuery query;

    QHash<QString, QVariant> hash = value.toHash();

    query.type = getTypeQuery(hash.value("type").toString());

    QString backend = hash.value("backend").toString();

    // NOTE: By default, we apply the current backend id.
    if (backend.isEmpty()) query.backend = id;
    else                   query.backend = backend;

    query.url = hash.value("url").toString();

    query.id = hash.value("id").toInt();

    query.data = hash.value("queryData");

    query.method = hash.value("queryMethod").toString();
    query.header = hash.value("header")     .toString();
    query.body   = hash.value("body")       .toString();

    const QVariant * variant = getVariant(&hash, "clearItems");

    if (variant) query.clearItems = variant->toBool();

    query.cookies   = hash.value("cookies")  .toBool();
    query.skipError = hash.value("skipError").toBool();

    query.delay = hash.value("delay").toInt();

    variant = getVariant(&hash, "timeout");

    if (variant) query.timeout = variant->toInt();

    return query;
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Quality WBackendUniversalPrivate::getQuality(const QString & string) const
{
    return WAbstractBackend::qualityFromString(string);
}

QDateTime WBackendUniversalPrivate::getDate(const QVariant & value) const
{
    qint64 seconds = value.toLongLong();

    return Sk::dateFromSecs(seconds);
}

//-------------------------------------------------------------------------------------------------

WLibraryItem::Type WBackendUniversalPrivate::getType(const QString & string) const
{
    if      (string == "folder")       return WLibraryItem::Folder;
    else if (string == "folderSearch") return WLibraryItem::FolderSearch;
    else if (string == "playlist")     return WLibraryItem::Playlist;
    else if (string == "feed")         return WLibraryItem::PlaylistFeed;
    else                               return WLibraryItem::Item;
}

WBackendNetQuery::Type WBackendUniversalPrivate::getTypeQuery(const QString & string) const
{
#ifdef SK_NO_TORRENT
    Q_UNUSED(string);

    return WBackendNetQuery::TypeDefault;
#else
    if (string == "torrent") return WBackendNetQuery::TypeTorrent;
    else                     return WBackendNetQuery::TypeDefault;
#endif
}

WTrack::Type WBackendUniversalPrivate::getTypeTrack(const QString & string) const
{
    return WTrack::typeFromString(string);
}

//-------------------------------------------------------------------------------------------------

WLocalObject::State WBackendUniversalPrivate::getState(const QString & string) const
{
    if (string == "default") return WLocalObject::Default;
    else                     return WLocalObject::Loaded;
}

WTrack::State WBackendUniversalPrivate::getStateTrack(const QString & string) const
{
    if      (string == "default") return WTrack::Default;
    else if (string == "cover")   return WTrack::LoadCover;
    else                          return WTrack::Loaded;
}

//-------------------------------------------------------------------------------------------------

const QVariant * WBackendUniversalPrivate::getVariant(const QHash<QString, QVariant> * hash,
                                                      const QString                  & name) const
{
    QHash<QString, QVariant>::const_iterator i = hash->find(name);

    if (i == hash->end())
    {
         return NULL;
    }
    else return &(i.value());
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::onLoad()
{
    Q_Q(WBackendUniversal);

    QByteArray array = remote->readAll();

    QObject::disconnect(remote, 0, q, 0);

    remote->deleteLater();

    remote = NULL;

    if (array.isEmpty())
    {
        data = WBackendUniversalData();

        applyLoaded();

        return;
    }

    loadData(array);
}

void WBackendUniversalPrivate::onUpdate()
{
    Q_Q(WBackendUniversal);

    QObject::disconnect(remote, 0, q, 0);

    if (remote->hasError())
    {
        remote->deleteLater();

        remote = NULL;

        applyLoaded();

        return;
    }

    QByteArray array = remote->readAll();

    remote->deleteLater();

    remote = NULL;

    if (array.isEmpty())
    {
        applyLoaded();

        return;
    }

    loadData(array);

    wControllerFile->startWriteFile(WControllerFile::filePath(source), array);
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::onData(const WBackendUniversalData & data)
{
    Q_Q(WBackendUniversal);

    QString version = this->data.version;

    if (version.isEmpty())
    {
        this->data = data;

        applyLoaded();

        return;
    }

    // NOTE: If the required API version is too high we keep our previous data.
    if (data.valid == false)
    {
        qWarning("WBackendUniversalPrivate::onData: Cannot update, the required API is too high.");

        applyLoaded();

        return;
    }

    this->data = data;

    applyLoaded();

    if (version != data.version)
    {
        emit q->updated();
    }
}

//=================================================================================================
// WBackendUniversal
//=================================================================================================
// Protected

WBackendUniversal::WBackendUniversal(const QString & id, const QString & source)
    : WBackendNet(new WBackendUniversalPrivate(this))
{
    Q_D(WBackendUniversal); d->init(id, source);
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ void WBackendUniversal::clearCache()
{
    if (cache) cache->clear();
}

//-------------------------------------------------------------------------------------------------
// WBackendNet implementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendUniversal::getId() const
{
    Q_D(const WBackendUniversal); return d->id;
}

/* Q_INVOKABLE virtual */ QString WBackendUniversal::getTitle() const
{
    Q_D(const WBackendUniversal); return d->data.title;
}

//-------------------------------------------------------------------------------------------------
// WBackendNet reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WBackendUniversal::update()
{
    Q_D(WBackendUniversal);

    QString origin = d->data.origin;

    if (origin.isEmpty())
    {
        qWarning("WBackendUniversal::update: 'origin' is empty for [%s].", d->id.C_STR);

        return;
    }

    cache->removeScripts(d->id);

    if (d->remote) delete d->remote;

    d->remote = wControllerDownload->getData(origin, BACKENDUNIVERSAL_TIMEOUT_LOAD);

    connect(d->remote, SIGNAL(loaded(WRemoteData *)), this, SLOT(onUpdate()));
}

/* Q_INVOKABLE virtual */ void WBackendUniversal::reload()
{
    Q_D(WBackendUniversal);

    cache->removeScripts(d->id);

    if (d->remote) delete d->remote;

    d->load();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendUniversal::hasSearch() const
{
    Q_D(const WBackendUniversal);

    if (d->data.items.isEmpty())
    {
         return false;
    }
    else return true;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendUniversal::isSearchEngine() const
{
    Q_D(const WBackendUniversal); return d->data.isSearchEngine;
}

/* Q_INVOKABLE virtual */ bool WBackendUniversal::isSearchCover() const
{
    Q_D(const WBackendUniversal); return d->data.isSearchCover;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendUniversal::validate() const
{
    Q_D(const WBackendUniversal); return d->data.validate;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendUniversal::checkCover(const QString &, const QString & q) const
{
    Q_D(const WBackendUniversal);

    if (d->data.isSearchCover == false)
    {
        return false;
    }

    QString extension = WControllerNetwork::extractUrlExtension(q);

    if (d->data.engines.testFlag(WBackendUniversalData::CoverAudio))
    {
        return WControllerPlaylist::extensionIsAudio(extension);
    }
    else // WBackendUniversalData::CoverVideo
    {
        return WControllerPlaylist::extensionIsVideo(extension);
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendUniversal::getHost() const
{
    Q_D(const WBackendUniversal); return d->data.host;
}

/* Q_INVOKABLE virtual */ QString WBackendUniversal::getCover() const
{
    Q_D(const WBackendUniversal); return d->data.cover;
}

/* Q_INVOKABLE virtual */ QString WBackendUniversal::getHub() const
{
    Q_D(const WBackendUniversal); return d->data.hub;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QList<WLibraryFolderItem> WBackendUniversal::getLibraryItems() const
{
    Q_D(const WBackendUniversal); return d->data.items;
}

/* Q_INVOKABLE virtual */
WLibraryFolderItem WBackendUniversal::getLibraryItem(const QString & id) const
{
    Q_D(const WBackendUniversal);

    foreach (const WLibraryFolderItem & item, d->data.items)
    {
        // NOTE: By default the label is the lowercased title.
        if (item.label == id) return item;
    }

    return WLibraryFolderItem();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
QString WBackendUniversal::getTrackId(const QString & url) const
{
    Q_D(const WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION getTrackId");
#endif

    WBackendUniversalScript * script = cache->getScript(d->id + ".trackId",
                                                        d->data.trackId);

    if (script == NULL) return QString();

    WBackendUniversalParameters parameters(*script, d->global);

    parameters.add("url", url);

    return script->run(&parameters).toString();
}

/* Q_INVOKABLE virtual */
WAbstractBackend::Output WBackendUniversal::getTrackOutput(const QString & url) const
{
    Q_D(const WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION getTrackOutput");
#endif

    WBackendUniversalScript * script = cache->getScript(d->id + ".trackOutput",
                                                        d->data.trackOutput);

    if (script == NULL) return WAbstractBackend::OutputMedia;

    WBackendUniversalParameters parameters(*script, d->global);

    parameters.add("url", url);

    QString output = script->run(&parameters).toString();

    if (output == "video")
    {
        return WAbstractBackend::OutputVideo;
    }
    else if (output == "audio")
    {
        return WAbstractBackend::OutputAudio;
    }
    else return WAbstractBackend::OutputMedia;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetPlaylistInfo WBackendUniversal::getPlaylistInfo(const QString & url) const
{
    Q_D(const WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION getPlaylistInfo");
#endif

    WBackendUniversalScript * script = cache->getScript(d->id + ".playlistInfo",
                                                        d->data.playlistInfo);

    if (script == NULL) return WBackendNetPlaylistInfo();

    WBackendUniversalParameters parameters(*script, d->global);

    parameters.add("type");
    parameters.add("id");

    parameters.add("url", url);

    if (script->run(&parameters).toBool())
    {
        WBackendNetPlaylistInfo info;

        QString type = parameters.value("type")->toString();

        if (type == "feed")
        {
             info.type = WLibraryItem::PlaylistFeed;
        }
        else info.type = WLibraryItem::Playlist;

        info.id = parameters.value("id")->toString();

        return info;
    }
    else return WBackendNetPlaylistInfo();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
QString WBackendUniversal::getUrlTrack(const QString & id) const
{
    Q_D(const WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION getUrlTrack");
#endif

    WBackendUniversalScript * script = cache->getScript(d->id + ".urlTrack",
                                                        d->data.urlTrack);

    if (script == NULL) return QString();

    WBackendUniversalParameters parameters(*script, d->global);

    parameters.add("id", id);

    return script->run(&parameters).toString();
}

/* Q_INVOKABLE virtual */
QString WBackendUniversal::getUrlPlaylist(const WBackendNetPlaylistInfo & info) const
{
    Q_D(const WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION getUrlPlaylist");
#endif

    WBackendUniversalScript * script = cache->getScript(d->id + ".urlPlaylist",
                                                        d->data.urlPlaylist);

    if (script == NULL) return QString();

    WBackendUniversalParameters parameters(*script, d->global);

    if (info.type == WLibraryItem::PlaylistFeed)
    {
         parameters.add("type", "feed");
    }
    else parameters.add("type", "playlist");

    parameters.add("id", info.id);

    return script->run(&parameters).toString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendUniversal::getQuerySource(const QString              & url,
                                                   WAbstractBackend::SourceMode mode) const
{
    Q_D(const WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION getQuerySource");
#endif

    WBackendNetQuery query;

    query.mode = mode;

    d->runQuery(&query, ".querySource", d->data.querySource, url);

    return query;
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendUniversal::getQueryTrack(const QString & url) const
{
    Q_D(const WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION getQueryTrack");
#endif

    WBackendNetQuery query;

    d->runQuery(&query, ".queryTrack", d->data.queryTrack, url);

    return query;
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendUniversal::getQueryPlaylist(const QString & url) const
{
    Q_D(const WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION getQueryPlaylist");
#endif

    WBackendNetQuery query;

    d->runQuery(&query, ".queryPlaylist", d->data.queryPlaylist, url);

    return query;
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendUniversal::getQueryFolder(const QString & url) const
{
    Q_D(const WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION getQueryFolder");
#endif

    WBackendNetQuery query;

    d->runQuery(&query, ".queryFolder", d->data.queryFolder, url);

    return query;
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendUniversal::getQueryItem(const QString & url) const
{
    Q_D(const WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION getQueryItem");
#endif

    WBackendNetQuery query;

    d->runQuery(&query, ".queryItem", d->data.queryItem, url);

    return query;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendUniversal::createQuery(const QString & method, const QString & label,
                                                const QString & q,      const QString & t) const
{
    Q_D(const WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION createQuery");
#endif

    WBackendUniversalScript * script = cache->getScript(d->id + ".createQuery",
                                                        d->data.createQuery);

    if (script == NULL) return WBackendNetQuery();

    WBackendUniversalParameters parameters(*script, d->global);

    WBackendNetQuery query;

    d->applyQueryParameters(&parameters, query);

    parameters.add("method", method);
    parameters.add("label",  label);
    parameters.add("q",      q);
    parameters.add("t",      t);

    script->run(&parameters);

    d->applyQueryResults(&parameters, &query);

    return query;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetSource WBackendUniversal::extractSource(const QByteArray       & data,
                                                   const WBackendNetQuery & query) const
{
    Q_D(const WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION extractSource");
#endif

    WBackendUniversalScript * script = cache->getScript(d->id + ".extractSource",
                                                        d->data.extractSource);

    if (script == NULL) return WBackendNetSource();

    WBackendUniversalParameters parameters(*script, d->global);

    WBackendNetSource reply;

    d->applySourceParameters(&parameters, data, query, reply);

    script->run(&parameters);

    d->applySourceResults(&parameters, &reply);

    return reply;
}

/* Q_INVOKABLE virtual */
WBackendNetTrack WBackendUniversal::extractTrack(const QByteArray       & data,
                                                 const WBackendNetQuery & query) const
{
    Q_D(const WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION extractTrack");
#endif

    WBackendUniversalScript * script = cache->getScript(d->id + ".extractTrack",
                                                        d->data.extractTrack);

    if (script == NULL) return WBackendNetTrack();

    WBackendUniversalParameters parameters(*script, d->global);

    WBackendNetTrack reply;

    d->applyTrackParameters(&parameters, data, query, reply);

    script->run(&parameters);

    d->applyTrackResults(&parameters, &reply);

    return reply;
}

/* Q_INVOKABLE virtual */
WBackendNetPlaylist WBackendUniversal::extractPlaylist(const QByteArray       & data,
                                                       const WBackendNetQuery & query) const
{
    Q_D(const WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION extractPlaylist");
#endif

    WBackendUniversalScript * script = cache->getScript(d->id + ".extractPlaylist",
                                                        d->data.extractPlaylist);

    if (script == NULL) return WBackendNetPlaylist();

    WBackendUniversalParameters parameters(*script, d->global);

    WBackendNetPlaylist reply;

    d->applyPlaylistParameters(&parameters, data, query, reply);

    script->run(&parameters);

    d->applyPlaylistResults(&parameters, &reply);

    return reply;
}

/* Q_INVOKABLE virtual */
WBackendNetFolder WBackendUniversal::extractFolder(const QByteArray       & data,
                                                   const WBackendNetQuery & query) const
{
    Q_D(const WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION extractFolder");
#endif

    WBackendUniversalScript * script = cache->getScript(d->id + ".extractFolder",
                                                        d->data.extractFolder);

    if (script == NULL) return WBackendNetFolder();

    WBackendUniversalParameters parameters(*script, d->global);

    WBackendNetFolder reply;

    d->applyFolderParameters(&parameters, data, query, reply);

    script->run(&parameters);

    d->applyFolderResults(&parameters, &reply);

    return reply;
}

/* Q_INVOKABLE virtual */
WBackendNetItem WBackendUniversal::extractItem(const QByteArray       & data,
                                               const WBackendNetQuery & query) const
{
    Q_D(const WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION extractItem");
#endif

    WBackendUniversalScript * script = cache->getScript(d->id + ".extractItem",
                                                        d->data.extractItem);

    if (script == NULL) return WBackendNetItem();

    WBackendUniversalParameters parameters(*script, d->global);

    WBackendNetItem reply;

    d->applyItemParameters(&parameters, data, query, reply);

    script->run(&parameters);

    d->applyItemResults(&parameters, &reply);

    return reply;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WBackendUniversal::queryFailed(const WBackendNetQuery & query)
{
    Q_D(const WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION queryFailed");
#endif

    WBackendUniversalScript * script = cache->getScript(d->id + ".queryFailed",
                                                        d->data.queryFailed);

    if (script == NULL) return;

    WBackendUniversalParameters parameters(*script, d->global);

    d->applyQueryParameters(&parameters, query);

    script->run(&parameters);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
void WBackendUniversal::applySource(const WBackendNetQuery  &,
                                    const WBackendNetSource & source)
{
    Q_D(WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION applySource");
#endif

    d->global = source.backup;
}

/* Q_INVOKABLE virtual */
void WBackendUniversal::applyTrack(const WBackendNetQuery &,
                                   const WBackendNetTrack & track)
{
    Q_D(WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION applyTrack");
#endif

    d->global = track.backup;
}

/* Q_INVOKABLE virtual */
void WBackendUniversal::applyPlaylist(const WBackendNetQuery    &,
                                      const WBackendNetPlaylist & playlist)
{
    Q_D(WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION applyPlaylist");
#endif

    d->global = playlist.backup;
}

/* Q_INVOKABLE virtual */
void WBackendUniversal::applyFolder(const WBackendNetQuery  &,
                                    const WBackendNetFolder & folder)
{
    Q_D(WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION applyFolder");
#endif

    d->global = folder.backup;
}

/* Q_INVOKABLE virtual */
void WBackendUniversal::applyItem(const WBackendNetQuery &,
                                  const WBackendNetItem  & item)
{
    Q_D(WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION applyItem");
#endif

    d->global = item.backup;
}

#endif // SK_NO_BACKENDUNIVERSAL

#include "WBackendUniversal.moc"
