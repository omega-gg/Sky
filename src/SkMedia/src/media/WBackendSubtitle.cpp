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
#include <WControllerApplication>
#include <WControllerFile>
#include <WControllerPlaylist>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int BACKENDSUBTITLE_LENGTH = 1000;

static const int BACKENDSUBTITLE_TIMEOUT = 16;

//=================================================================================================
// WBackendSubtitlePrivate
//=================================================================================================

WBackendSubtitlePrivate::WBackendSubtitlePrivate(WBackendSubtitle * p) : WPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WBackendSubtitlePrivate::init()
{
    item = NULL;

    enabled = true;

    retry      =  1;
    retryCount = -1;

    currentTime = -1;

    index = -1;

    start = -1;
    end   = -1;

    timer = -1;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBackendSubtitlePrivate::loadSrt(const QByteArray & data)
{
    bool parse = false;

    int start = 0;
    int end   = 0;

    QString text;

    QString content = Sk::readUtf8(data);

    content.remove('\r');

    while (content.isEmpty() == false)
    {
        QString string = Sk::extractLine(&content);

        if (parse == false)
        {
            int index = string.indexOf('>');

            if (index == -1) continue;

            parse = true;

            string.remove(' ');

            QString time = string.mid(0, string.indexOf('-'));

            start = extractMsecs(time);

            time = string.mid(index);

            end = extractMsecs(time);
        }
        else if (string.isEmpty())
        {
            if (text.isEmpty() == false)
            {
                text.chop(4);

                WBackendSubtitleData dataSubtitle;

                dataSubtitle.start = start;
                dataSubtitle.end   = end;

                dataSubtitle.text = text;

                list.append(dataSubtitle);
            }

            parse = false;

            text.clear();
        }
        else if (text.length() < BACKENDSUBTITLE_LENGTH)
        {
            text.append(string + "<br>");

            int length = text.length();

            if (length > BACKENDSUBTITLE_LENGTH)
            {
                text.chop(length - BACKENDSUBTITLE_LENGTH);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void WBackendSubtitlePrivate::updateText()
{
    int count = list.count();

    if (count == 0) return;

    int index = this->index;

    if (index == count || (currentTime >= start && currentTime <= end)) return;

    if (index > -1)
    {
        index++;

        if (index != count)
        {
            const WBackendSubtitleData & data = list.at(index);

            if (currentTime >= data.start && currentTime <= data.end)
            {
                this->index = index;

                start = data.start;
                end   = data.end;

                setText(data.text);
            }
            else if (text.isNull() == false)
            {
                end = data.start;

                clearString();
            }
        }
        else if (text.isNull() == false)
        {
            clearString();
        }

        return;
    }

    for (int i = 0; i < count; i++)
    {
        const WBackendSubtitleData & data = list.at(i);

        if (currentTime < data.start)
        {
            this->index = -2;

            start = 0;
            end   = data.start;

            clearString();

            return;
        }

        if (currentTime <= data.end)
        {
            this->index = i;

            start = data.start;
            end   = data.end;

            setText(data.text);

            return;
        }
    }

    this->index = count;

    if (text.isNull()) return;

    clearString();
}

//-------------------------------------------------------------------------------------------------

void WBackendSubtitlePrivate::clearText()
{
    if (index == -1) return;

    index = -1;

    start = -1;
    end   = -1;

    clearString();
}

void WBackendSubtitlePrivate::clearString()
{
    Q_Q(WBackendSubtitle);

    text = QString();

    stopTimer();

    emit q->textChanged();
}

//-------------------------------------------------------------------------------------------------

bool WBackendSubtitlePrivate::checkIndex()
{
    int count = list.count();

    if (count == 0) return false;

    int index = this->index;

    if (index < 0) return true;

    if (index == count)
    {
        if (currentTime > list.at(count - 1).end)
        {
            return false;
        }
    }
    else if (currentTime >= start)
    {
        if (currentTime <= end)
        {
            return false;
        }

        index++;

        if (index != list.count() && currentTime <= list.at(index).end)
        {
            return true;
        }
    }

    this->index = -1;

    return true;
}

//-------------------------------------------------------------------------------------------------

void WBackendSubtitlePrivate::stopTimer()
{
    if (timer == -1) return;

    Q_Q(WBackendSubtitle);

    q->killTimer(timer);

    timer = -1;
}

//-------------------------------------------------------------------------------------------------

int WBackendSubtitlePrivate::extractMsecs(const QString & string)
{
    QTime time = QTime::fromString(string, "HH:mm:ss,zzz");

    return Sk::getMsecs(time);
}

//-------------------------------------------------------------------------------------------------

WLibraryItem * WBackendSubtitlePrivate::getItem()
{
    if (item) return item;

    Q_Q(WBackendSubtitle);

    item = new WLibraryItem;

    item->setParent(q);

    QObject::connect(item, SIGNAL(queryData(const QByteArray &, const QString &)),
                     q,    SLOT(onQueryData(const QByteArray &, const QString &)));

    QObject::connect(item, SIGNAL(queryCompleted()), q, SLOT(onQueryCompleted()));

    return item;
}

//-------------------------------------------------------------------------------------------------

void WBackendSubtitlePrivate::setText(const QString & text)
{
    if (this->text == text) return;

    Q_Q(WBackendSubtitle);

    this->text = text;

    stopTimer();

    if (enabled && currentTime != -1 && list.isEmpty() == false)
    {
        time.restart();

        timer = q->startTimer(BACKENDSUBTITLE_TIMEOUT);
    }

    emit q->textChanged();
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WBackendSubtitlePrivate::onQueryData(const QByteArray & data, const QString & extension)
{
    qDebug("SUBTITLE %d %s", data.length(), extension.C_STR);

    if (extension == "srt")
    {
        loadSrt(data);

        if (enabled) updateText();
    }

    Q_Q(WBackendSubtitle);

    QObject::disconnect(item, 0, q, 0);

    item->deleteLater();

    item = NULL;

    if (list.isEmpty())
    {
         emit q->loaded(false);
    }
    else emit q->loaded(true);
}

void WBackendSubtitlePrivate::onQueryCompleted()
{
    Q_Q(WBackendSubtitle);

    if (retryCount > 0)
    {
        retryCount--;

        item->reloadQuery();

        return;
    }

    QObject::disconnect(item, 0, q, 0);

    item->deleteLater();

    item = NULL;

    emit q->loaded(false);
}

//=================================================================================================
// WBackendSubtitle
//=================================================================================================

/* explicit */ WBackendSubtitle::WBackendSubtitle(QObject * parent)
    : QObject(parent), WPrivatable(new WBackendSubtitlePrivate(this))
{
    Q_D(WBackendSubtitle); d->init();
}

//-------------------------------------------------------------------------------------------------
// Protected events
//-------------------------------------------------------------------------------------------------

/* virtual */ void WBackendSubtitle::timerEvent(QTimerEvent *)
{
    Q_D(WBackendSubtitle);

    d->currentTime += d->time.elapsed();

    d->time.restart();

    d->updateText();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WBackendSubtitle::isEnabled() const
{
    Q_D(const WBackendSubtitle); return d->enabled;
}

void WBackendSubtitle::setEnabled(bool enabled)
{
    Q_D(WBackendSubtitle);

    if (d->enabled == enabled) return;

    d->enabled = enabled;

    if (enabled)
    {
        d->updateText();
    }
    else d->clearText();

    emit enabledChanged();
}

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

    d->list.clear();

    d->clearText();

    if (url.isEmpty() == false)
    {
        WLibraryItem * item = d->getItem();

        QString fileUrl = wControllerFile->getFileUrl(url);

        if (fileUrl.isEmpty())
        {
             item->loadSource(url);
        }
        else item->loadSource(fileUrl);
    }
    else if (d->item)
    {
        d->onQueryCompleted();
    }

    emit sourceChanged();
}

//-------------------------------------------------------------------------------------------------

bool WBackendSubtitle::retry() const
{
    Q_D(const WBackendSubtitle); return d->retry;
}

void WBackendSubtitle::setRetry(int count)
{
    Q_D(WBackendSubtitle);

    if (d->retry == count) return;

    d->retry = count;

    emit retryChanged();
}

//-------------------------------------------------------------------------------------------------

int WBackendSubtitle::currentTime() const
{
    Q_D(const WBackendSubtitle); return d->currentTime;
}

void WBackendSubtitle::setCurrentTime(int msec)
{
    Q_D(WBackendSubtitle);

    if (d->currentTime == msec) return;

    d->currentTime = msec;

    if (d->enabled)
    {
        if (msec == -1)
        {
            d->clearText();
        }
        else if (d->checkIndex())
        {
            d->updateText();
        }
    }

    emit currentTimeChanged();
}

//-------------------------------------------------------------------------------------------------

QString WBackendSubtitle::text() const
{
    Q_D(const WBackendSubtitle); return d->text;
}

#endif // SK_NO_BACKENDSUBTITLE
