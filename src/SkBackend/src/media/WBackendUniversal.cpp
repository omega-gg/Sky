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

#include "WBackendUniversal.h"

#ifndef SK_NO_BACKENDUNIVERSAL

// Qt includes
#ifdef QT_LATEST
#include <QUrlQuery>
#endif

// Sk includes
#include <WControllerApplication>
#include <WControllerNetwork>
#include <WControllerDownload>
#include <WCache>
#include <WBackendCache>
#include <WYamlReader>

//-------------------------------------------------------------------------------------------------
// Static variables

static const QString BACKENDUNIVERSAL_FUNCTIONS = "IF|AND|OR|ELSE|ELIF|FI|FOREACH|CONTINUE|BREAK|"
                                                  "END|RETURN|EQUAL|NOT_EQUAL|LESSER|GREATER|"
                                                  "LESSER_EQUAL|GREATER_EQUAL|NUMBER|LIST|TIME|"
                                                  "DATE|ADD|SUB|MULTIPLY|GET_CHAR|GET_LIST|"
                                                  "GET_HASH|SET|SET_HASH|PREPEND|APPEND|"
                                                  "APPEND_LIST|REMOVE_CHAR|REMOVE_LIST|CHOP|"
                                                  "REPLACE|REPLACE_REGEXP|MID|SPLIT|JOIN|LOWER|"
                                                  "SIMPLIFY|TAKE|READ|LATIN_TO_UTF8|LENGTH|COUNT|"
                                                  "COUNT_STRING|INDEX_OF|INDEX_REGEXP|INDEX_END|"
                                                  "INDEX_REGEXP_END|LAST_INDEX_OF|"
                                                  "LAST_INDEX_REGEXP|LAST_INDEX_END|"
                                                  "LAST_INDEX_REGEXP_END|CONTAIN|CONTAIN_REGEXP|"
                                                  "CONTAIN_LIST|START_WITH|START_WITH_REGEXP|"
                                                  "SLICE|SLICE_IN|SLICES|ADD_QUERY|DECODE_URL|"
                                                  "URL_NAME|URL_FRAGMENT|EXTENSION_IS_MEDIA|"
                                                  "EXTENSION_IS_VIDEO|EXTENSION_IS_AUDIO|"
                                                  "EXTRACT_URL_PATH|EXTRACT_URL_ELEMENT|"
                                                  "EXTRACT_URL_ELEMENTS|EXTRACT_URL_EXTENSION|"
                                                  "REMOVE_URL_PREFIX|EXTRACT_HTML|"
                                                  "EXTRACT_ATTRIBUTE|EXTRACT_ATTRIBUTE_AT|"
                                                  "EXTRACT_ATTRIBUTE_UTF8|"
                                                  "EXTRACT_ATTRIBUTE_UTF8_AT|EXTRACT_JSON|"
                                                  "EXTRACT_JSON_UTF8|EXTRACT_JSON_HTML|SPLIT_JSON|"
                                                  "TORRENT_STRING_AFTER|TORRENT_INTEGER_AFTER|"
                                                  "TORRENT_LIST_AFTER|TORRENT_ITEMS|"
                                                  "TORRENT_FOLDERS|PRINT";

static const int BACKENDUNIVERSAL_MAX_NODES = 200;

static const int BACKENDUNIVERSAL_MAX_CONDITIONS = 1;
static const int BACKENDUNIVERSAL_MAX_LOOPS      = 1;

static const int BACKENDUNIVERSAL_MAX_LOOP = 1000;

//-------------------------------------------------------------------------------------------------

typedef QVariant (*function)(const WBackendUniversalNode *, WBackendUniversalParameters *);

static QHash<QString, function> hash;

static WBackendCache * cache = NULL;

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

    float valueA = node->getFloat(parameters, 0);
    float valueB = node->getFloat(parameters, 1);

    return (valueA < valueB);
}

inline QVariant greater(const WBackendUniversalNode * node,
                        WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("GREATER");
#endif

    if (node->nodes.count() < 2) return false;

    float valueA = node->getFloat(parameters, 0);
    float valueB = node->getFloat(parameters, 1);

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

    float valueA = node->getFloat(parameters, 0);
    float valueB = node->getFloat(parameters, 1);

    return (valueA <= valueB);
}

inline QVariant greaterEqual(const WBackendUniversalNode * node,
                             WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("GREATER_EQUAL");
#endif

    if (node->nodes.count() < 2) return false;

    float valueA = node->getFloat(parameters, 0);
    float valueB = node->getFloat(parameters, 1);

    return (valueA >= valueB);
}

//-------------------------------------------------------------------------------------------------

inline QVariant number(const WBackendUniversalNode * node,
                       WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("NUMBER");
#endif

    if (node->nodes.count() < 1) return 0;

    return node->getFloat(parameters, 0);
}

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

inline QVariant time(const WBackendUniversalNode * node,
                     WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("MSEC");
#endif

    if (node->nodes.count() < 2) return QTime();

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

    if (node->nodes.count() < 2) return QDateTime();

    QDateTime date = QDateTime::fromString(node->getString(parameters, 0),
                                           node->getString(parameters, 1));

#ifdef QT_4
    return date.toMSecsSinceEpoch() / 1000;
#else
    return date.toSecsSinceEpoch();
#endif
}

//-------------------------------------------------------------------------------------------------

inline QVariant add(const WBackendUniversalNode * node,
                    WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("ADD");
#endif

    if (node->nodes.count() < 2) return false;

    float valueA = node->getFloat(parameters, 0);
    float valueB = node->getFloat(parameters, 1);

    return (valueA + valueB);
}

inline QVariant sub(const WBackendUniversalNode * node,
                    WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("SUB");
#endif

    if (node->nodes.count() < 2) return false;

    float valueA = node->getFloat(parameters, 0);
    float valueB = node->getFloat(parameters, 1);

    return (valueA - valueB);
}

