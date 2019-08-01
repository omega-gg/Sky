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
#include <WControllerPlaylist>
#include <WCache>
#include <WYamlReader>

//-------------------------------------------------------------------------------------------------
// Static variables

static const QString BACKENDUNIVERSAL_FUNCTIONS = "IF|AND|OR|ELSE|ELIF|FI|FOREACH|END|EQUALS|"
                                                  "NOT_EQUALS|LOWER|GREATER|MULTIPLY|RETURN|SET|"
                                                  "SET_HASH|PREPEND|APPEND|APPEND_LIST|READ|"
                                                  "LENGTH|INDEX_OF|INDEX_OF_REGEXP|INDEX_END|"
                                                  "INDEX_END_REGEXP|CONTAINS|STARTS_WITH|"
                                                  "STARTS_WITH_REGEXP|REMOVE_CHARS|REMOVE_PREFIX|"
                                                  "EXTRACT_URL_ELEMENT|ADD_QUERY|EXTRACT_JSON|"
                                                  "EXTRACT_JSON_UTF8|EXTRACT_JSON_HTML|SPLIT_JSON|"
                                                  "PRINT";

static const int BACKENDUNIVERSAL_MAX_NODES = 200;

static const int BACKENDUNIVERSAL_MAX_CONDITIONS = 1;
static const int BACKENDUNIVERSAL_MAX_LOOPS      = 1;

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

    data.api = extractString(reader, "api");

    data.hasSearch = extractBool(reader, "hasSearch");

    data.isSearchEngine = extractBool(reader, "isSearchEngine");
    data.isSearchCover  = extractBool(reader, "isSearchCover");

    data.title = extractString(reader, "title");
    data.host  = extractString(reader, "host");

    data.items = extractItems(reader);

    //---------------------------------------------------------------------------------------------

    data.validate = extractString(reader, "VALIDATE");

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

