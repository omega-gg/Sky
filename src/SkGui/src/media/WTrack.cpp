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

#include "WTrack.h"

#ifndef SK_NO_TRACK

// Qt includes
#include <QDateTime>

// Sk includes
#include <WControllerApplication>
#include <WControllerNetwork>
#include <WControllerPlaylist>
#include <WPlaylist>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WTrack_p.h"

WTrackPrivate::WTrackPrivate(WTrack * p) : WPrivate(p) {}

void WTrackPrivate::init()
{
    type = WTrack::Track;

    id = -1;

    duration = -1;

    playlist = NULL;

    timeUpdate = 0;
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WTrack::WTrack(const QString & source, State state)
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
    Q_D(const WTrack);

    return (d->source.isEmpty() == false);
}

//-------------------------------------------------------------------------------------------------

void WTrack::applyDataTo(WTrack * other) const
{
    Q_D(const WTrack);

    WTrackPrivate * op = other->d_func();

    if (d->type != WTrack::Unknown)
    {
        op->type = d->type;
    }

    if (d->title.isEmpty() == false)
    {
        op->title = d->title;
    }

    if (d->cover.isEmpty() == false)
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
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ WTrack::Type WTrack::typeFromString(const QString & string)
{
    if      (string == "track")   return Track;
    else if (string == "live")    return Live;
    else if (string == "hub")     return Hub;
    else if (string == "channel") return Channel;
    else                          return Unknown;
}

/* Q_INVOKABLE static */ QString WTrack::typeToString(Type type)
{
    if      (type == Live)    return "live";
    else if (type == Hub)     return "hub";
    else if (type == Channel) return "channel";
    else                      return "track";
}

//-------------------------------------------------------------------------------------------------
// Virtual interface
//-------------------------------------------------------------------------------------------------

/* virtual */ QVariantMap WTrack::toMap() const
{
    Q_D(const WTrack);

    QVariantMap map;

    map.insert("id", d->id);

    map.insert("type", d->type);

    map.insert("state", d->state);

    map.insert("source", d->source);

    map.insert("title", d->title);
    map.insert("cover", d->cover);

    map.insert("author", d->author);
    map.insert("feed",   d->feed);

    map.insert("duration", d->duration);

    map.insert("date", d->date);

    return map;
}

/* virtual */ QString WTrack::toVbml() const
{
    Q_D(const WTrack);

    QString vbml = WControllerPlaylist::vbml();

    Sk::bmlPair(vbml, "type", typeToString(d->type), "\n\n");

    if (WControllerNetwork::urlIsFile(d->source) == false)
    {
        Sk::bmlPair(vbml, "origin", d->source, "\n\n");
    }

    Sk::bmlPair(vbml, "title", d->title, "\n\n");
    Sk::bmlPair(vbml, "cover", d->cover, "\n\n");

    Sk::bmlPair(vbml, "author", d->author, "\n\n");
    Sk::bmlPair(vbml, "feed",   d->feed,   "\n\n");

    if (d->duration > 0)
    {
        Sk::bmlPair(vbml, "duration", QString::number(d->duration), "\n\n");
    }

    if (d->date.isValid())
    {
        Sk::bmlPair(vbml, "date", Sk::bmlDate(d->date), "\n\n");
    }

    // NOTE: We clear the last '\n'.
    vbml.chop(1);

    return vbml;
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

bool WTrack::operator==(const WTrack & other) const
{
    Q_D(const WTrack);

    const WTrackPrivate * op = other.d_func();

    if (d->id == op->id &&

        d->type == op->type &&

        d->state == op->state &&

        d->source == op->source &&

        d->title == op->title &&
        d->cover == op->cover &&

        d->author == op->author &&
        d->feed   == op->feed   &&

        d->duration == op->duration &&

        d->date == op->date &&

        d->playlist == op->playlist)
    {
         return true;
    }
    else return false;
}

WTrack & WTrack::operator=(const WTrack & other)
{
    Q_D(WTrack);

    const WTrackPrivate * op = other.d_func();

    d->id = op->id;

    d->type = op->type;

    d->state = op->state;

    d->source = op->source;

    d->title = op->title;
    d->cover = op->cover;

    d->author = op->author;
    d->feed   = op->feed;

    d->duration = op->duration;

    d->date = op->date;

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

void WTrack::setId(int id)
{
    Q_D(WTrack); d->id = id;
}

//-------------------------------------------------------------------------------------------------

WTrack::Type WTrack::type() const
{
    Q_D(const WTrack); return d->type;
}

void WTrack::setType(WTrack::Type type)
{
    Q_D(WTrack); d->type = type;
}

bool WTrack::isHub() const
{
    Q_D(const WTrack); return (d->type == Hub);
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
    Q_D(const WTrack); return (d->state >= Loaded);
}

//-------------------------------------------------------------------------------------------------

QString WTrack::source() const
{
    Q_D(const WTrack); return d->source;
}

void WTrack::setSource(const QString & url)
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

QString WTrack::cover() const
{
    Q_D(const WTrack); return d->cover;
}

void WTrack::setCover(const QString & cover)
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

WPlaylist * WTrack::playlist() const
{
    Q_D(const WTrack); return d->playlist;
}

#endif // SK_NO_TRACK
