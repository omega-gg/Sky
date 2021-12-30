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
    QList<WYamlNode> children;

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

public: // Static functions
    Q_INVOKABLE static bool extractBool(const WYamlReader & reader, const QString & key);
    Q_INVOKABLE static int  extractInt (const WYamlReader & reader, const QString & key);

    Q_INVOKABLE static QString extractString(const WYamlReader & reader, const QString & key);

private:
    W_DECLARE_PRIVATE(WYamlReader)
};

#endif // WYAMLREADER_H
