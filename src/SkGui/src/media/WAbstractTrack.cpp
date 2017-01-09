//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WAbstractTrack.h"

#ifndef SK_NO_ABSTRACTTRACK

// Qt includes
#include <QDateTime>

// Sk includes
#include <WControllerScript>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WAbstractTrack_p.h"

WAbstractTrackPrivate::WAbstractTrackPrivate(WAbstractTrack * p) : WPrivate(p) {}

void WAbstractTrackPrivate::init(WAbstractTrack::State state)
{
    this->state = state;

    id = -1;

    duration = -1;

    quality = WAbstractBackend::QualityInvalid;

    playlist = NULL;
}

//-------------------------------------------------------------------------------------------------
// Protected ctor / dtor
//-------------------------------------------------------------------------------------------------

WAbstractTrack::WAbstractTrack(WAbstractTrackPrivate * p, State state) : WPrivatable(p)
{
    Q_D(WAbstractTrack); d->init(state);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

bool WAbstractTrack::isValid() const
{
    Q_D(const WAbstractTrack); return (d->source.isValid());
}

//-------------------------------------------------------------------------------------------------

void WAbstractTrack::copyDataTo(WAbstractTrack * other) const
{
    Q_D(const WAbstractTrack);

    WAbstractTrackPrivate * op = other->d_func();

    if (d->state == Loading)
    {
         op->state = Default;
    }
    else op->state = d->state;

    op->title = d->title;
    op->cover = d->cover;

    op->author = d->author;
    op->feed   = d->feed;

    op->duration = d->duration;

    op->date = d->date;

    op->quality = d->quality;
}

void WAbstractTrack::applyDataTo(WAbstractTrack * other) const
{
    Q_D(const WAbstractTrack);

    WAbstractTrackPrivate * op = other->d_func();

    if (d->title.isEmpty() == false)
    {
        op->title = d->title;
    }

    if (d->cover.isValid())
    {
        op->cover = d->cover;
    }

    if (d->author.isEmpty() == false)
    {
        op->author = d->author;
    }

    if (d->feed.isEmpty() == false)
    {
        op->feed = d->feed;
    }

    if (d->duration != -1)
    {
        op->duration = d->duration;
    }

    if (d->date.isValid())
    {
        op->date = d->date;
    }

    if (d->quality != WAbstractBackend::QualityInvalid)
    {
        op->quality = d->quality;
    }
}

//-------------------------------------------------------------------------------------------------
// Virtual interface
//-------------------------------------------------------------------------------------------------

/* virtual */ QVariantMap WAbstractTrack::toMap() const
{
    Q_D(const WAbstractTrack);

    QVariantMap map;

    map.insert("id", d->id);

    map.insert("state", d->state);

    map.insert("source", d->source);

    map.insert("title", d->title);
    map.insert("cover", d->cover);

    map.insert("author", d->author);
    map.insert("feed",   d->feed);

    map.insert("duration", d->duration);

    map.insert("date", d->date);

    map.insert("quality", d->quality);

    return map;
}

//-------------------------------------------------------------------------------------------------
// Operators
//-------------------------------------------------------------------------------------------------

/* virtual */ bool WAbstractTrack::operator==(const WAbstractTrack & other) const
{
    Q_D(const WAbstractTrack);

    const WAbstractTrackPrivate * op = other.d_func();

    if (d->id == op->id &&

        d->state == op->state &&

        d->source == op->source &&

        d->title == op->title &&
        d->cover == op->cover &&

        d->author == op->author &&
        d->feed   == op->feed   &&

        d->duration == op->duration &&

        d->date == op->date &&

        d->quality == op->quality &&

        d->playlist == op->playlist)
    {
         return true;
    }
    else return false;
}

/* virtual */ WAbstractTrack & WAbstractTrack::operator=(const WAbstractTrack & other)
{
    Q_D(WAbstractTrack);

    const WAbstractTrackPrivate * op = other.d_func();

    d->id = op->id;

    d->state = op->state;

    d->source = op->source;

    d->title = op->title;
    d->cover = op->cover;

    d->author = op->author;
    d->feed   = op->feed;

    d->duration = op->duration;

    d->date = op->date;

    d->quality = op->quality;

    d->playlist = op->playlist;

    return *this;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

int WAbstractTrack::id() const
{
    Q_D(const WAbstractTrack); return d->id;
}

//-------------------------------------------------------------------------------------------------

WAbstractTrack::State WAbstractTrack::state() const
{
    Q_D(const WAbstractTrack); return d->state;
}

void WAbstractTrack::setState(WAbstractTrack::State state)
{
    Q_D(WAbstractTrack); d->state = state;
}

//-------------------------------------------------------------------------------------------------

bool WAbstractTrack::isDefault() const
{
    Q_D(const WAbstractTrack); return (d->state == Default);
}

bool WAbstractTrack::isLoading() const
{
    Q_D(const WAbstractTrack); return (d->state == Loading);
}

bool WAbstractTrack::isLoaded() const
{
    Q_D(const WAbstractTrack); return (d->state == Loaded);
}

//-------------------------------------------------------------------------------------------------

QUrl WAbstractTrack::source() const
{
    Q_D(const WAbstractTrack); return d->source;
}

void WAbstractTrack::setSource(const QUrl & url)
{
    Q_D(WAbstractTrack); d->source = url;
}

//-------------------------------------------------------------------------------------------------

QString WAbstractTrack::title() const
{
    Q_D(const WAbstractTrack); return d->title;
}

void WAbstractTrack::setTitle(const QString & title)
{
    Q_D(WAbstractTrack); d->title = title.simplified();
}

//-------------------------------------------------------------------------------------------------

QUrl WAbstractTrack::cover() const
{
    Q_D(const WAbstractTrack); return d->cover;
}

void WAbstractTrack::setCover(const QUrl & cover)
{
    Q_D(WAbstractTrack); d->cover = cover;
}

//-------------------------------------------------------------------------------------------------

QString WAbstractTrack::author() const
{
    Q_D(const WAbstractTrack); return d->author;
}

void WAbstractTrack::setAuthor(const QString & author)
{
    Q_D(WAbstractTrack); d->author = author;
}

//-------------------------------------------------------------------------------------------------

QString WAbstractTrack::feed() const
{
    Q_D(const WAbstractTrack); return d->feed;
}

void WAbstractTrack::setFeed(const QString & feed)
{
    Q_D(WAbstractTrack); d->feed = feed;
}

//-------------------------------------------------------------------------------------------------

int WAbstractTrack::duration() const
{
    Q_D(const WAbstractTrack); return d->duration;
}

void WAbstractTrack::setDuration(int msec)
{
    Q_D(WAbstractTrack); d->duration = msec;
}

//-------------------------------------------------------------------------------------------------

QDateTime WAbstractTrack::date() const
{
    Q_D(const WAbstractTrack); return d->date;
}

void WAbstractTrack::setDate(const QDateTime & date)
{
    Q_D(WAbstractTrack); d->date = date;
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Quality WAbstractTrack::quality() const
{
    Q_D(const WAbstractTrack); return d->quality;
}

void WAbstractTrack::setQuality(WAbstractBackend::Quality quality)
{
    Q_D(WAbstractTrack); d->quality = quality;
}

//-------------------------------------------------------------------------------------------------

WAbstractPlaylist * WAbstractTrack::playlist() const
{
    Q_D(const WAbstractTrack); return d->playlist;
}

#endif // SK_NO_ABSTRACTTRACK