inline QVariant multiply(const WBackendUniversalNode * node,
                         WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("MULTIPLY");
#endif

    if (node->nodes.count() < 2) return false;

    float valueA = node->getFloat(parameters, 0);
    float valueB = node->getFloat(parameters, 1);

    return (valueA * valueB);
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

inline QVariant getList(const WBackendUniversalNode * node,
                        WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("GET_LIST");
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

inline QVariant getHash(const WBackendUniversalNode * node,
                        WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("GET_HASH");
#endif

    if (node->nodes.count() < 2) return QVariant();

    QHash<QString, QVariant> hash = node->getHash(parameters, 0);

    return hash.value(node->getString(parameters, 1));
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

    QVariant * key = node->getValue(parameters, 0);

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

inline QVariant setHash(const WBackendUniversalNode * node,
                        WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("SET_HASH");
#endif

    int count = node->nodes.count();

    if (count < 3) return false;

    QVariant * key = node->getValue(parameters, 0);

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

//-------------------------------------------------------------------------------------------------

inline QVariant prepend(const WBackendUniversalNode * node,
                        WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("PREPEND");
#endif

    int count = node->nodes.count();

    if (count < 2) return false;

    QString string;

    QVariant * key = node->getValue(parameters, 0);

    if (key)
    {
        string = key->toString();

        for (int i = 1; i < count; i++)
        {
            string.prepend(node->getString(parameters, i));
        }

        *key = string;
    }
    else
    {
        string = node->getString(parameters, 0);

        for (int i = 1; i < count; i++)
        {
            string.prepend(node->getString(parameters, i));
        }
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

    QString string;

    QVariant * key = node->getValue(parameters, 0);

    if (key)
    {
        string = key->toString();

        for (int i = 1; i < count; i++)
        {
            string.append(node->getString(parameters, i));
        }

        *key = string;
    }
    else
    {
        string = node->getString(parameters, 0);

        for (int i = 1; i < count; i++)
        {
            string.append(node->getString(parameters, i));
        }
    }

    return string;
}

inline QVariant appendList(const WBackendUniversalNode * node,
                           WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("APPEND_LIST");
#endif

    if (node->nodes.count() < 2) return false;

    QVariant * key = node->getValue(parameters, 0);

    if (key == NULL) return false;

    QVariantList list = key->toList();

    list.append(*(node->getValue(parameters, 1)));

    *key = list;

    return true;
}

//-------------------------------------------------------------------------------------------------

inline QVariant removeChar(const WBackendUniversalNode * node,
                           WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("REMOVE_CHAR");
#endif

    int count = node->nodes.count();

    if (count < 2) return QString();

    QVariant * key = node->getValue(parameters, 0);

    if (key == NULL) return QString();

    if (count == 2)
    {
         *key = key->toString().remove(node->getInt(parameters, 1));
    }
    else *key = key->toString().remove(node->getInt(parameters, 1), node->getInt(parameters, 2));

    return *key;
}

inline QVariant removeList(const WBackendUniversalNode * node,
                           WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("REMOVE_LIST");
#endif

    if (node->nodes.count() < 2) return false;

    QVariant * key = node->getValue(parameters, 0);

    if (key == NULL) return false;

    QVariantList list = key->toList();

    list.removeOne(node->getInt(parameters, 1));

    *key = list;

    return true;
}

//-------------------------------------------------------------------------------------------------

inline QVariant chop(const WBackendUniversalNode * node, WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("CHOP");
#endif

    if (node->nodes.count() < 2) return QString();

    QVariant * key = node->getValue(parameters, 0);

    if (key == NULL) return QString();

    QString string = key->toString();

    string.chop(node->getInt(parameters, 1));

    *key = string;

    return *key;
}

//-------------------------------------------------------------------------------------------------

inline QVariant replace(const WBackendUniversalNode * node,
                        WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("REPLACE");
#endif

    if (node->nodes.count() < 3) return QString();

    QVariant * key = node->getValue(parameters, 0);

    if (key == NULL) return QString();

    *key = key->toString().replace(node->getString(parameters, 1), node->getString(parameters, 2));

    return *key;
}

inline QVariant replaceRegExp(const WBackendUniversalNode * node,
                              WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("REPLACE_REGEXP");
#endif

    if (node->nodes.count() < 3) return QString();

    QVariant * key = node->getValue(parameters, 0);

    if (key == NULL) return QString();

    *key = key->toString().replace(node->getRegExp(parameters, 1), node->getString(parameters, 2));

    return *key;
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

    return string.split(node->getInt(parameters, 1));
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

inline QVariant take(const WBackendUniversalNode * node,
                     WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("TAKE");
#endif

    if (node->nodes.count() < 2) return QVariant();

    QVariant * key = node->getValue(parameters, 0);

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

    return node->getList(parameters, 0).count();
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

    if (count == 2)
    {
         return string.indexOf(node->getString(parameters, 1));
    }
    else return string.indexOf(node->getString(parameters, 1), node->getInt(parameters, 2));
}

inline QVariant indexRegExp(const WBackendUniversalNode * node,
                            WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("INDEX_REGEXP");
#endif

    int count = node->nodes.count();

    if (count < 2) return -1;

    QString string = node->getString(parameters, 0);

    QRegExp regExp = node->getRegExp(parameters, 1);

    if (count == 2)
    {
        return string.indexOf(regExp);
    }
    else if (count > 2)
    {
        return string.indexOf(regExp, node->getInt(parameters, 2));
    }

    return string.indexOf(regExp);
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
    QString stringB = node->getString(parameters, 1);

    int index;

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

inline QVariant indexRegExpEnd(const WBackendUniversalNode * node,
                               WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("INDEX_REGEXP_END");
#endif

    int count = node->nodes.count();

    if (count < 2) return -1;

    QString string = node->getString(parameters, 0);

    QRegExp regExp = node->getRegExp(parameters, 1);

    int index;

    if (count == 2)
    {
         index = string.indexOf(regExp);
    }
    else index = string.indexOf(regExp, node->getInt(parameters, 2));

    if (index == -1)
    {
         return -1;
    }
    else return index + regExp.matchedLength();
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

    if (count == 2)
    {
         return string.lastIndexOf(node->getString(parameters, 1));
    }
    else return string.lastIndexOf(node->getString(parameters, 1), node->getInt(parameters, 2));
}

inline QVariant lastIndexRegExp(const WBackendUniversalNode * node,
                                WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("LAST_INDEX_REGEXP");
#endif

    int count = node->nodes.count();

    if (count < 2) return -1;

    QString string = node->getString(parameters, 0);

    QRegExp regExp = node->getRegExp(parameters, 1);

    if (count == 2)
    {
        return string.indexOf(regExp);
    }
    else if (count > 2)
    {
        return string.lastIndexOf(regExp, node->getInt(parameters, 2));
    }

    return string.lastIndexOf(regExp);
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
    QString stringB = node->getString(parameters, 1);

    int index;

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

inline QVariant lastIndexRegExpEnd(const WBackendUniversalNode * node,
                                   WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("LAST_INDEX_REGEXP_END");
#endif

    int count = node->nodes.count();

    if (count < 2) return -1;

    QString string = node->getString(parameters, 0);

    QRegExp regExp = node->getRegExp(parameters, 1);

    int index;

    if (count == 2)
    {
         index = string.lastIndexOf(regExp);
    }
    else index = string.lastIndexOf(regExp, node->getInt(parameters, 2));

    if (index == -1)
    {
         return -1;
    }
    else return index + regExp.matchedLength();
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

#ifdef QT_4
    // FIXME Qt4: The code does not compile without the 'QVariant'.
    return QVariant(string.contains(node->getString(parameters, 1)));
#else
    return string.contains(node->getString(parameters, 1));
#endif
}

inline QVariant containRegExp(const WBackendUniversalNode * node,
                              WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("CONTAIN_REGEXP");
#endif

    if (node->nodes.count() < 2) return false;

    QString string = node->getString(parameters, 0);

#ifdef QT_4
    // FIXME Qt4: The code does not compile without the 'QVariant'.
    return QVariant(string.contains(node->getRegExp(parameters, 1)));
#else
    return string.contains(node->getRegExp(parameters, 1));
#endif
}

inline QVariant containList(const WBackendUniversalNode * node,
                            WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("CONTAIN_LIST");
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

inline QVariant startWith(const WBackendUniversalNode * node,
                          WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("START_WITH");
#endif

    if (node->nodes.count() < 2) return false;

    QString string = node->getString(parameters, 0);

    return string.startsWith(node->getString(parameters, 1));
}

inline QVariant startWithRegExp(const WBackendUniversalNode * node,
                                WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("START_WITH_REGEXP");
#endif

    if (node->nodes.count() < 2) return false;

    QString string = node->getString(parameters, 0);

    QRegExp regExp = node->getRegExp(parameters, 1);

    if (string.indexOf(regExp) == 0)
    {
         return true;
    }
    else return false;
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

    if (count == 3)
    {
         return Sk::slice(node->getString(parameters, 0), node->getString(parameters, 1),
                                                          node->getString(parameters, 2));
    }
    else return Sk::slice(node->getString(parameters, 0), node->getString(parameters, 1),
                                                          node->getString(parameters, 2),
                                                          node->getInt(parameters, 3));
}

inline QVariant sliceIn(const WBackendUniversalNode * node,
                        WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("SLICE_IN");
#endif

    int count = node->nodes.count();

    if (count < 3) return QString();

    if (count == 3)
    {
         return Sk::sliceIn(node->getString(parameters, 0), node->getString(parameters, 1),
                                                            node->getString(parameters, 2));
    }
    else return Sk::sliceIn(node->getString(parameters, 0), node->getString(parameters, 1),
                                                            node->getString(parameters, 2),
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

    if (count < 3) return QString();

    if (count == 3)
    {
         return node->variants(Sk::slices(node->getString(parameters, 0),
                                          node->getString(parameters, 1),
                                          node->getString(parameters, 2)));
    }
    else return node->variants(Sk::slices(node->getString(parameters, 0),
                                          node->getString(parameters, 1),
                                          node->getString(parameters, 2),
                                          node->getInt(parameters, 3)));
}

//-------------------------------------------------------------------------------------------------

inline QVariant addQuery(const WBackendUniversalNode * node,
                         WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("ADD_QUERY");
#endif

    if (node->nodes.count() < 3) return QString();

    QVariant * key = node->getValue(parameters, 0);

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

inline QVariant decodeUrl(const WBackendUniversalNode * node,
                          WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("DECODE_URL");
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

//-------------------------------------------------------------------------------------------------

inline QVariant extractUrlPath(const WBackendUniversalNode * node,
                               WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("EXTRACT_URL_PATH");
#endif

    if (node->nodes.count() < 1) return QString();

    return WControllerNetwork::extractUrlPath(node->getString(parameters, 0));
}

inline QVariant extractUrlElement(const WBackendUniversalNode * node,
                                  WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("EXTRACT_URL_ELEMENT");
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

inline QVariant extractUrlElements(const WBackendUniversalNode * node,
                                   WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("EXTRACT_URL_ELEMENTS");
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

inline QVariant extractUrlExtension(const WBackendUniversalNode * node,
                                    WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("EXTRACT_URL_EXTENSION");
#endif

    if (node->nodes.count() < 1) return QString();

    return WControllerNetwork::extractUrlExtension(node->getString(parameters, 0));
}

//-------------------------------------------------------------------------------------------------

inline QVariant removeUrlPrefix(const WBackendUniversalNode * node,
                                WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("REMOVE_URL_PREFIX");
#endif

    if (node->nodes.count() < 1) return QString();

    QVariant * key = node->getValue(parameters, 0);

    if (key == NULL) return QString();

    *key = WControllerNetwork::removeUrlPrefix(key->toString());

    return *key;
}

//-------------------------------------------------------------------------------------------------

inline QVariant extractHtml(const WBackendUniversalNode * node,
                            WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("EXTRACT_HTML");
#endif

    int count = node->nodes.count();

    if (count < 2) return QVariant();

    WControllerPlaylistData data;

    if (count != 2)
    {
        QStringList list = node->getStringList(parameters, 2);

        foreach (const QString & string, list)
        {
            int index = string.indexOf('|');

            if (index == -1)
            {
                 data.addSlice(string);
            }
            else data.addSlice(string.mid(0, index), string.mid(index + 1));
        }
    }

    data.applyHtml(node->getByteArray(parameters, 0), node->getString(parameters, 1));

    return node->getHtml(data);
}

//-------------------------------------------------------------------------------------------------

inline QVariant extractAttribute(const WBackendUniversalNode * node,
                                 WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("EXTRACT_ATTRIBUTE");
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

inline QVariant extractAttributeAt(const WBackendUniversalNode * node,
                                   WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("EXTRACT_ATTRIBUTE_AT");
#endif

    if (node->nodes.count() < 2) return QString();

    return WControllerNetwork::extractAttributeAt(node->getString(parameters, 0),
                                                  node->getInt(parameters, 1));
}

inline QVariant extractAttributeUtf8(const WBackendUniversalNode * node,
                                     WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("EXTRACT_ATTRIBUTE_UTF8");
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

inline QVariant extractAttributeUtf8At(const WBackendUniversalNode * node,
                                       WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("EXTRACT_ATTRIBUTE_UTF8_AT");
#endif

    if (node->nodes.count() < 2) return QString();

    return WControllerNetwork::extractAttributeUtf8At(node->getString(parameters, 0),
                                                      node->getInt(parameters, 1));
}

//-------------------------------------------------------------------------------------------------

inline QVariant extractJson(const WBackendUniversalNode * node,
                            WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("EXTRACT_JSON");
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

inline QVariant extractJsonUtf8(const WBackendUniversalNode * node,
                                WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("EXTRACT_JSON_UTF8");
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

inline QVariant extractJsonHtml(const WBackendUniversalNode * node,
                                WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("EXTRACT_JSON_HTML");
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

inline QVariant splitJson(const WBackendUniversalNode * node,
                          WBackendUniversalParameters * parameters)
{
#ifdef SK_BACKEND_LOG
    qDebug("SPLIT_JSON");
#endif

    if (node->nodes.count() == 1)
    {
         return node->variants(WControllerNetwork::splitJson(node->getString(parameters, 0)));
    }
    else return node->variants(WControllerNetwork::splitJson(node->getString(parameters, 0),
                                                             node->getInt(parameters, 1)));
}

//-------------------------------------------------------------------------------------------------

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
         return WControllerTorrent::stringAfter(node->getString(parameters, 0),
                                                node->getString(parameters, 1));
    }
    else return WControllerTorrent::stringAfter(node->getString(parameters, 0),
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
         return WControllerTorrent::integerAfter(node->getString(parameters, 0),
                                                 node->getString(parameters, 1));
    }
    else return WControllerTorrent::integerAfter(node->getString(parameters, 0),
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
         return WControllerTorrent::listAfter(node->getString(parameters, 0),
                                              node->getString(parameters, 1));
    }
    else return WControllerTorrent::listAfter(node->getString(parameters, 0),
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

    QList<WTorrentItemData> items = WControllerTorrent::torrentItems(data);

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

    QList<WTorrentItemFolder> folder = WControllerTorrent::torrentFolders(items);

    return node->getTorrentFolders(folder);
}

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

//=================================================================================================
// WBackendUniversalLoop
//=================================================================================================

struct WBackendUniversalLoop
{
    int index;

    QStringList list;

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
    bool extractBool(const WYamlReader & reader, const QString & key) const;

    QString extractString(const WYamlReader & reader, const QString & key) const;

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

    QString content = Sk::readUtf8(array);

    content.remove('\r');

    content.replace('\t', ' ');

    WYamlReader reader(content.toUtf8());

    reader.dump();

    data.api     = extractString(reader, "api");
    data.version = extractString(reader, "version");

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

    data.title = extractString(reader, "title");
    data.host  = extractString(reader, "host");

    data.items = extractItems(reader);

    data.validate = extractString(reader, "validate");

    //---------------------------------------------------------------------------------------------

    data.trackId     = extractString(reader, "TRACK_ID");
    data.trackOutput = extractString(reader, "TRACK_OUTPUT");

    data.playlistInfo = extractString(reader, "PLAYLIST_INFO");

    data.urlTrack    = extractString(reader, "URL_TRACK");
    data.urlPlaylist = extractString(reader, "URL_PLAYLIST");

    data.querySource   = extractString(reader, "QUERY_SOURCE");
    data.queryTrack    = extractString(reader, "QUERY_TRACK");
    data.queryPlaylist = extractString(reader, "QUERY_PLAYLIST");
    data.queryFolder   = extractString(reader, "QUERY_FOLDER");
    data.queryItem     = extractString(reader, "QUERY_ITEM");

    data.createQuery = extractString(reader, "CREATE_QUERY");

    data.extractSource   = extractString(reader, "EXTRACT_SOURCE");
    data.extractTrack    = extractString(reader, "EXTRACT_TRACK");
    data.extractPlaylist = extractString(reader, "EXTRACT_PLAYLIST");
    data.extractFolder   = extractString(reader, "EXTRACT_FOLDER");
    data.extractItem     = extractString(reader, "EXTRACT_ITEM");

    data.queryFailed = extractString(reader, "QUERY_FAILED");

    data.applySource   = extractString(reader, "APPLY_SOURCE");
    data.applyTrack    = extractString(reader, "APPLY_TRACK");
    data.applyPlaylist = extractString(reader, "APPLY_PLAYLIST");
    data.applyFolder   = extractString(reader, "APPLY_FOLDER");
    data.applyItem     = extractString(reader, "APPLY_ITEM");

    emit loaded(data);

    deleteLater();
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

bool WBackendUniversalQuery::extractBool(const WYamlReader & reader, const QString & key) const
{
    const WYamlNode * node = reader.at(key);

    if (node == NULL) return false;

    QString value = node->value.toLower();

    if (value == "true")
    {
        return true;
    }
    else if (value == "false")
    {
        return false;
    }
    else return value.toInt();
}

QString WBackendUniversalQuery::extractString(const WYamlReader & reader,
                                              const QString     & key) const
{
    const WYamlNode * node = reader.at(key);

    if (node)
    {
         return node->value;
    }
    else return QString();
}

WBackendUniversalData::Engines
WBackendUniversalQuery::extractEngines(const WYamlReader & reader, const QString & key) const
{
    QRegExp regExp("tracks|coverAudio|coverVideo");

    QString string = extractString(reader, key);

    if (string.indexOf(regExp) == -1)
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

    foreach (const WYamlNode & node, node->childs)
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

float WBackendUniversalNode::getFloat(WBackendUniversalParameters * parameters, int index) const
{
    return getVariant(parameters, index).toFloat();
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

QRegExp WBackendUniversalNode::getRegExp(WBackendUniversalParameters * parameters, int index) const
{
    return QRegExp(getString(parameters, index));
}

//-------------------------------------------------------------------------------------------------

QVariantList
WBackendUniversalNode::getList(WBackendUniversalParameters * parameters, int index) const
{
    return getVariant(parameters, index).toList();
}

//-------------------------------------------------------------------------------------------------

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

//-------------------------------------------------------------------------------------------------

QStringList
WBackendUniversalNode::getStringList(WBackendUniversalParameters * parameters, int index) const
{
    return getVariant(parameters, index).toStringList();
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

        variant.insert("url",   media.url);
        variant.insert("title", media.title);

        list.append(variant);
    }

    hash.insert("medias", list);

    return hash;
}

//-------------------------------------------------------------------------------------------------

QVariant * WBackendUniversalNode::getValue(WBackendUniversalParameters * parameters,
                                           int                           index) const
{
    const WBackendUniversalNode & node = nodes.at(index);

    if (node.type == Variable)
    {
         return parameters->value(node.data);
    }
    else return NULL;
}

const QVariant * WBackendUniversalNode::getValueConst(WBackendUniversalParameters * parameters,
                                                      int                           index) const
{
    const WBackendUniversalNode & node = nodes.at(index);

    if (node.type == Variable)
    {
         return parameters->valueConst(node.data);
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

//=================================================================================================
// WBackendUniversalScript
//=================================================================================================

WBackendUniversalScript::WBackendUniversalScript(const QString & data)
{
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
                if (condition > BACKENDUNIVERSAL_MAX_CONDITIONS)
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

            QVariant * value = node.getValue(parameters, 1);

            if (value)
            {
                WBackendUniversalLoop loop;

                QStringList list = node.getStringList(parameters, 0);

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
                    if (loops.count() > BACKENDUNIVERSAL_MAX_LOOPS)
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

            QStringList * list = &(loop.list);

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

    QRegExp regExp(BACKENDUNIVERSAL_FUNCTIONS);

    QString content = data;

    while (content.isEmpty() == false)
    {
        QString string = Sk::extractLine(&content);

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
                    qWarning("WBackendUniversalScript::load: Invalid parameters [%s].", word.C_STR);

                    nodes.clear();

                    return;
                }
                else if (nodes.count() > BACKENDUNIVERSAL_MAX_NODES)
                {
                    qWarning("WBackendUniversalScript::load: Maximum nodes reached [%s].",
                             word.C_STR);

                    nodes.clear();

                    return;
                }

                nodes.append(node);
            }
            else
            {
                qWarning("WBackendUniversalScript::load: Invalid function [%s].", word.C_STR);

                nodes.clear();

                return;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

bool WBackendUniversalScript::loadParameters(WBackendUniversalNode * node,
                                             QString               * string,
                                             const QRegExp         & regExp) const
{
    Sk::skipSpaces(string);

    if (string->isEmpty()) return true;

    QChar character = string->at(0);

    if (character == ')') return true;

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
                                           const QRegExp         & regExp) const
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
    return Sk::extractText(string, QRegExp("[\\s()]"));
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
        while (string->at(index - 1) == '\\')
        {
            int at = string->indexOf(character, index + 1);

            if (at == -1) break;

            index = at;
        }

        result = string->mid(1, index - 1);

        result.replace("\\\"", "\"");

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
    (*index)++;

    while (*index < nodes.count())
    {
        const QString & data = nodes.at(*index).data;

        if (data == "END")
        {
            return true;
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

//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::init(const QString & id, const QString & source)
{
    W_GET_CONTROLLER(WControllerPlaylist, controller);

    if (controller == NULL)
    {
        qWarning("WBackendUniversalPrivate::init: WControllerPlaylist does not exist.");

        thread = NULL;
    }
    else thread = controller->thread();

    remote = NULL;

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

void WBackendUniversalPrivate::populateHash() const
{
    if (hash.isEmpty() == false) return;

    hash.insert("EQUAL",                     equal);
    hash.insert("NOT_EQUAL",                 notEqual);
    hash.insert("LESSER",                    lesser);
    hash.insert("GREATER",                   greater);
    hash.insert("LESSER_EQUAL",              lesserEqual);
    hash.insert("GREATER_EQUAL",             greaterEqual);
    hash.insert("NUMBER",                    number);
    hash.insert("LIST",                      list);
    hash.insert("TIME",                      time);
    hash.insert("DATE",                      date);
    hash.insert("ADD",                       add);
    hash.insert("SUB",                       sub);
    hash.insert("MULTIPLY",                  multiply);
    hash.insert("GET_CHAR",                  getChar);
    hash.insert("GET_LIST",                  getList);
    hash.insert("GET_HASH",                  getHash);
    hash.insert("SET",                       set);
    hash.insert("SET_HASH",                  setHash);
    hash.insert("PREPEND",                   prepend);
    hash.insert("APPEND",                    append);
    hash.insert("APPEND_LIST",               appendList);
    hash.insert("REMOVE_CHAR",               removeChar);
    hash.insert("REMOVE_LIST",               removeList);
    hash.insert("CHOP",                      chop);
    hash.insert("REPLACE",                   replace);
    hash.insert("REPLACE_REGEXP",            replaceRegExp);
    hash.insert("MID",                       mid);
    hash.insert("SPLIT",                     split);
    hash.insert("JOIN",                      join);
    hash.insert("LOWER",                     lower);
    hash.insert("SIMPLIFY",                  simplify);
    hash.insert("TAKE",                      take);
    hash.insert("READ",                      read);
    hash.insert("LATIN_TO_UTF8",             latinToUtf8);
    hash.insert("LENGTH",                    length);
    hash.insert("COUNT",                     count);
    hash.insert("COUNT_STRING",              countString);
    hash.insert("INDEX_OF",                  indexOf);
    hash.insert("INDEX_REGEXP",              indexRegExp);
    hash.insert("INDEX_END",                 indexEnd);
    hash.insert("INDEX_REGEXP_END",          indexRegExpEnd);
    hash.insert("LAST_INDEX_OF",             lastIndexOf);
    hash.insert("LAST_INDEX_REGEXP",         lastIndexRegExp);
    hash.insert("LAST_INDEX_END",            lastIndexEnd);
    hash.insert("LAST_INDEX_REGEXP_END",     lastIndexRegExpEnd);
    hash.insert("CONTAIN",                   contain);
    hash.insert("CONTAIN_REGEXP",            containRegExp);
    hash.insert("CONTAIN_LIST",              containList);
    hash.insert("START_WITH",                startWith);
    hash.insert("START_WITH_REGEXP",         startWithRegExp);
    hash.insert("SLICE",                     slice);
    hash.insert("SLICE_IN",                  sliceIn);
    hash.insert("SLICES",                    slices);
    hash.insert("ADD_QUERY",                 addQuery);
    hash.insert("DECODE_URL",                decodeUrl);
    hash.insert("URL_NAME",                  urlName);
    hash.insert("URL_FRAGMENT",              urlFragment);
    hash.insert("EXTENSION_IS_MEDIA",        extensionIsMedia);
    hash.insert("EXTENSION_IS_VIDEO",        extensionIsVideo);
    hash.insert("EXTENSION_IS_AUDIO",        extensionIsAudio);
    hash.insert("EXTRACT_URL_PATH",          extractUrlPath);
    hash.insert("EXTRACT_URL_ELEMENT",       extractUrlElement);
    hash.insert("EXTRACT_URL_ELEMENTS",      extractUrlElements);
    hash.insert("EXTRACT_URL_EXTENSION",     extractUrlExtension);
    hash.insert("REMOVE_URL_PREFIX",         removeUrlPrefix);
    hash.insert("EXTRACT_HTML",              extractHtml);
    hash.insert("EXTRACT_ATTRIBUTE",         extractAttribute);
    hash.insert("EXTRACT_ATTRIBUTE_AT",      extractAttributeAt);
    hash.insert("EXTRACT_ATTRIBUTE_UTF8",    extractAttributeUtf8);
    hash.insert("EXTRACT_ATTRIBUTE_UTF8_AT", extractAttributeUtf8At);
    hash.insert("EXTRACT_JSON",              extractJson);
    hash.insert("EXTRACT_JSON_UTF8",         extractJsonUtf8);
    hash.insert("EXTRACT_JSON_HTML",         extractJsonHtml);
    hash.insert("SPLIT_JSON",                splitJson);
    hash.insert("TORRENT_STRING_AFTER",      torrentStringAfter);
    hash.insert("TORRENT_INTEGER_AFTER",     torrentIntegerAfter);
    hash.insert("TORRENT_LIST_AFTER",        torrentListAfter);
    hash.insert("TORRENT_ITEMS",             torrentItems);
    hash.insert("TORRENT_FOLDERS",           torrentFolders);
    hash.insert("PRINT",                     print);

    cache = new WBackendCache(sk);
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::load()
{
    Q_Q(WBackendUniversal);

    if (remote) delete remote;

    remote = wControllerDownload->getData(source);

    QObject::connect(remote, SIGNAL(loaded(WRemoteData *)), q, SLOT(onLoaded()));
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::runQuery(WBackendNetQuery * query, const QString & source,
                                                                  const QString & url) const
{
    WBackendUniversalScript * script = cache->getScript(id + source, source);

    if (script->isValid() == false) return;

    WBackendUniversalParameters parameters(*script, global);

    query->url = url;

    applyQueryParameters(&parameters, *query);

    script->run(&parameters);

    applyQueryResults(&parameters, query);
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::applyQueryParameters(WBackendUniversalParameters * parameters,
                                                    const WBackendNetQuery      & query) const
{
    parameters->add("backend", query.backend);

    parameters->add("url",         query.url);
    parameters->add("urlRedirect", query.urlRedirect);

    parameters->add("id", query.id);

    parameters->add("queryData", query.data);

    parameters->add("clearItems", query.clearItems);
    parameters->add("cookies",    query.cookies);
    parameters->add("header",     query.header);
    parameters->add("skipError",  query.skipError);

    parameters->add("delay",   query.delay);
    parameters->add("timeout", query.timeout);
}

void WBackendUniversalPrivate::applyQueryResults(WBackendUniversalParameters * parameters,
                                                 WBackendNetQuery            * query) const
{
    query->backend = parameters->value("backend")->toString();

    query->url = parameters->value("url")->toString();

    query->id = parameters->value("id")->toInt();

    query->data = *(parameters->value("queryData"));

    query->clearItems = parameters->value("clearItems")->toBool();
    query->cookies    = parameters->value("cookies")   ->toBool();
    query->header     = parameters->value("header")    ->toBool();
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

    parameters->add("expiry", reply.expiry);

    parameters->add("next");
}

void WBackendUniversalPrivate::applySourceResults(WBackendUniversalParameters * parameters,
                                                  WBackendNetSource           * reply) const
{
    reply->valid = parameters->value("valid")->toBool();

    applyQualities(&(reply->medias), parameters->value("medias"));
    applyQualities(&(reply->audios), parameters->value("audios"));

    reply->expiry = getDate(*(parameters->value("expiry")));

    applyQuery(&(reply->nextQuery), parameters->value("next"));

    reply->backup = *(parameters->value("global"));
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

    parameters->add("source");

    parameters->add("title");
    parameters->add("cover");

    parameters->add("author");
    parameters->add("feed");

    parameters->add("duration", reply.track.duration());

    parameters->add("date");

    parameters->add("quality");
}

void WBackendUniversalPrivate::applyTrackResults(WBackendUniversalParameters * parameters,
                                                 WBackendNetTrack            * reply) const
{
    reply->valid = parameters->value("valid")->toBool();

    reply->cache = parameters->value("cache")->toByteArray();

    applyQuery(&(reply->nextQuery), parameters->value("next"));

    reply->backup = *(parameters->value("global"));

    WTrack & track = reply->track;

    track.setSource(parameters->value("source")->toString());

    track.setTitle(parameters->value("title")->toString());
    track.setCover(parameters->value("cover")->toString());

    track.setAuthor(parameters->value("author")->toString());
    track.setFeed  (parameters->value("feed")  ->toString());

    track.setDuration(parameters->value("duration")->toInt());

    track.setDate(getDate(*(parameters->value("date"))));

    track.setQuality(getQuality(parameters->value("quality")->toString()));
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

    applyQuery(&(reply->nextQuery), parameters->value("next"));

    reply->backup = *(parameters->value("global"));

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

    applyQuery(&(reply->nextQuery), parameters->value("next"));

    reply->backup = *(parameters->value("global"));

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

    parameters->add("itemData");
    parameters->add("extension");
}

void WBackendUniversalPrivate::applyItemResults(WBackendUniversalParameters * parameters,
                                                WBackendNetItem             * reply) const
{
    reply->valid = parameters->value("valid")->toBool();

    reply->cache = parameters->value("cache")->toByteArray();

    applyQuery(&(reply->nextQuery), parameters->value("next"));

    reply->backup = *(parameters->value("global"));

    reply->data = parameters->value("itemData")->toByteArray();

    reply->extension = parameters->value("extension")->toString();
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::applyTrack(QList<WTrack>  * tracks,
                                          const QVariant & value) const
{
    QHash<QString, QVariant> hash = value.toHash();

    WTrack track;

    track.setState(getStateTrack(hash.value("state").toString()));

    track.setSource(hash.value("source").toString());

    track.setTitle(hash.value("title").toString());
    track.setCover(hash.value("cover").toString());

    track.setAuthor(hash.value("author").toString());
    track.setFeed  (hash.value("feed")  .toString());

    const QVariant * variant = getVariant(&hash, "duration");

    if (variant) track.setDuration(variant->toInt());

    track.setDate(getDate(hash.value("date")));

    track.setQuality(getQuality(hash.value("quality").toString()));

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

void WBackendUniversalPrivate::applyQuery(WBackendNetQuery * query, QVariant * value) const
{
    QHash<QString, QVariant> hash = value->toHash();

    query->backend = hash.value("backend").toString();

    query->url = hash.value("url").toString();

    query->id = hash.value("id").toInt();

    query->data = hash.value("queryData");

    const QVariant * variant = getVariant(&hash, "clearItems");

    if (variant) query->clearItems = variant->toBool();

    query->cookies   = hash.value("cookies")  .toBool();
    query->header    = hash.value("header")   .toBool();
    query->skipError = hash.value("skipError").toBool();

    query->delay = hash.value("delay").toInt();

    variant = getVariant(&hash, "timeout");

    if (variant) query->timeout = variant->toInt();
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

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Quality WBackendUniversalPrivate::getQuality(const QString & string) const
{
    if      (string ==  "240") return WAbstractBackend::Quality240;
    else if (string ==  "360") return WAbstractBackend::Quality360;
    else if (string ==  "480") return WAbstractBackend::Quality480;
    else if (string ==  "720") return WAbstractBackend::Quality720;
    else if (string == "1080") return WAbstractBackend::Quality1080;
    else if (string == "1440") return WAbstractBackend::Quality1440;
    else if (string == "2160") return WAbstractBackend::Quality2160;
    else                       return WAbstractBackend::QualityInvalid;
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

//-------------------------------------------------------------------------------------------------

WLocalObject::State WBackendUniversalPrivate::getState(const QString & string) const
{
    if (string == "default") return WLocalObject::Default;
    else                     return WLocalObject::Loaded;
}

WTrack::State WBackendUniversalPrivate::getStateTrack(const QString & string) const
{
    if      (string == "default") return WTrack::Default;
    else if (string == "cover")   return WTrack::Cover;
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

QString WBackendUniversalPrivate::getUrl(const QString & url) const
{
    int indexA = url.indexOf("//");

    if (indexA != -1)
    {
        indexA += 2;

        while (indexA < url.length() && url.at(indexA) == '/')
        {
            indexA++;
        }
    }

    int indexB = url.indexOf("www.", indexA);

    if (indexB != -1 && indexB == indexA)
    {
        indexA += 4;
    }
    else if (indexA == -1)
    {
        indexB = url.indexOf(QRegExp("[\\?#]"));

        if (indexB == -1)
        {
             return url;
        }
        else return url.mid(0, indexB);
    }

    indexB = url.indexOf(QRegExp("[\\?#]"), indexA);

    if (indexB == -1)
    {
         return url.mid(indexA);
    }
    else return url.mid(indexA, indexB - indexA);
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::onLoaded()
{
    Q_Q(WBackendUniversal);

    QByteArray array = remote->readAll();

    QObject::disconnect(remote, 0, q, 0);

    remote->deleteLater();

    remote = NULL;

    if (array.isEmpty()) return;

    WBackendUniversalQuery * query = new WBackendUniversalQuery;

    QObject::connect(query, SIGNAL(loaded(WBackendUniversalData)),
                     q,     SLOT(onData(WBackendUniversalData)));

    if (thread)
    {
        query->moveToThread(thread);

        method.invoke(query, Q_ARG(const QByteArray &, array));
    }
    else query->extract(array);
}

void WBackendUniversalPrivate::onData(const WBackendUniversalData & data)
{
    Q_Q(WBackendUniversal);

    this->data = data;

    WBackendUniversalScript script(data.trackId);

    script.dump();

    //---------------------------------------------------------------------------------------------
    // checkValidUrl

    bool validUrl = q->checkValidUrl("https://www.dailymotion.com/video/1234567");

    //---------------------------------------------------------------------------------------------
    // getTrackId

    QString trackId = q->getTrackId("https://www.dailymotion.com/video/1234567");

    //---------------------------------------------------------------------------------------------
    // getPlaylistInfo

    WBackendNetPlaylistInfo info = q->getPlaylistInfo("https://www.dailymotion.com/tennisactu");

    //---------------------------------------------------------------------------------------------
    // getUrlTrack

    QString urlTrack = q->getUrlTrack("1234");

    //---------------------------------------------------------------------------------------------
    // getUrlPlaylist

    QString urlPlaylist = q->getUrlPlaylist(info);

    //---------------------------------------------------------------------------------------------
    // getQuery

    WBackendNetQuery querySource = q->getQuerySource("https://www.dailymotion.com/video/1234567");
    WBackendNetQuery queryTrack  = q->getQueryTrack ("https://www.dailymotion.com/video/1234567");

    WBackendNetQuery queryPlaylist = q->getQueryPlaylist("https://www.dailymotion.com/tennisactu");

    //---------------------------------------------------------------------------------------------
    // createQuery

    WBackendNetQuery createQuery = q->createQuery("search", "channels", "above & beyond");

    //---------------------------------------------------------------------------------------------
    // extract

    WBackendNetSource   extractSource   = q->extractSource  ("lol", WBackendNetQuery());
    WBackendNetTrack    extractTrack    = q->extractTrack   ("lol", WBackendNetQuery());
    WBackendNetPlaylist extractPlaylist = q->extractPlaylist("lol", WBackendNetQuery());
    WBackendNetFolder   extractFolder   = q->extractFolder  ("lol", WBackendNetQuery());

    //---------------------------------------------------------------------------------------------

    qDebug("checkValidUrl [%d]", validUrl);

    qDebug("getTrackId [%s]", trackId.C_STR);

    qDebug("getPlaylistInfo [%d] [%s]", info.type, info.id.C_STR);

    qDebug("getUrlTrack [%s]", urlTrack.C_STR);

    qDebug("getUrlPlaylist [%s]", urlPlaylist.C_STR);

    qDebug("getQuerySource   [%s]", querySource  .url.C_STR);
    qDebug("getQueryTrack    [%s]", queryTrack   .url.C_STR);
    qDebug("getQueryPlaylist [%s]", queryPlaylist.url.C_STR);

    qDebug("createQuery [%s]", createQuery.url.C_STR);

    qDebug("extractSource   [%d]", extractSource  .valid);
    qDebug("extractTrack    [%d]", extractTrack   .valid);
    qDebug("extractPlaylist [%d]", extractPlaylist.valid);
    qDebug("extractFolder   [%d]", extractFolder  .valid);
}

//=================================================================================================
// WBackendUniversal
//=================================================================================================

WBackendUniversal::WBackendUniversal(const QString & id, const QString & source)
    : WBackendNet(new WBackendUniversalPrivate(this))
{
    Q_D(WBackendUniversal); d->init(id, source);
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

/* Q_INVOKABLE virtual */ void WBackendUniversal::reload()
{
    Q_D(WBackendUniversal);

    cache->removeScripts(d->id);

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

/* Q_INVOKABLE virtual */ bool WBackendUniversal::checkValidUrl(const QString & url) const
{
    Q_D(const WBackendUniversal);

    QString source = d->getUrl(url).toLower();

    if (source.indexOf(QRegExp(d->data.validate)) == -1)
    {
         return false;
    }
    else return true;
}

/* Q_INVOKABLE virtual */ bool WBackendUniversal::checkCover(const QString &, const QString & q) const
{
    Q_D(const WBackendUniversal);

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

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QList<WLibraryFolderItem> WBackendUniversal::getLibraryItems() const
{
    Q_D(const WBackendUniversal); return d->data.items;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
QString WBackendUniversal::getTrackId(const QString & url) const
{
    Q_D(const WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION getTrackId");
#endif

    WBackendUniversalScript * script = cache->getScript(d->id + "trackId",
                                                        d->data.trackId);

    if (script->isValid() == false) return QString();

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

    WBackendUniversalScript * script = cache->getScript(d->id + "trackOutput",
                                                        d->data.trackOutput);

    if (script->isValid() == false) return WAbstractBackend::OutputMedia;

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

    WBackendUniversalScript * script = cache->getScript(d->id + "playlistInfo",
                                                        d->data.playlistInfo);

    if (script->isValid() == false) return WBackendNetPlaylistInfo();

    WBackendUniversalParameters parameters(*script, d->global);

    parameters.add("type");
    parameters.add("id");

    parameters.add("url", url);

    script->run(&parameters);

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

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
QString WBackendUniversal::getUrlTrack(const QString & id) const
{
    Q_D(const WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION getUrlTrack");
#endif

    WBackendUniversalScript * script = cache->getScript(d->id + "urlTrack",
                                                        d->data.urlTrack);

    if (script->isValid() == false) return QString();

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

    WBackendUniversalScript * script = cache->getScript(d->id + "urlPlaylist",
                                                        d->data.urlPlaylist);

    if (script->isValid() == false) return QString();

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
WBackendNetQuery WBackendUniversal::getQuerySource(const QString & url) const
{
    Q_D(const WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION getQuerySource");
#endif

    WBackendNetQuery query;

    d->runQuery(&query, d->data.querySource, url);

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

    d->runQuery(&query, d->data.queryTrack, url);

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

    d->runQuery(&query, d->data.queryPlaylist, url);

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

    d->runQuery(&query, d->data.queryFolder, url);

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

    d->runQuery(&query, d->data.queryItem, url);

    return query;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendUniversal::createQuery(const QString & method,
                                                const QString & label, const QString & q) const
{
    Q_D(const WBackendUniversal);

#ifdef SK_BACKEND_LOG
    qDebug("FUNCTION createQuery");
#endif

    WBackendUniversalScript * script = cache->getScript(d->id + "createQuery",
                                                        d->data.createQuery);

    if (script->isValid() == false) return WBackendNetQuery();

    WBackendUniversalParameters parameters(*script, d->global);

    WBackendNetQuery query;

    d->applyQueryParameters(&parameters, query);

    parameters.add("method", method);
    parameters.add("label",  label);
    parameters.add("q",      q);

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

    WBackendUniversalScript * script = cache->getScript(d->id + "extractSource",
                                                        d->data.extractSource);

    if (script->isValid() == false) return WBackendNetSource();

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

    WBackendUniversalScript * script = cache->getScript(d->id + "extractTrack",
                                                        d->data.extractTrack);

    if (script->isValid() == false) return WBackendNetTrack();

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

    WBackendUniversalScript * script = cache->getScript(d->id + "extractPlaylist",
                                                        d->data.extractPlaylist);

    if (script->isValid() == false) return WBackendNetPlaylist();

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

    WBackendUniversalScript * script = cache->getScript(d->id + "extractFolder",
                                                        d->data.extractFolder);

    if (script->isValid() == false) return WBackendNetFolder();

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

    WBackendUniversalScript * script = cache->getScript(d->id + "extractItem",
                                                        d->data.extractItem);

    if (script->isValid() == false) return WBackendNetItem();

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

    WBackendUniversalScript * script = cache->getScript(d->id + "queryFailed",
                                                        d->data.queryFailed);

    if (script->isValid() == false) return;

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
