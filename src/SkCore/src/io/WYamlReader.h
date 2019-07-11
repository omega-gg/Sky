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

#ifndef WYAMLREADER_H
#define WYAMLREADER_H

// Qt includes
#include <QObject>

// Sk includes
#include <Sk>

// Forward declarations
class WYamlReaderPrivate;

//-------------------------------------------------------------------------------------------------
// WYamlNode
//-------------------------------------------------------------------------------------------------

class SK_CORE_EXPORT WYamlNode
{
public:
    WYamlNode(const QString & key);

public: // Interface
    const WYamlNode * at(const QString & key) const;

    void dump(int indent = 0) const;

public: // Variables
    QList<WYamlNode> childs;

    QString key;
    QString value;
};

//-------------------------------------------------------------------------------------------------
// WYamlReader
//-------------------------------------------------------------------------------------------------

class SK_CORE_EXPORT WYamlReader : public QObject, public WPrivatable
{
    Q_OBJECT

public:
    explicit WYamlReader(QObject * parent = NULL);

    WYamlReader(const QByteArray & data, QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE void load(const QByteArray & data);

    Q_INVOKABLE const WYamlNode * at(const QString & key) const;

    Q_INVOKABLE void dump() const;

private:
    W_DECLARE_PRIVATE(WYamlReader)
};

#endif // WYAMLREADER_H
