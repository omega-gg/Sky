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

#include "WRegExp.h"

#ifndef SK_NO_REGEXP

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/Sk_p>

class SK_CORE_EXPORT WRegExpPrivate : public WPrivate
{
public:
    WRegExpPrivate(WRegExp * p);

    void init();

#ifndef QT_4
public: // Variables
    QRegularExpressionMatch match;
#endif

protected:
    W_DECLARE_PUBLIC(WRegExp)
};

//-------------------------------------------------------------------------------------------------

WRegExpPrivate::WRegExpPrivate(WRegExp * p) : WPrivate(p) {}

void WRegExpPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

WRegExp::WRegExp()
#ifdef QT_4
    : QRegExp(), WPrivatable(new WRegExpPrivate(this)) {}
#else
    : QRegularExpression(), WPrivatable(new WRegExpPrivate(this)) {}
#endif

/* explicit */ WRegExp::WRegExp(const QString & pattern)
#ifdef QT_4
    : QRegExp(pattern), WPrivatable(new WRegExpPrivate(this)) {}
#else
    : QRegularExpression(pattern), WPrivatable(new WRegExpPrivate(this)) {}
#endif

#ifndef QT_4

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

bool WRegExp::exactMatch(const QString & string) const
{
    Q_D(const WRegExp);

    const_cast<WRegExpPrivate *> (d)->match = match(string);

    return d->match.hasMatch();
}

int WRegExp::indexIn(const QString & string, int from) const
{
    Q_D(const WRegExp);

    const_cast<WRegExpPrivate *> (d)->match = match(string, from);

    return d->match.capturedStart();
}

int WRegExp::lastIndexIn(const QString & string, int from) const
{
    Q_D(const WRegExp);

    const_cast<WRegExpPrivate *> (d)->match = match(string, from);

    return d->match.lastCapturedIndex();
}

//-------------------------------------------------------------------------------------------------

QString WRegExp::cap(int index) const
{
    Q_D(const WRegExp);

    return d->match.captured(index);
}

QStringList WRegExp::capturedTexts() const
{
    Q_D(const WRegExp);

    return d->match.capturedTexts();
}

//-------------------------------------------------------------------------------------------------

int WRegExp::matchedLength() const
{
    Q_D(const WRegExp);

    return d->match.capturedLength();
}

#endif // QT_4

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* static */ bool WRegExp::isRegExp(const QVariant & variant)
{
#ifdef QT_4
    return (variant.type() == QVariant::RegExp);
#elif defined(QT_5)
    return (variant.type() == QVariant::RegularExpression);
#else
    return (variant.typeId() == QMetaType::QRegularExpression);
#endif
}

/* static */ WRegExp WRegExp::fromVariant(const QVariant & variant)
{
#ifdef QT_4
    return variant.toRegExp();
#else
    return variant.toRegularExpression();
#endif
}

//-------------------------------------------------------------------------------------------------
// Operators
//-------------------------------------------------------------------------------------------------

WRegExp::WRegExp(const WRegExp & other)
#ifdef QT_4
    : QRegExp(), WPrivatable(new WRegExpPrivate(this))
#else
    : QRegularExpression(), WPrivatable(new WRegExpPrivate(this))
#endif
{
    *this = other;
}

bool WRegExp::operator==(const WRegExp & other) const
{
#ifdef QT_4
    return QRegExp::operator==(static_cast<const QRegExp &> (other));
#else
    return QRegularExpression::operator==(static_cast<const QRegularExpression &> (other));
#endif
}

WRegExp & WRegExp::operator=(const WRegExp & other)
{
#ifdef QT_4
    QRegExp::operator=(static_cast<const QRegExp &> (other));
#else
    QRegularExpression::operator=(static_cast<const QRegularExpression &> (other));
#endif

#ifndef QT_4
    Q_D(WRegExp);

    d->match = other.d_func()->match;
#endif

    return *this;
}

#ifdef QT_4

WRegExp::WRegExp(const QRegExp & other)
    : QRegExp(), WPrivatable(new WRegExpPrivate(this))
{
    *this = other;
}

bool WRegExp::operator==(const QRegExp & other) const
{
    return QRegExp::operator==(other);
}

WRegExp & WRegExp::operator=(const QRegExp & other)
{
    QRegExp::operator=(other);

    return *this;
}

#else

WRegExp::WRegExp(const QRegularExpression & other)
    : QRegularExpression(), WPrivatable(new WRegExpPrivate(this))
{
    *this = other;
}

bool WRegExp::operator==(const QRegularExpression & other) const
{
    return QRegularExpression::operator==(other);
}

WRegExp & WRegExp::operator=(const QRegularExpression & other)
{
    Q_D(WRegExp);

    QRegularExpression::operator=(other);

    d->match = QRegularExpressionMatch();

    return *this;
}

#endif // NOT_QT_4

#endif // SK_NO_REGEXP