WBackendUniversalNode::WBackendUniversalNode(Type type)
{
    this->type = type;
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

QVariant WBackendUniversalNode::run(WBackendUniversalParameters * parameters) const
{
    if (data == "EQUALS")
    {
        if (nodes.count() < 2) return false;

        QString stringA = getString(parameters, 0);
        QString stringB = getString(parameters, 1);

#ifdef SK_BACKEND_LOG
        qDebug("EQUALS [%d]", (stringA == stringB));
#endif

        return (stringA == stringB);
    }
    else if (data == "NOT_EQUALS")
    {
        if (nodes.count() < 2) return false;

        QString stringA = getString(parameters, 0);
        QString stringB = getString(parameters, 1);

#ifdef SK_BACKEND_LOG
        qDebug("NOT_EQUALS [%d]", (stringA != stringB));
#endif

        return (stringA != stringB);
    }
    else if (data == "LOWER")
    {
        if (nodes.count() < 2) return false;

        float floatA = getFloat(parameters, 0);
        float floatB = getFloat(parameters, 1);

#ifdef SK_BACKEND_LOG
        qDebug("LOWER [%d]", (floatA < floatB));
#endif

        return (floatA < floatB);
    }
    else if (data == "GREATER")
    {
        if (nodes.count() < 2) return false;

        float floatA = getFloat(parameters, 0);
        float floatB = getFloat(parameters, 1);

#ifdef SK_BACKEND_LOG
        qDebug("GREATER [%d]", (floatA > floatB));
#endif

        return (floatA > floatB);
    }
    else if (data == "MULTIPLY")
    {
        if (nodes.count() < 2) return false;

        float floatA = getFloat(parameters, 0);
        float floatB = getFloat(parameters, 1);

#ifdef SK_BACKEND_LOG
        qDebug("MULTIPLY [%f]", (floatA * floatB));
#endif

        return (floatA * floatB);
    }
    else if (data == "SET")
    {
        int count = nodes.count();

        if (count < 2) return false;

        QVariant * key = getValue(parameters, 0);

        if (key == NULL) return false;

#ifdef SK_BACKEND_LOG
        qDebug("SET");
#endif

        if (count != 2)
        {
            QVariantList list;

            for (int i = 1; i < count; i++)
            {
                list.append(getVariant(parameters, i));
            }

            *key = list;
        }
        else *key = getVariant(parameters, 1);

        return true;
    }
    else if (data == "SET_HASH")
    {
        if (nodes.count() < 3) return false;

#ifdef SK_BACKEND_LOG
        qDebug("SET_HASH");
#endif

        QVariant * key = getValue(parameters, 0);

        if (key == NULL) return false;

        QHash<QString, QVariant> hash = key->toHash();

        hash.insert(getString(parameters, 1), getString(parameters, 2));

        *key = hash;

        return true;
    }
    else if (data == "PREPEND")
    {
        int count = nodes.count();

        if (count < 2) return false;

#ifdef SK_BACKEND_LOG
        qDebug("PREPEND");
#endif

        QString string;

        QVariant * key = getValue(parameters, 0);

        if (key)
        {
            string = key->toString();

            for (int i = 1; i < count; i++)
            {
                string.prepend(getString(parameters, i));
            }

            *key = string;
        }
        else
        {
            string = getString(parameters, 0);

            for (int i = 1; i < count; i++)
            {
                string.prepend(getString(parameters, i));
            }
        }

        return string;
    }
    else if (data == "APPEND")
    {
        int count = nodes.count();

        if (count < 2) return false;

#ifdef SK_BACKEND_LOG
        qDebug("APPEND");
#endif

        QString string;

        QVariant * key = getValue(parameters, 0);

        if (key)
        {
            string = key->toString();

            for (int i = 1; i < count; i++)
            {
                string.append(getString(parameters, i));
            }

            *key = string;
        }
        else
        {
            string = getString(parameters, 0);

            for (int i = 1; i < count; i++)
            {
                string.append(getString(parameters, i));
            }
        }

        return string;
    }
    else if (data == "APPEND_LIST")
    {
        int count = nodes.count();

        if (count < 2) return false;

        QVariant * key = getValue(parameters, 0);

        if (key == NULL) return false;

#ifdef SK_BACKEND_LOG
        qDebug("APPEND_LIST");
#endif

        QList<QVariant> list = key->toList();

        list.append(*(getValue(parameters, 1)));

        *key = list;
    }
    else if (data == "READ")
    {
#ifdef SK_BACKEND_LOG
        qDebug("READ");
#endif

        int count = nodes.count();

        if (count == 1)
        {
            return Sk::readUtf8(getByteArray(parameters, 0));
        }
        else if (count > 1)
        {
            return Sk::readCodec(getByteArray(parameters, 0), getString(parameters, 1));
        }
    }
    else if (data == "LENGTH")
    {
        if (nodes.count())
        {
            qDebug("LENGTH [%d]", getString(parameters, 0).length());

            return getString(parameters, 0).length();
        }
        else
        {
            qDebug("LENGTH [0]");

            return 0;
        }
    }
    else if (data == "INDEX_OF")
    {
        int count = nodes.count();

        if (count < 2) return -1;

#ifdef SK_BACKEND_LOG
        qDebug("INDEX_OF");
#endif
        QString string = getString(parameters, 0);

        if (count == 2)
        {
            return string.indexOf(getString(parameters, 1));
        }
        else if (count > 2)
        {
            return string.indexOf(getString(parameters, 1), getInt(parameters, 2));
        }
    }
    else if (data == "INDEX_OF_REGEXP")
    {
        int count = nodes.count();

        if (count < 2) return -1;

#ifdef SK_BACKEND_LOG
        qDebug("INDEX_OF_REGEXP");
#endif

        QString string = getString(parameters, 0);

        QRegExp regExp = getRegExp(parameters, 1);

        if (count == 2)
        {
            return string.indexOf(regExp);
        }
        else if (count > 2)
        {
            return string.indexOf(regExp, getInt(parameters, 2));
        }

        return string.indexOf(regExp);
    }
    else if (data == "INDEX_END")
    {
        int count = nodes.count();

        if (count < 2) return -1;

#ifdef SK_BACKEND_LOG
        qDebug("INDEX_OF");
#endif

        QString stringA = getString(parameters, 0);
        QString stringB = getString(parameters, 1);

        if (count == 2)
        {
            return stringA.indexOf(stringB) + stringB.length();
        }
        else if (count > 2)
        {
            return stringA.indexOf(stringB, getInt(parameters, 2)) + stringB.length();
        }
    }
    else if (data == "INDEX_END_REGEXP")
    {
        int count = nodes.count();

        if (count < 2) return -1;

#ifdef SK_BACKEND_LOG
        qDebug("INDEX_OF_REGEXP");
#endif

        QString string = getString(parameters, 0);

        QRegExp regExp = getRegExp(parameters, 1);

        if (count == 2)
        {
            return string.indexOf(regExp) + regExp.matchedLength();
        }
        else if (count > 2)
        {
            return string.indexOf(regExp, getInt(parameters, 2)) + regExp.matchedLength();
        }
    }
    else if (data == "CONTAINS")
    {
        if (nodes.count() < 2) return false;

#ifdef SK_BACKEND_LOG
        qDebug("CONTAINS");
#endif

        QString string = getString(parameters, 0);

#ifdef QT_4
        // FIXME Qt4: The code does not compile without the 'QVariant'.
        return QVariant(string.contains(getString(parameters, 1)));
#else
        return string.contains(getString(parameters, 1));
#endif
    }
    else if (data == "STARTS_WITH")
    {
        if (nodes.count() < 2) return false;

#ifdef SK_BACKEND_LOG
        qDebug("STARTS_WITH");
#endif

        QString string = getString(parameters, 0);

        return string.startsWith(getString(parameters, 1));
    }
    else if (data == "STARTS_WITH_REGEXP")
    {
        if (nodes.count() < 2) return false;

#ifdef SK_BACKEND_LOG
        qDebug("STARTS_WITH_REGEXP");
#endif

        QString string = getString(parameters, 0);

        QRegExp regExp = getRegExp(parameters, 1);

        if (string.indexOf(regExp) == 0)
        {
             return true;
        }
        else return false;
    }
    else if (data == "REMOVE_CHARS")
    {
        if (nodes.count() < 3) return QString();

#ifdef SK_BACKEND_LOG
        qDebug("REMOVE_CHARS");
#endif

        QVariant * key = getValue(parameters, 0);

        if (key == NULL) return QString();

        *key = key->toString().remove(getInt(parameters, 1), getInt(parameters, 2));

        return *key;
    }
    else if (data == "REMOVE_PREFIX")
    {
        if (nodes.count() < 1) return QString();

#ifdef SK_BACKEND_LOG
        qDebug("REMOVE_PREFIX");
#endif

        QVariant * key = getValue(parameters, 0);

        if (key == NULL) return QString();

        *key = WControllerNetwork::removeUrlPrefix(key->toString());

        return *key;
    }
    else if (data == "EXTRACT_URL_ELEMENT")
    {
        if (nodes.count() < 2) return QString();

#ifdef SK_BACKEND_LOG
        qDebug("EXTRACT_URL_ELEMENT");
#endif

        QString string = getString(parameters, 0);

        int from = getInt(parameters, 1);

        return WControllerNetwork::extractUrlElement(string, from);
    }
    else if (data == "ADD_QUERY")
    {
        if (nodes.count() < 3) return QString();

        QVariant * key = getValue(parameters, 0);

        if (key == NULL) return QString();

#ifdef SK_BACKEND_LOG
        qDebug("ADD_QUERY");
#endif

        QUrl url(key->toString());

#ifdef QT_4
        url.addQueryItem(getString(parameters, 1), getString(parameters, 2));
#else
        QUrlQuery urlQuery(url);

        urlQuery.addQueryItem(getString(parameters, 1), getString(parameters, 2));

        url.setQuery(urlQuery);
#endif

        *key = url;

        return *key;
    }
    else if (data == "EXTRACT_JSON")
    {
#ifdef SK_BACKEND_LOG
        qDebug("EXTRACT_JSON");
#endif

        int count = nodes.count();

        if (count == 1)
        {
            return WControllerNetwork::extractJson(getString(parameters, 0));
        }
        else if (count == 2)
        {
            return WControllerNetwork::extractJson(getString(parameters, 0),
                                                   getString(parameters, 1));
        }
        else if (count > 2)
        {
            return WControllerNetwork::extractJson(getString(parameters, 0),
                                                   getString(parameters, 1),
                                                   getInt(parameters, 2));
        }
    }
    else if (data == "EXTRACT_JSON_UTF8")
    {
#ifdef SK_BACKEND_LOG
        qDebug("EXTRACT_JSON_UTF8");
#endif

        int count = nodes.count();

        if (count == 1)
        {
            return WControllerNetwork::extractJsonUtf8(getString(parameters, 0));
        }
        else if (count == 2)
        {
            return WControllerNetwork::extractJsonUtf8(getString(parameters, 0),
                                                       getString(parameters, 1));
        }
        else if (count > 2)
        {
            return WControllerNetwork::extractJsonUtf8(getString(parameters, 0),
                                                       getString(parameters, 1),
                                                       getInt(parameters, 2));
        }
    }
    else if (data == "EXTRACT_JSON_HTML")
    {
#ifdef SK_BACKEND_LOG
        qDebug("EXTRACT_JSON_HTML");
#endif

        int count = nodes.count();

        if (count == 1)
        {
            return WControllerNetwork::extractJsonHtml(getString(parameters, 0));
        }
        else if (count == 2)
        {
            return WControllerNetwork::extractJsonHtml(getString(parameters, 0),
                                                       getString(parameters, 1));
        }
        else if (count > 2)
        {
            return WControllerNetwork::extractJsonHtml(getString(parameters, 0),
                                                       getString(parameters, 1),
                                                       getInt(parameters, 2));
        }
    }
    else if (data == "SPLIT_JSON")
    {
#ifdef SK_BACKEND_LOG
        qDebug("SPLIT_JSON");
#endif

        int count = nodes.count();

        if (count == 1)
        {
            return WControllerNetwork::splitJson(getString(parameters, 0));
        }
        else if (count > 2)
        {
            return WControllerNetwork::splitJson(getString(parameters, 0), getInt(parameters, 1));
        }
    }
    else if (data == "PRINT")
    {
        int count = nodes.count();

        if (count < 1) return QString();

        QString string;

        for (int i = 0; i < count; i++)
        {
            string.append(getString(parameters, i));
        }

        qDebug("PRINT [%s]", string.C_STR);
    }

    return QVariant();
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
        QByteArray data = node.data.toUtf8();

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

QVariant * WBackendUniversalNode::getValue(WBackendUniversalParameters * parameters,
                                           int                           index) const
{
    const WBackendUniversalNode & node = nodes.at(index);

    if (node.type == Variable)
    {
        QByteArray data = node.data.toUtf8();

        return parameters->value(data);
    }
    else return NULL;
}

const QVariant * WBackendUniversalNode::getValueConst(WBackendUniversalParameters * parameters,
                                                      int                           index) const
{
    const WBackendUniversalNode & node = nodes.at(index);

    if (node.type == Variable)
    {
        QByteArray data = node.data.toUtf8();

        return parameters->valueConst(data);
    }
    else return NULL;
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
            if (node.nodes.count() < 2)
            {
                qWarning("WBackendUniversalScript::run: Invalid 'FOREACH' statement.");

                return QVariant();
            }

            const QVariant * key = node.getValueConst(parameters, 0);

            QVariant * value = node.getValue(parameters, 1);

            if (key && value)
            {
                WBackendUniversalLoop loop;

                QStringList list = key->toStringList();

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
                    loop.list  = list;

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
                    qWarning("WBackendUniversalScript::load: Invalid parameters [%s]", word.C_STR);

                    nodes.clear();

                    return;
                }
                else if (nodes.count() > BACKENDUNIVERSAL_MAX_NODES)
                {
                    qWarning("WBackendUniversalScript::load: Maximum nodes reached [%s]",
                             word.C_STR);

                    nodes.clear();

                    return;
                }

                nodes.append(node);
            }
            else
            {
                qWarning("WBackendUniversalScript::load: Invalid function [%s]", word.C_STR);

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

        WBackendUniversalNode child(WBackendUniversalNode::Function);

        loadFunction(&child, string, regExp);

        if (string->isEmpty())
        {
            qWarning("WBackendUniversalScript::loadFunction: Missing parenthesis [%s]",
                     string->C_STR);

            return false;
        }

        node->nodes.append(child);

        string->remove(0, 1);
    }
    else if (character == '"')
    {
        WBackendUniversalNode child(WBackendUniversalNode::String);

        child.data = extractString(string);

        node->nodes.append(child);
    }
    else
    {
        WBackendUniversalNode child(WBackendUniversalNode::Variable);

        child.data = extractWord(string);

        node->nodes.append(child);
    }

    loadParameters(node, string, regExp);

    return true;
}

bool WBackendUniversalScript::loadFunction(WBackendUniversalNode * node,
                                           QString               * string,
                                           const QRegExp         & regExp) const
{
    QString word = extractWord(string);

    if (regExp.exactMatch(word) == false)
    {
        qWarning("WBackendUniversalScript::loadFunction: Invalid function [%s]", word.C_STR);

        return false;
    }

    node->data = word;

    loadParameters(node, string, regExp);

    return true;
}

//-------------------------------------------------------------------------------------------------

QString WBackendUniversalScript::extractWord(QString * string) const
{
    return Sk::extractText(string, QRegExp("[\\s()]"));
}

QString WBackendUniversalScript::extractString(QString * string) const
{
    QString result;

    int index = string->indexOf('"', 1);

    if (index == -1)
    {
        result = *string;

        string->clear();

        //return result;
    }
    else
    {
        //index++;

        result = string->mid(1, index - 1);

        string->remove(0, index + 1);
    }

    /*while (string->at(index - 1) == '\\')
    {
        int at = string->indexOf('"', index + 1);

        if (at == -1) break;

        index = at;
    }

    index++;

    result = string->mid(0, index);

    result.replace("\\\"", "\"");

    string->remove(0, index);*/

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
        if (string.at(0).isLower())
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

    load();
}

//-------------------------------------------------------------------------------------------------
// Private functions
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
    WBackendUniversalScript script(source);

    if (script.isValid() == false) return;

    WBackendUniversalParameters parameters(script, global);

    query->url = url;

    applyQueryParameters(&parameters, *query);

    script.run(&parameters);

    applyQueryResults(&parameters, query);
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::applyQueryParameters(WBackendUniversalParameters * parameters,
                                                    const WBackendNetQuery      & query) const
{
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

    QList<QVariant> list = parameters->value("tracks")->toList();

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

    QList<QVariant> list = parameters->value("items")->toList();

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
    return QDateTime::fromTime_t(value.toUInt());
}

//-------------------------------------------------------------------------------------------------

WLibraryItem::Type WBackendUniversalPrivate::getType(const QString & string) const
{
    if      (string == "folder")   return WLibraryItem::Folder;
    else if (string == "playlist") return WLibraryItem::Playlist;
    else if (string == "feed")     return WLibraryItem::PlaylistFeed;
    else                           return WLibraryItem::Item;
}

WLocalObject::State WBackendUniversalPrivate::getState(const QString & string) const
{
    if (string == "default") return WLocalObject::Default;
    else                     return WLocalObject::Loaded;
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

/* Q_INVOKABLE virtual */ bool WBackendUniversal::hasSearch() const
{
    Q_D(const WBackendUniversal); return d->data.hasSearch;
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

    WBackendUniversalScript script(d->data.validate);

    if (script.isValid() == false) return false;

    WBackendUniversalParameters parameters(script, d->global);

    parameters.add("url", url);

    return script.run(&parameters).toBool();
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

    WBackendUniversalScript script(d->data.trackId);

    if (script.isValid() == false) return QString();

    WBackendUniversalParameters parameters(script, d->global);

    parameters.add("url", url);

    return script.run(&parameters).toString();
}

/* Q_INVOKABLE virtual */
WAbstractBackend::Output WBackendUniversal::getTrackOutput(const QString & url) const
{
    Q_D(const WBackendUniversal);

    WBackendUniversalScript script(d->data.trackId);

    if (script.isValid() == false) return WAbstractBackend::OutputMedia;

    WBackendUniversalParameters parameters(script, d->global);

    parameters.add("url", url);

    QString output = script.run(&parameters).toString();

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

    WBackendUniversalScript script(d->data.playlistInfo);

    if (script.isValid() == false) return WBackendNetPlaylistInfo();

    WBackendUniversalParameters parameters(script, d->global);

    parameters.add("type");
    parameters.add("id");

    parameters.add("url", url);

    script.run(&parameters);

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

    WBackendUniversalScript script(d->data.urlTrack);

    if (script.isValid() == false) return QString();

    WBackendUniversalParameters parameters(script, d->global);

    parameters.add("id", id);

    return script.run(&parameters).toString();
}

/* Q_INVOKABLE virtual */
QString WBackendUniversal::getUrlPlaylist(const WBackendNetPlaylistInfo & info) const
{
    Q_D(const WBackendUniversal);

    WBackendUniversalScript script(d->data.urlPlaylist);

    if (script.isValid() == false) return QString();

    WBackendUniversalParameters parameters(script, d->global);

    if (info.type == WLibraryItem::PlaylistFeed)
    {
         parameters.add("type", "feed");
    }
    else parameters.add("type", "playlist");

    parameters.add("id", info.id);

    return script.run(&parameters).toString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendUniversal::getQuerySource(const QString & url) const
{
    Q_D(const WBackendUniversal);

    WBackendNetQuery query;

    d->runQuery(&query, d->data.querySource, url);

    return query;
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendUniversal::getQueryTrack(const QString & url) const
{
    Q_D(const WBackendUniversal);

    WBackendNetQuery query;

    d->runQuery(&query, d->data.queryTrack, url);

    return query;
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendUniversal::getQueryPlaylist(const QString & url) const
{
    Q_D(const WBackendUniversal);

    WBackendNetQuery query;

    d->runQuery(&query, d->data.queryPlaylist, url);

    return query;
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendUniversal::getQueryFolder(const QString & url) const
{
    Q_D(const WBackendUniversal);

    WBackendNetQuery query;

    d->runQuery(&query, d->data.queryFolder, url);

    return query;
}

/* Q_INVOKABLE virtual */
WBackendNetQuery WBackendUniversal::getQueryItem(const QString & url) const
{
    Q_D(const WBackendUniversal);

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

    WBackendUniversalScript script(d->data.createQuery);

    if (script.isValid() == false) return WBackendNetQuery();

    WBackendUniversalParameters parameters(script, d->global);

    WBackendNetQuery query;

    d->applyQueryParameters(&parameters, query);

    parameters.add("method", method);
    parameters.add("label",  label);
    parameters.add("q",      q);

    script.run(&parameters);

    d->applyQueryResults(&parameters, &query);

    return query;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
WBackendNetSource WBackendUniversal::extractSource(const QByteArray       & data,
                                                   const WBackendNetQuery & query) const
{
    Q_D(const WBackendUniversal);

    WBackendUniversalScript script(d->data.extractSource);

    if (script.isValid() == false) return WBackendNetSource();

    WBackendUniversalParameters parameters(script, d->global);

    WBackendNetSource reply;

    d->applySourceParameters(&parameters, data, query, reply);

    script.run(&parameters);

    d->applySourceResults(&parameters, &reply);

    return reply;
}

/* Q_INVOKABLE virtual */
WBackendNetTrack WBackendUniversal::extractTrack(const QByteArray       & data,
                                                 const WBackendNetQuery & query) const
{
    Q_D(const WBackendUniversal);

    WBackendUniversalScript script(d->data.extractTrack);

    if (script.isValid() == false) return WBackendNetTrack();

    WBackendUniversalParameters parameters(script, d->global);

    WBackendNetTrack reply;

    d->applyTrackParameters(&parameters, data, query, reply);

    script.run(&parameters);

    d->applyTrackResults(&parameters, &reply);

    return reply;
}

/* Q_INVOKABLE virtual */
WBackendNetPlaylist WBackendUniversal::extractPlaylist(const QByteArray       & data,
                                                       const WBackendNetQuery & query) const
{
    Q_D(const WBackendUniversal);

    WBackendUniversalScript script(d->data.extractPlaylist);

    if (script.isValid() == false) return WBackendNetPlaylist();

    WBackendUniversalParameters parameters(script, d->global);

    WBackendNetPlaylist reply;

    d->applyPlaylistParameters(&parameters, data, query, reply);

    script.run(&parameters);

    d->applyPlaylistResults(&parameters, &reply);

    return reply;
}

/* Q_INVOKABLE virtual */
WBackendNetFolder WBackendUniversal::extractFolder(const QByteArray       & data,
                                                   const WBackendNetQuery & query) const
{
    Q_D(const WBackendUniversal);

    WBackendUniversalScript script(d->data.extractFolder);

    if (script.isValid() == false) return WBackendNetFolder();

    WBackendUniversalParameters parameters(script, d->global);

    WBackendNetFolder reply;

    d->applyFolderParameters(&parameters, data, query, reply);

    script.run(&parameters);

    d->applyFolderResults(&parameters, &reply);

    return reply;
}

/* Q_INVOKABLE virtual */
WBackendNetItem WBackendUniversal::extractItem(const QByteArray       & data,
                                               const WBackendNetQuery & query) const
{
    Q_D(const WBackendUniversal);

    WBackendUniversalScript script(d->data.extractItem);

    if (script.isValid() == false) return WBackendNetItem();

    WBackendUniversalParameters parameters(script, d->global);

    WBackendNetItem reply;

    d->applyItemParameters(&parameters, data, query, reply);

    script.run(&parameters);

    d->applyItemResults(&parameters, &reply);

    return reply;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */
void WBackendUniversal::applySource(const WBackendNetQuery  &,
                                    const WBackendNetSource & source)
{
    Q_D(WBackendUniversal); d->global = source.backup;
}

/* Q_INVOKABLE virtual */
void WBackendUniversal::applyTrack(const WBackendNetQuery &,
                                   const WBackendNetTrack & track)
{
    Q_D(WBackendUniversal); d->global = track.backup;
}

/* Q_INVOKABLE virtual */
void WBackendUniversal::applyPlaylist(const WBackendNetQuery    &,
                                      const WBackendNetPlaylist & playlist)
{
    Q_D(WBackendUniversal); d->global = playlist.backup;
}

/* Q_INVOKABLE virtual */
void WBackendUniversal::applyFolder(const WBackendNetQuery  &,
                                    const WBackendNetFolder & folder)
{
    Q_D(WBackendUniversal); d->global = folder.backup;
}

/* Q_INVOKABLE virtual */
void WBackendUniversal::applyItem(const WBackendNetQuery &,
                                  const WBackendNetItem  & item)
{
    Q_D(WBackendUniversal); d->global = item.backup;
}

#endif // SK_NO_BACKENDUNIVERSAL

#include "WBackendUniversal.moc"
