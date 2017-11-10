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

#include "WTrack.h"

#ifndef SK_NO_TRACK

// Qt includes
#include <QDateTime>

// Sk includes
#include <WControllerScript>
#include <WPlaylist>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WTrack_p.h"

WTrackPrivate::WTrackPrivate(WTrack * p) : WPrivate(p) {}

void WTrackPrivate::init()
{
    id = -1;

    duration = -1;

    quality = WAbstractBackend::QualityInvalid;

    playlist = NULL;
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WTrack::WTrack(const QUrl & source, State state)
    : WPrivatable(new WTrackPrivate(this))
{
    Q_D(WTrack);

    d->init();

    d->state  = state;
    d->source = source;
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

bool WTrack::isValid() const
{
    Q_D(const WTrack); return (d->source.isValid());
}

//-------------------------------------------------------------------------------------------------

void WTrack::copyDataTo(WTrack * other) const
{
    Q_D(const WTrack);

    WTrackPrivate * op = other->d_func();

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

void WTrack::applyDataTo(WTrack * other) const
{
    Q_D(const WTrack);

    WTrackPrivate * op = other->d_func();

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

/* virtual */ QVariantMap WTrack::toMap() const
{
    Q_D(const WTrack);

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

WTrack::WTrack(const WTrack & other) : WPrivatable(new WTrackPrivate(this))
{
    Q_D(WTrack);

    d->init();

    *this = other;
}

//-------------------------------------------------------------------------------------------------

/* virtual */ bool WTrack::operator==(const WTrack & other) const
{
    Q_D(const WTrack);

    const WTrackPrivate * op = other.d_func();

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

/* virtual */ WTrack & WTrack::operator=(const WTrack & other)
{
    Q_D(WTrack);

    const WTrackPrivate * op = other.d_func();

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

int WTrack::id() const
{
    Q_D(const WTrack); return d->id;
}

//-------------------------------------------------------------------------------------------------

WTrack::State WTrack::state() const
{
    Q_D(const WTrack); return d->state;
}

void WTrack::setState(WTrack::State state)
{
    Q_D(WTrack); d->state = state;
}

//-------------------------------------------------------------------------------------------------

bool WTrack::isDefault() const
{
    Q_D(const WTrack); return (d->state == Default);
}

bool WTrack::isLoading() const
{
    Q_D(const WTrack); return (d->state == Loading);
}

bool WTrack::isLoaded() const
{
    Q_D(const WTrack); return (d->state == Loaded);
}

//-------------------------------------------------------------------------------------------------

QUrl WTrack::source() const
{
    Q_D(const WTrack); return d->source;
}

void WTrack::setSource(const QUrl & url)
{
    Q_D(WTrack); d->source = url;
}

//-------------------------------------------------------------------------------------------------

QString WTrack::title() const
{
    Q_D(const WTrack); return d->title;
}

void WTrack::setTitle(const QString & title)
{
    Q_D(WTrack); d->title = title.simplified();
}

//-------------------------------------------------------------------------------------------------

QUrl WTrack::cover() const
{
    Q_D(const WTrack); return d->cover;
}

void WTrack::setCover(const QUrl & cover)
{
    Q_D(WTrack); d->cover = cover;
}

//-------------------------------------------------------------------------------------------------

QString WTrack::author() const
{
    Q_D(const WTrack); return d->author;
}

void WTrack::setAuthor(const QString & author)
{
    Q_D(WTrack); d->author = author;
}

//-------------------------------------------------------------------------------------------------

QString WTrack::feed() const
{
    Q_D(const WTrack); return d->feed;
}

void WTrack::setFeed(const QString & feed)
{
    Q_D(WTrack); d->feed = feed;
}

//-------------------------------------------------------------------------------------------------

int WTrack::duration() const
{
    Q_D(const WTrack); return d->duration;
}

void WTrack::setDuration(int msec)
{
    Q_D(WTrack); d->duration = msec;
}

//-------------------------------------------------------------------------------------------------

QDateTime WTrack::date() const
{
    Q_D(const WTrack); return d->date;
}

void WTrack::setDate(const QDateTime & date)
{
    Q_D(WTrack); d->date = date;
}

//-------------------------------------------------------------------------------------------------

WAbstractBackend::Quality WTrack::quality() const
{
    Q_D(const WTrack); return d->quality;
}

void WTrack::setQuality(WAbstractBackend::Quality quality)
{
    Q_D(WTrack); d->quality = quality;
}

//-------------------------------------------------------------------------------------------------

WPlaylist * WTrack::playlist() const
{
    Q_D(const WTrack); return d->playlist;
}

#endif // SK_NO_TRACK
