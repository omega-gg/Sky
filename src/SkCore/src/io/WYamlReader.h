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
#include <QDateTime>
#ifdef QT_4
#include <QStringList>
#endif

// Sk includes
#include <Sk>

// Forward declarations
class WYamlReaderPrivate;
class WYamlNode;

//-------------------------------------------------------------------------------------------------
// WYamlNodeBase
//-------------------------------------------------------------------------------------------------

class SK_CORE_EXPORT WYamlNodeBase
{
public:
    WYamlNodeBase();

    virtual ~WYamlNodeBase();

public: // Interface
    void append(const WYamlNode & node);

    const WYamlNode * at(const QString & key) const;

    QList<WYamlNode> shuffled(uint seed) const;

    void dump() const;

    void clear();

    //---------------------------------------------------------------------------------------------

    bool extractBool(const QString & key) const;

    int extractInt  (const QString & key, int defaultValue = 0) const;
    int extractMsecs(const QString & key, int defaultValue = 0) const;

    QString extractString(const QString & key) const;

    QDateTime extractDate(const QString & key, Qt::DateFormat format = Qt::ISODate) const;

    QStringList extractList(const QString & key) const;

public: // Variables
    QList<WYamlNode> children;
};

//-------------------------------------------------------------------------------------------------
// WYamlNode
//-------------------------------------------------------------------------------------------------

class SK_CORE_EXPORT WYamlNode : public WYamlNodeBase
{
public:
    explicit WYamlNode(const QString & key = QString());

public: // Interface
    void dump(int indent = 0) const;

public: // Operators
    WYamlNode(const WYamlNode & other);

    WYamlNode & operator=(const WYamlNode & other);

public: // Variables
    QString key;
    QString value;
};

//-------------------------------------------------------------------------------------------------
// WYamlReader
//-------------------------------------------------------------------------------------------------

class SK_CORE_EXPORT WYamlReader : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(WYamlNodeBase node READ node CONSTANT)

    Q_PROPERTY(int count READ count CONSTANT)

public:
    explicit WYamlReader(QObject * parent = NULL);

    WYamlReader(const QByteArray & data, QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE void load(const QByteArray & data);

    Q_INVOKABLE const WYamlNode * at(const QString & key) const;

    Q_INVOKABLE void dump() const;

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE bool extractBool(const QString & key) const;

    Q_INVOKABLE int extractInt(const QString & key, int defaultValue = 0) const;

    // NOTE: This extracts msecs as a raw value or mm:ss.zzz and hh:mm:ss.zzz format with optional
    //       zzz for msecs.
    Q_INVOKABLE int extractMsecs(const QString & key, int defaultValue = 0) const;

    Q_INVOKABLE QString extractString(const QString & key) const;

    Q_INVOKABLE QDateTime extractDate(const QString & key,
                                      Qt::DateFormat format = Qt::ISODate) const;

    Q_INVOKABLE QStringList extractList(const QString & key) const;

public: // Properties
    const WYamlNodeBase & node() const;

    int count() const;

private:
    W_DECLARE_PRIVATE(WYamlReader)
};

#endif // WYAMLREADER_H
