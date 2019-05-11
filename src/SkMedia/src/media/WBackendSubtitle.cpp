//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkMedia module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WBackendSubtitle.h"

#ifndef SK_NO_BACKENDSUBTITLE

// Sk includes
#include <WControllerPlaylist>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WBackendSubtitlePrivate::WBackendSubtitlePrivate(WBackendSubtitle * p) : WPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WBackendSubtitlePrivate::init()
{
    item = NULL;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

WLibraryItem * WBackendSubtitlePrivate::getItem()
{
    if (item) return item;

    Q_Q(WBackendSubtitle);

    item = new WLibraryItem;

    item->setParent(q);

    QObject::connect(item, SIGNAL(queryData(const QByteArray &, const QString &)),
                     q,    SLOT(onQueryData(const QByteArray &, const QString &)));

    QObject::connect(item, SIGNAL(queryEnded()), q, SLOT(onQueryCompleted()));

    return item;
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WBackendSubtitlePrivate::onQueryData(const QByteArray & data, const QString & extension)
{
    if (extension == "srt")
    {

    }

    Q_Q(WBackendSubtitle);

    QObject::disconnect(item, 0, q, 0);

    item->deleteLater();

    item = NULL;
}

void WBackendSubtitlePrivate::onQueryCompleted()
{
    Q_Q(WBackendSubtitle);

    QObject::disconnect(item, 0, q, 0);

    delete item;

    item = NULL;
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WBackendSubtitle::WBackendSubtitle(QObject * parent)
    : QObject(parent), WPrivatable(new WBackendSubtitlePrivate(this))
{
    Q_D(WBackendSubtitle); d->init();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QString WBackendSubtitle::source() const
{
    Q_D(const WBackendSubtitle); return d->source;
}

void WBackendSubtitle::setSource(const QString & url)
{
    Q_D(WBackendSubtitle);

    if (d->source == url) return;

    d->source = url;

    if (url.isEmpty() == false)
    {
        WLibraryItem * item = d->getItem();

        item->loadSource(url);
    }
    else if (d->item)
    {
        d->onQueryCompleted();
    }

    emit sourceChanged();
}

//-------------------------------------------------------------------------------------------------

QString WBackendSubtitle::text() const
{
    Q_D(const WBackendSubtitle); return d->text;
}

#endif // SK_NO_BACKENDSUBTITLE
