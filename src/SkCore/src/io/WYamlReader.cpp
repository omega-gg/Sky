//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WYamlReader.h"

#ifndef SK_NO_YAMLREADER

// Sk includes
#include <WControllerApplication>

//=================================================================================================
// WYamlNode
//=================================================================================================

WYamlNode::WYamlNode(const QString & key)
{
    this->key = key;
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

const WYamlNode * WYamlNode::at(const QString & key) const
{
    foreach (const WYamlNode & node, childs)
    {
        if (node.key == key) return &node;
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------

void WYamlNode::dump(int indent) const
{
    QString string;

    for (int i = 0; i < indent; i++)
    {
        string.append(' ');
    }

    if (childs.isEmpty() == false)
    {
        qDebug("%s[%s]", string.C_STR, key.C_STR);

        indent += 4;

        foreach (const WYamlNode & node, childs)
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

    QString extractLine(QString * string, int from) const;
    QString extractText(QString * string, int from) const;

    int getIndent(const QString * content) const;

public: // Variables
    QList<WYamlNode> nodes;

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
        else node->value = string;
    }
    else extractChilds(node, content);
}

void WYamlReaderPrivate::extractChilds(WYamlNode * node, QString * content) const
{
    int indent = getIndent(content);

    while (content->isEmpty() == false)
    {
        QString string = extractLine(content, indent);

        if (string.isEmpty()) continue;

        QChar character = string.at(0);

        if (character == '#') continue;

        QString key = extractKey(&string);

        if (key.isEmpty()) return;

        WYamlNode child(key);

        extractNode(&child, content, &string);

        node->childs.append(child);

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

//-------------------------------------------------------------------------------------------------

QString WYamlReaderPrivate::extractLine(QString * string, int from) const
{
    QString result;

    int index = string->indexOf('\n', from);

    if (index == -1)
    {
        result = string->mid(from);

        string->clear();
    }
    else
    {
        result = string->mid(from, index - from);

        string->remove(0, index + 1);
    }

    return result;
}

QString WYamlReaderPrivate::extractText(QString * string, int from) const
{
    QString result;

    int index = string->indexOf('\n', from);

    if (index == -1)
    {
        result = string->mid(from);

        string->clear();
    }
    else
    {
        index++;

        result = string->mid(from, index - from);

        string->remove(0, index);
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

    d->nodes.clear();

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

        d->nodes.append(node);
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ const WYamlNode * WYamlReader::at(const QString & key) const
{
    Q_D(const WYamlReader);

    foreach (const WYamlNode & node, d->nodes)
    {
        if (node.key == key) return &node;
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WYamlReader::dump() const
{
    Q_D(const WYamlReader);

    foreach (const WYamlNode & node, d->nodes)
    {
        node.dump();

        qDebug(" ");
    }
}

#endif // SK_NO_YAMLREADER
