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

#ifndef WREGEXP_H
#define WREGEXP_H

// Qt includes
#ifdef QT_4
#include <QRegExp>
#include <QVariant>
#else
#include <QRegularExpression>
#endif

// Sk includes
#include <Sk>

#ifndef SK_NO_REGEXP

// Forward declarations
class WRegExpPrivate;

// NOTE Qt5+: When a WRegExp is passed to a QString you won't be able to retrieve matched data.

#ifdef QT_4
class SK_CORE_EXPORT WRegExp : public QRegExp, public WPrivatable
#else
class SK_CORE_EXPORT WRegExp : public QRegularExpression, public WPrivatable
#endif
{
public:
    WRegExp();

    explicit WRegExp(const QString & pattern);

    WRegExp(const QString & pattern, Qt::CaseSensitivity sensitivity);

#ifndef QT_4
public: // Interface
    bool exactMatch(const QString & string) const;

    int indexIn(const QString & string, int from = 0) const;

    int lastIndexIn(const QString & string, int from = 0) const;

    QString cap(int index) const;

    QStringList capturedTexts() const;

    int matchedLength() const;
#endif

public: // Static functions
    static bool isRegExp(const QVariant & variant);

    static WRegExp fromVariant(const QVariant & variant);

public: // Operators
    WRegExp(const WRegExp & other);

    bool operator==(const WRegExp & other) const;

    WRegExp & operator=(const WRegExp & other);

#ifdef QT_4
    WRegExp(const QRegExp & other);

    bool operator==(const QRegExp & other) const;

    WRegExp & operator=(const QRegExp & other);
#else
    WRegExp(const QRegularExpression & other);

    bool operator==(const QRegularExpression & other) const;

    WRegExp & operator=(const QRegularExpression & other);
#endif

#ifndef QT_4
public: // Properties
    Qt::CaseSensitivity caseSensitivity() const;
    void                setCaseSensitivity(Qt::CaseSensitivity sensitivity);
#endif

private:
    W_DECLARE_PRIVATE_COPY(WRegExp)
};

#endif // SK_NO_REGEXP
#endif // WREGEXP_H
