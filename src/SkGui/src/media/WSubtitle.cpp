//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkGui.

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

#include "WSubtitle.h"

#ifndef SK_NO_SUBTITLE

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WSubtitle_p.h"

WSubtitlePrivate::WSubtitlePrivate(WSubtitle * p) : WPrivate(p) {}

void WSubtitlePrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WSubtitle::WSubtitle(const QString & source, const QString & title)
    : WPrivatable(new WSubtitlePrivate(this))
{
    Q_D(WSubtitle);

    d->source = source;
    d->title  = title;

    d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

bool WSubtitle::isValid() const
{
    Q_D(const WSubtitle);

    return (d->source.isEmpty() == false);
}

//-------------------------------------------------------------------------------------------------
// Virtual interface
//-------------------------------------------------------------------------------------------------

/* virtual */ QVariantMap WSubtitle::toMap() const
{
    Q_D(const WSubtitle);

    QVariantMap map;

    map.insert("source", d->source);
    map.insert("title",  d->title);

    return map;
}

//-------------------------------------------------------------------------------------------------
// Operators
//-------------------------------------------------------------------------------------------------

WSubtitle::WSubtitle(const WSubtitle & other) : WPrivatable(new WSubtitlePrivate(this))
{
    Q_D(WSubtitle);

    d->init();

    *this = other;
}

//-------------------------------------------------------------------------------------------------

bool WSubtitle::operator==(const WSubtitle & other) const
{
    Q_D(const WSubtitle);

    const WSubtitlePrivate * op = other.d_func();

    if (d->source == op->source &&
        d->title  == op->title)
    {
         return true;
    }
    else return false;
}

WSubtitle & WSubtitle::operator=(const WSubtitle & other)
{
    Q_D(WSubtitle);

    const WSubtitlePrivate * op = other.d_func();

    d->source = op->source;
    d->title  = op->title;

    return *this;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QString WSubtitle::source() const
{
    Q_D(const WSubtitle); return d->source;
}

void WSubtitle::setSource(const QString & source)
{
    Q_D(WSubtitle); d->source = source;
}

QString WSubtitle::title() const
{
    Q_D(const WSubtitle); return d->title;
}

void WSubtitle::setTitle(const QString & title)
{
    Q_D(WSubtitle); d->title = title.simplified();
}

#endif // SK_NO_SUBTITLE
