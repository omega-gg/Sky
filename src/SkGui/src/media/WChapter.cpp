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

#include "WChapter.h"

#ifndef SK_NO_CHAPTER

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WChapter_p.h"

WChapterPrivate::WChapterPrivate(WChapter * p) : WPrivate(p) {}

void WChapterPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WChapter::WChapter(int time) : WPrivatable(new WChapterPrivate(this))
{
    Q_D(WChapter);

    d->time = time;

    d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

bool WChapter::isValid() const
{
    Q_D(const WChapter);

    return (d->time != -1);
}

//-------------------------------------------------------------------------------------------------
// Operators
//-------------------------------------------------------------------------------------------------

WChapter::WChapter(const WChapter & other) : WPrivatable(new WChapterPrivate(this))
{
    Q_D(WChapter);

    d->init();

    *this = other;
}

//-------------------------------------------------------------------------------------------------

bool WChapter::operator==(const WChapter & other) const
{
    Q_D(const WChapter);

    const WChapterPrivate * op = other.d_func();

    if (d->time == op->time &&

        d->title == op->title &&
        d->cover == op->cover)
    {
         return true;
    }
    else return false;
}

WChapter & WChapter::operator=(const WChapter & other)
{
    Q_D(WChapter);

    const WChapterPrivate * op = other.d_func();

    d->time = op->time;

    d->title = op->title;
    d->cover = op->cover;

    return *this;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

int WChapter::time() const
{
    Q_D(const WChapter); return d->time;
}

void WChapter::setTime(int msec)
{
    Q_D(WChapter); d->time = msec;
}

//-------------------------------------------------------------------------------------------------

QString WChapter::title() const
{
    Q_D(const WChapter); return d->title;
}

void WChapter::setTitle(const QString & title)
{
    Q_D(WChapter); d->title = title.simplified();
}

QString WChapter::cover() const
{
    Q_D(const WChapter); return d->cover;
}

void WChapter::setCover(const QString & cover)
{
    Q_D(WChapter); d->cover = cover;
}

#endif // SK_NO_CHAPTER
