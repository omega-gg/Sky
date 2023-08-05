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

#include "WYamlReader.h"

#ifndef SK_NO_YAMLREADER

// Sk includes
#include <WControllerApplication>

//=================================================================================================
// WYamlNodeBase
//=================================================================================================

void WYamlNodeBase::append(const WYamlNode & node)
{
    children.append(node);
}

const WYamlNode * WYamlNodeBase::at(const QString & key) const
{
    foreach (const WYamlNode & node, children)
    {
        if (node.key == key) return &node;
    }

    return NULL;
}

void WYamlNodeBase::dump() const
{
    foreach (const WYamlNode & node, children)
    {
        node.dump();

        qDebug(" ");
    }
}

void WYamlNodeBase::clear()
{
    children.clear();
}

//---------------------------------------------------------------------------------------------

bool WYamlNodeBase::extractBool(const QString & key) const
{
    const WYamlNode * node = at(key);

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

int WYamlNodeBase::extractInt(const QString & key, int defaultValue) const
{
    const WYamlNode * node = at(key);

    if (node)
    {
         return node->value.toInt();
    }
    else return defaultValue;
}

int WYamlNodeBase::extractMsecs(const QString & key, int defaultValue) const
{
    const WYamlNode * node = at(key);

    if (node == NULL) return defaultValue;

    QStringList list = node->value.split(':');

    int count = list.count();

    if (count == 1)
    {
        return list.first().toInt();
    }
    else if (count == 2)
    {
        QStringList seconds = list.last().split('.');

        int msecs = list.at(0).toInt() * 60000 // 1 minute
                    +
                    seconds.first().toInt() * 1000;

        if (seconds.count() == 2)
        {
             return msecs + seconds.last().toInt();
        }
        else return msecs;
    }
    else if (count == 3)
    {
        QStringList seconds = list.last().split('.');

        int msecs = list.at(0).toInt() * 3600000 // 1 hour
                    +
                    list.at(1).toInt() * 60000 // 1 hour
                    +
                    seconds.first().toInt() * 1000;

        if (seconds.count() == 2)
        {
             return msecs + seconds.last().toInt();
        }
        else return msecs;
    }
    else return defaultValue;
}

QString WYamlNodeBase::extractString(const QString & key) const
{
    const WYamlNode * node = at(key);

    if (node)
    {
         return node->value;
    }
    else return QString();
}

QDateTime WYamlNodeBase::extractDate(const QString & key, Qt::DateFormat format) const
{
    const WYamlNode * node = at(key);

    if (node)
    {
         return QDateTime::fromString(node->value, format);
    }
    else return QDateTime();
}

QStringList WYamlNodeBase::extractList(const QString & key) const
{
    QString string = extractString(key);

    return string.split('\n');
}

//=================================================================================================
// WYamlNode
//=================================================================================================

WYamlNode::WYamlNode(const QString & key) : WYamlNodeBase()
{
    this->key = key;
}

void WYamlNode::dump(int indent) const
{
    QString string;

    for (int i = 0; i < indent; i++)
    {
        string.append(' ');
    }

    if (children.isEmpty() == false)
    {
        qDebug("%s[%s]", string.C_STR, key.C_STR);

        indent += 4;

        foreach (const WYamlNode & node, children)
        {
            node.dump(indent);
        }
    }
    else qDebug("%s[%s] [%s]", string.C_STR, key.C_STR, value.C_STR);
}

//=================================================================================================
// WYamlReaderPrivate
//=================================================================================================

#include <private/Sk_p>

class SK_CORE_EXPORT WYamlReaderPrivate : public WPrivate
{
public: // Enums
    enum Type
    {
        Nodes,
        Paragraph,
        Line
    };

public:
    WYamlReaderPrivate(WYamlReader * p);

    void init();

public: // Functions
    QString extractKey(QString * string) const;

    void extractNode(WYamlNode * node, QString * content, QString * string) const;

    void extractChilds(WYamlNode * node, QString * content) const;

    void extractValue(Type type, WYamlNode * node, QString * content) const;

    QString extractBlock(QString * content, int indent) const;

    QString extractText(QString * content, int from) const;

    int getIndent(const QString * content) const;

public: // Variables
    WYamlNodeBase node;

protected:
    W_DECLARE_PUBLIC(WYamlReader)
};

//-------------------------------------------------------------------------------------------------

WYamlReaderPrivate::WYamlReaderPrivate(WYamlReader * p) : WPrivate(p) {}

void WYamlReaderPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

QString WYamlReaderPrivate::extractKey(QString * string) const
{
    int index = string->indexOf(':');

    if (index == -1) return QString();

    QString result = string->mid(0, index);

    string->remove(0, index + 1);

    Sk::skipSpaces(string);

    return result;
}

//-------------------------------------------------------------------------------------------------

void WYamlReaderPrivate::extractNode(WYamlNode * node, QString * content, QString * string) const
{
    if (string->isEmpty() == false)
    {
        QChar character = string->at(0);

        if (character == '|')
        {
            extractValue(WYamlReaderPrivate::Paragraph, node, content);
        }
        else if (character == '>')
        {
            extractValue(WYamlReaderPrivate::Line, node, content);
        }
        else node->value = *string;
    }
    else extractChilds(node, content);
}

void WYamlReaderPrivate::extractChilds(WYamlNode * node, QString * content) const
{
    int indent = getIndent(content);

    while (content->isEmpty() == false)
    {
        QString string = Sk::extractLine(content, indent);

        if (string.isEmpty()) continue;

        QChar character = string.at(0);

        if (character == '#') continue;

        QString key = extractKey(&string);

        if (key.isEmpty()) return;

        WYamlNode child(key);

        extractNode(&child, content, &string);

        node->children.append(child);

        if (getIndent(content) != indent) return;
    }
}

void WYamlReaderPrivate::extractValue(Type type, WYamlNode * node, QString * content) const
{
    int indent = getIndent(content);

    QString result = extractBlock(content, indent);

    if (type == Line)
    {
        result.replace('\n', ' ');
    }

    node->value = result;
}

//-------------------------------------------------------------------------------------------------

QString WYamlReaderPrivate::extractBlock(QString * content, int indent) const
{
    QString result;

    while (content->isEmpty() == false)
    {
        QString string = extractText(content, indent);

        result.append(string);

        if (getIndent(content) < indent) break;
    }

    if (result.endsWith('\n'))
    {
        result.chop(1);
    }

    return result;
}

QString WYamlReaderPrivate::extractText(QString * content, int from) const
{
    QString result;

    int index = content->indexOf('\n', from);

    if (index == -1)
    {
        result = content->mid(from);

        content->clear();
    }
    else
    {
        index++;

        result = content->mid(from, index - from);

        content->remove(0, index);
    }

    return result;
}

//-------------------------------------------------------------------------------------------------

int WYamlReaderPrivate::getIndent(const QString * content) const
{
    int indent = 0;

    while (indent < content->length() && content->at(indent) == ' ')
    {
        indent++;
    }

    return indent;
}

//=================================================================================================
// WYamlReader
//=================================================================================================

/* explicit */ WYamlReader::WYamlReader(QObject * parent)
    : QObject(parent), WPrivatable(new WYamlReaderPrivate(this))
{
    Q_D(WYamlReader); d->init();
}

WYamlReader::WYamlReader(const QByteArray & data, QObject * parent)
    : QObject(parent), WPrivatable(new WYamlReaderPrivate(this))
{
    Q_D(WYamlReader);

    d->init();

    load(data);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WYamlReader::load(const QByteArray & data)
{
    Q_D(WYamlReader);

    d->node.clear();

    QString content = Sk::readUtf8(data);

    content.remove('\r');

    while (content.isEmpty() == false)
    {
        QString string = Sk::extractLine(&content);

        Sk::skipSpaces(&string);

        if (string.isEmpty()) continue;

        QChar character = string.at(0);

        if (character == '#') continue;

        QString key = d->extractKey(&string);

        if (key.isEmpty()) continue;

        WYamlNode node(key);

        d->extractNode(&node, &content, &string);

        d->node.append(node);
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ const WYamlNode * WYamlReader::at(const QString & key) const
{
    Q_D(const WYamlReader);

    return d->node.at(key);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WYamlReader::dump() const
{
    Q_D(const WYamlReader); d->node.dump();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WYamlReader::extractBool(const QString & key) const
{
    Q_D(const WYamlReader);

    return d->node.extractBool(key);
}

/* Q_INVOKABLE */ int WYamlReader::extractInt(const QString & key, int defaultValue) const
{
    Q_D(const WYamlReader);

    return d->node.extractInt(key, defaultValue);
}

/* Q_INVOKABLE */ int WYamlReader::extractMsecs(const QString & key, int defaultValue) const
{
    Q_D(const WYamlReader);

    return d->node.extractMsecs(key, defaultValue);
}

/* Q_INVOKABLE */ QString WYamlReader::extractString(const QString & key) const
{
    Q_D(const WYamlReader);

    return d->node.extractString(key);
}

/* Q_INVOKABLE */ QDateTime WYamlReader::extractDate(const QString & key,
                                                     Qt::DateFormat  format) const
{
    Q_D(const WYamlReader);

    return d->node.extractDate(key, format);
}

/* Q_INVOKABLE */ QStringList WYamlReader::extractList(const QString & key) const
{
    Q_D(const WYamlReader);

    return d->node.extractList(key);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

const WYamlNodeBase & WYamlReader::node() const
{
    Q_D(const WYamlReader);

    return d->node;
}

int WYamlReader::count() const
{
    Q_D(const WYamlReader);

    return d->node.children.count();
}

#endif // SK_NO_YAMLREADER
