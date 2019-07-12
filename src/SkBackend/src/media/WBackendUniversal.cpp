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

static const QString BACKENDUNIVERSAL_FUNCTIONS = "IF|AND|OR|ELSE|ELIF|FI|EQUALS|NOT_EQUALS|LOWER|"
                                                  "GREATER|RETURN|SET|APPEND|READ|LENGTH|INDEX_OF|"
                                                  "INDEX_OF_REGEXP|INDEX_END|INDEX_END_REGEXP|"
                                                  "STARTS_WITH|STARTS_WITH_REGEXP|REMOVE_CHARS|"
                                                  "REMOVE_PREFIX|EXTRACT_URL_ELEMENT|ADD_QUERY|"
                                                  "EXTRACT_JSON|EXTRACT_JSON_HTML|PRINT";

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
        if (nodes.count() != 2) return false;

        QString stringA = getString(parameters, 0);
        QString stringB = getString(parameters, 1);

#ifdef SK_BACKEND_LOG
        qDebug("EQUALS [%d]", (stringA == stringB));
#endif

        return (stringA == stringB);
    }
    else if (data == "NOT_EQUALS")
    {
        if (nodes.count() != 2) return false;

        QString stringA = getString(parameters, 0);
        QString stringB = getString(parameters, 1);

#ifdef SK_BACKEND_LOG
        qDebug("NOT_EQUALS [%d]", (stringA != stringB));
#endif

        return (stringA != stringB);
    }
    else if (data == "LOWER")
    {
        if (nodes.count() != 2) return false;

        float floatA = getFloat(parameters, 0);
        float floatB = getFloat(parameters, 1);

#ifdef SK_BACKEND_LOG
        qDebug("LOWER [%d]", (floatA < floatB));
#endif

        return (floatA < floatB);
    }
    else if (data == "GREATER")
    {
        if (nodes.count() != 2) return false;

        float floatA = getFloat(parameters, 0);
        float floatB = getFloat(parameters, 1);

#ifdef SK_BACKEND_LOG
        qDebug("GREATER [%d]", (floatA > floatB));
#endif

        return (floatA > floatB);
    }
    else if (data == "SET")
    {
        if (nodes.count() != 2) return false;

        QVariant * key = getValue(parameters, 0);

        if (key == NULL) return false;

        *key = getString(parameters, 1);

#ifdef SK_BACKEND_LOG
        qDebug("SET [%s]", key->toString().C_STR);
#endif

        return true;
    }
    else if (data == "APPEND")
    {
        if (nodes.count() != 2) return false;

        QString string = getString(parameters, 0);

        string.append(getString(parameters, 1));

#ifdef SK_BACKEND_LOG
        qDebug("APPEND [%s]", string.C_STR);
#endif

        return string;
    }
    else if (data == "READ")
    {
        int count = nodes.count();

#ifdef SK_BACKEND_LOG
        qDebug("READ");
#endif

        if (count == 1)
        {
            return Sk::readUtf8(getByteArray(parameters, 0));
        }
        else if (count == 2)
        {
            return Sk::readCodec(getByteArray(parameters, 0), getString(parameters, 1));
        }
    }
    else if (data == "INDEX_OF")
    {
        if (nodes.count() != 2) return -1;

        QString string = getString(parameters, 0);

#ifdef SK_BACKEND_LOG
        qDebug("INDEX_OF [%d]", string.indexOf(getString(parameters, 1)));
#endif

        return string.indexOf(getString(parameters, 1));
    }
    else if (data == "INDEX_OF_REGEXP")
    {
        if (nodes.count() != 2) return -1;

        QString string = getString(parameters, 0);

        QRegExp regExp = getRegExp(parameters, 1);

#ifdef SK_BACKEND_LOG
        qDebug("INDEX_OF_REGEXP [%d]", string.indexOf(regExp));
#endif

        return string.indexOf(regExp);
    }
    else if (data == "INDEX_END")
    {
        if (nodes.count() != 2) return -1;

        QString stringA = getString(parameters, 0);
        QString stringB = getString(parameters, 1);

#ifdef SK_BACKEND_LOG
        qDebug("INDEX_OF [%d]", stringA.indexOf(stringB) + stringB.length());
#endif

        return stringA.indexOf(stringB) + stringB.length();
    }
    else if (data == "INDEX_END_REGEXP")
    {
        if (nodes.count() != 2) return -1;

        QString string = getString(parameters, 0);

        QRegExp regExp = getRegExp(parameters, 1);

#ifdef SK_BACKEND_LOG
        qDebug("INDEX_OF_REGEXP [%d]", string.indexOf(regExp) + regExp.matchedLength());
#endif

        return string.indexOf(regExp) + regExp.matchedLength();
    }
    else if (data == "STARTS_WITH")
    {
        if (nodes.count() != 2) return false;

        QString string = getString(parameters, 0);

#ifdef SK_BACKEND_LOG
        qDebug("STARTS_WITH [%d]", string.startsWith(getString(parameters, 1)));
#endif

        return string.startsWith(getString(parameters, 1));
    }
    else if (data == "STARTS_WITH_REGEXP")
    {
        if (nodes.count() != 2) return false;

        QString string = getString(parameters, 0);

        QRegExp regExp = getRegExp(parameters, 1);

        if (string.indexOf(regExp) == 0)
        {
#ifdef SK_BACKEND_LOG
            qDebug("STARTS_WITH_REGEXP true");
#endif

            return true;
        }
        else
        {
#ifdef SK_BACKEND_LOG
            qDebug("STARTS_WITH_REGEXP false");
#endif

            return false;
        }
    }
    else if (data == "REMOVE_CHARS")
    {
        if (nodes.count() != 3) return QString();

        QVariant * key = getValue(parameters, 0);

        if (key == NULL) return QString();

        *key = key->toString().remove(getInt(parameters, 1), getInt(parameters, 2));

#ifdef SK_BACKEND_LOG
        qDebug("REMOVE_CHARS [%s]", key->toString().C_STR);
#endif

        return *key;
    }
    else if (data == "REMOVE_PREFIX")
    {
        if (nodes.count() != 1) return QString();

        QVariant * key = getValue(parameters, 0);

        if (key == NULL) return QString();

        *key = WControllerNetwork::removeUrlPrefix(key->toString());

#ifdef SK_BACKEND_LOG
        qDebug("REMOVE_PREFIX [%s]", key->toString().C_STR);
#endif

        return *key;
    }
    else if (data == "EXTRACT_URL_ELEMENT")
    {
        if (nodes.count() != 2) return QString();

        QString string = getString(parameters, 0);

        int from = getInt(parameters, 1);

#ifdef SK_BACKEND_LOG
        qDebug("EXTRACT_URL_ELEMENT [%s]",
               WControllerNetwork::extractUrlElement(string, from).C_STR);
#endif

        return WControllerNetwork::extractUrlElement(string, from);
    }
    else if (data == "ADD_QUERY")
    {
        if (nodes.count() != 3) return QString();

        QVariant * key = getValue(parameters, 0);

        if (key == NULL) return QString();

        QUrl url(key->toString());

#ifdef QT_4
        url.addQueryItem(getString(parameters, 1), getString(parameters, 2));
#else
        QUrlQuery urlQuery(url);

        urlQuery.addQueryItem(getString(parameters, 1), getString(parameters, 2));

        url.setQuery(urlQuery);
#endif

        *key = url;

#ifdef SK_BACKEND_LOG
        qDebug("ADD_QUERY [%s]", key->toString().C_STR);
#endif

        return *key;
    }
    else if (data == "EXTRACT_JSON")
    {
        int count = nodes.count();

#ifdef SK_BACKEND_LOG
        qDebug("EXTRACT_JSON");
#endif

        if (count == 2)
        {
            return WControllerNetwork::extractJson(getString(parameters, 0),
                                                   getString(parameters, 1));
        }
        else if (count == 3)
        {
            return WControllerNetwork::extractJson(getString(parameters, 0),
                                                   getString(parameters, 1),
                                                   getInt(parameters, 2));
        }
    }
    else if (data == "EXTRACT_JSON_HTML")
    {
        int count = nodes.count();

#ifdef SK_BACKEND_LOG
        qDebug("EXTRACT_JSON_HTML");
#endif

        if (count == 2)
        {
            return WControllerNetwork::extractJsonHtml(getString(parameters, 0),
                                                       getString(parameters, 1));
        }
        else if (count == 3)
        {
            return WControllerNetwork::extractJsonHtml(getString(parameters, 0),
                                                       getString(parameters, 1),
                                                       getInt(parameters, 2));
        }
    }
    else if (data == "PRINT")
    {
        if (nodes.count() != 1) return QString();

        qDebug("PRINT [%s]", getString(parameters, 0).C_STR);
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

    if (type == WBackendUniversalNode::Variable)
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

int WBackendUniversalNode::getInt(WBackendUniversalParameters * parameters, int index) const
{
    return getString(parameters, index).toInt();
}

float WBackendUniversalNode::getFloat(WBackendUniversalParameters * parameters, int index) const
{
    return getString(parameters, index).toFloat();
}

//-------------------------------------------------------------------------------------------------

QByteArray WBackendUniversalNode::getByteArray(WBackendUniversalParameters * parameters,
                                               int                           index) const
{
    QByteArray result;

    const WBackendUniversalNode & node = nodes.at(index);

    if (node.type == Variable)
    {
        result = node.data.toUtf8();
    }
    else // if (node.type == Function)
    {
        result = node.run(parameters).toByteArray();
    }

    if (result.startsWith('"') == false)
    {
        const QVariant * variant = parameters->valueConst(result);

        if (variant)
        {
             return variant->toByteArray();
        }
        else return result;
    }
    else return result.mid(1, result.length() - 2);
}

QString WBackendUniversalNode::getString(WBackendUniversalParameters * parameters,
                                         int                           index) const
{
    QString result;

    const WBackendUniversalNode & node = nodes.at(index);

    if (node.type == Variable)
    {
        result = node.data;
    }
    else // if (node.type == Function)
    {
        result = node.run(parameters).toString();
    }

    if (result.startsWith('"') == false)
    {
        const QVariant * variant = parameters->valueConst(result);

        if (variant)
        {
             return variant->toString();
        }
        else return result;
    }
    else return result.mid(1, result.length() - 2);
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
    QString result;

    const WBackendUniversalNode & node = nodes.at(index);

    if (node.type == Variable)
    {
        result = node.data;
    }
    else // if (node.type == Function)
    {
        result = node.run(parameters).toString();
    }

    return parameters->value(result);
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

    int index = 0;

    while (index < nodes.count())
    {
        const WBackendUniversalNode & node = nodes.at(index);

        const QString & data = node.data;

        if (data == "AND" || data == "OR")
        {
            index++;

            continue;
        }

        if (data == "IF")
        {
            if (condition == 0)
            {
#ifdef SK_BACKEND_LOG
                qDebug("SKIP IF");
#endif

                skipCondition(&index);

                index++;

                continue;
            }

            if (condition != -1)
            {
                conditions.append(condition);
            }

            condition = getCondition(parameters, node, &index);

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

                    index++;
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

                    index++;
                }
            }
            else
            {
#ifdef SK_BACKEND_LOG
                qDebug("ELIF");
#endif

                condition = getCondition(parameters, node, &index);
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
            index++;

            continue;
        }
        else if (data == "RETURN")
        {
            int count = node.nodes.count();

            if (count > 0)
            {
                QString string = node.getString(parameters, 0);

                QVariant * key = parameters->value(string);

                if (key == NULL)
                {
#ifdef SK_BACKEND_LOG
                    qDebug("RETURN [%s]", string.C_STR);
#endif

                    return string;
                }
                else
                {
#ifdef SK_BACKEND_LOG
                    qDebug("RETURN [%s]", key->toString().C_STR);
#endif

                    return *key;
                }
            }
            else
            {
#ifdef SK_BACKEND_LOG
                qDebug("RETURN");
#endif

                return true;
            }
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
    else
    {
        WBackendUniversalNode child(WBackendUniversalNode::Variable);

        if (character == '"')
        {
             child.data = extractString(string);
        }
        else child.data = extractWord(string);

        node->nodes.append(child);
    }

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
        result = string->mid(1, index - 1);

        string->remove(0, index + 1);
    }

    /*while (string->at(index - 1) == '\\')
    {
        int at = string->indexOf('"', index + 1);

        if (at == -1) break;

        index = at;
    }

    result = string->mid(1, index - 1);

    result.replace("\\\"", "\"");

    string->remove(0, index + 1);*/

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

WBackendUniversalParameters::WBackendUniversalParameters(const WBackendUniversalScript & script)
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

    WBackendUniversalParameters parameters(script);

    applyQueryParameters(&parameters, url);

    script.run(&parameters);

    applyQuery(query, &parameters);
}

//-------------------------------------------------------------------------------------------------

void WBackendUniversalPrivate::applyQueryParameters(WBackendUniversalParameters * parameters,
                                                    const QString               & url) const
{
    parameters->add("url", url);

    parameters->add("id");

    parameters->add("data");

    parameters->add("clearItems");
    parameters->add("cookies");
    parameters->add("header");
    parameters->add("skipError");

    parameters->add("delay");
    parameters->add("timeout");
}

void WBackendUniversalPrivate::applyQuery(WBackendNetQuery            * query,
                                          WBackendUniversalParameters * parameters) const
{
    query->url = parameters->value("url")->toString();

    query->id = parameters->value("id")->toInt();

    query->data = *(parameters->value("data"));

    query->clearItems = parameters->value("clearItems")->toBool();
    query->cookies    = parameters->value("cookies")   ->toBool();
    query->header     = parameters->value("header")    ->toBool();
    query->skipError  = parameters->value("skipError") ->toBool();

    query->delay   = parameters->value("delay")  ->toInt();
    query->timeout = parameters->value("timeout")->toInt();
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

    qDebug("checkValidUrl [%d]", validUrl);

    qDebug("getTrackId [%s]", trackId.C_STR);

    qDebug("getPlaylistInfo [%d] [%s]", info.type, info.id.C_STR);

    qDebug("getUrlTrack [%s]", urlTrack.C_STR);

    qDebug("getUrlPlaylist [%s]", urlPlaylist.C_STR);

    qDebug("getQuerySource   [%s]", querySource  .url.C_STR);
    qDebug("getQueryTrack    [%s]", queryTrack   .url.C_STR);
    qDebug("getQueryPlaylist [%s]", queryPlaylist.url.C_STR);

    qDebug("createQuery [%s]", createQuery.url.C_STR);
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

    WBackendUniversalParameters parameters(script);

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

    WBackendUniversalParameters parameters(script);

    parameters.add("url", url);

    return script.run(&parameters).toString();
}

/* Q_INVOKABLE virtual */
WAbstractBackend::Output WBackendUniversal::getTrackOutput(const QString & url) const
{
    Q_D(const WBackendUniversal);

    WBackendUniversalScript script(d->data.trackId);

    if (script.isValid() == false) return WAbstractBackend::OutputMedia;

    WBackendUniversalParameters parameters(script);

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

    WBackendUniversalParameters parameters(script);

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

    WBackendUniversalParameters parameters(script);

    parameters.add("id", id);

    return script.run(&parameters).toString();
}

/* Q_INVOKABLE virtual */
QString WBackendUniversal::getUrlPlaylist(const WBackendNetPlaylistInfo & info) const
{
    Q_D(const WBackendUniversal);

    WBackendUniversalScript script(d->data.urlPlaylist);

    if (script.isValid() == false) return QString();

    WBackendUniversalParameters parameters(script);

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

    WBackendUniversalParameters parameters(script);

    d->applyQueryParameters(&parameters, QString());

    parameters.add("method", method);
    parameters.add("label",  label);
    parameters.add("q",      q);

    script.run(&parameters);

    WBackendNetQuery query;

    d->applyQuery(&query, &parameters);

    return query;
}

#endif // SK_NO_BACKENDUNIVERSAL

#include "WBackendUniversal.moc"
