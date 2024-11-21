//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkMedia.

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

#include "WBackendSubtitle.h"

#ifndef SK_NO_BACKENDSUBTITLE

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>
#include <WControllerNetwork>
#include <WControllerPlaylist>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int BACKENDSUBTITLE_LENGTH = 1000;

static const int BACKENDSUBTITLE_TIMEOUT = 16;

//=================================================================================================
// WBackendSubtitleQuery
//=================================================================================================

class WBackendSubtitleQuery : public QObject
{
    Q_OBJECT

public: // Interface
    Q_INVOKABLE void extract(const QByteArray & data);

private: // Functions
    int extractMsecs(const QString & string);

signals:
    void loaded(const QList<WBackendSubtitleData> & list);
};

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WBackendSubtitleQuery::extract(const QByteArray & data)
{
    QList<WBackendSubtitleData> list;

    bool parse = false;

    int start = 0;
    int end   = 0;

    QString text;

    QString codec = Sk::detectCodec(data);

    QString content;

    if (codec.isEmpty() || codec == "ascii")
    {
         content = Sk::readUtf8(data);
    }
    else content = Sk::readCodec(data, codec);

    content.remove('\r');

    if (content.startsWith("<?xml"))
    {
        content = Sk::sliceIn(content, "<transcript>", "</transcript>");

        QStringList array = Sk::split(content, "</text>");

        foreach (const QString & string, array)
        {
            WBackendSubtitleData dataSubtitle;

            start = WControllerNetwork::extractAttribute(string, "start").toFloat() * 1000;
            end   = WControllerNetwork::extractAttribute(string, "dur")  .toFloat() * 1000;

            dataSubtitle.start = start;
            dataSubtitle.end   = start + end;

            int index = string.indexOf('>') + 1;

            dataSubtitle.text = WControllerNetwork::htmlToUtf8(string.mid(index));

            list.append(dataSubtitle);
        }

        emit loaded(list);

        deleteLater();

        return;
    }

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

    emit loaded(list);

    deleteLater();
}

//-------------------------------------------------------------------------------------------------

int WBackendSubtitleQuery::extractMsecs(const QString & string)
{
    QTime time = QTime::fromString(string, "HH:mm:ss,zzz");

    return Sk::getMsecs(time);
}

//=================================================================================================
// WBackendSubtitlePrivate
//=================================================================================================

WBackendSubtitlePrivate::WBackendSubtitlePrivate(WBackendSubtitle * p) : WPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WBackendSubtitlePrivate::init()
{
    W_GET_CONTROLLER(WControllerPlaylist, controller);

    if (controller == NULL)
    {
        qWarning("WBackendSubtitlePrivate::init: WControllerPlaylist does not exist.");

        thread = NULL;
    }
    else thread = controller->thread();

    item = NULL;

    enabled = true;

    retry      =  1;
    retryCount = -1;

    cursor = -1;

    currentTime = -1;

    delay = 0;

    index = -1;

    start = -1;
    end   = -1;

    timer = -1;

    qRegisterMetaType<QList<WBackendSubtitleData> >("QList<WBackendSubtitleData>");

    const QMetaObject * meta = WBackendSubtitleQuery().metaObject();

    method = meta->method(meta->indexOfMethod("extract(QByteArray)"));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBackendSubtitlePrivate::updateText()
{
    int count = list.count();

    if (count == 0) return;

    int index = this->index;

    if (index == count || (cursor >= start && cursor <= end)) return;

    if (index > -1)
    {
        index++;

        if (index != count)
        {
            const WBackendSubtitleData & data = list.at(index);

            if (cursor >= data.start && cursor <= data.end)
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

        if (cursor < data.start)
        {
            this->index = -2;

            start = 0;
            end   = data.start;

            clearString();

            return;
        }

        if (cursor <= data.end)
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
        if (cursor > list.at(count - 1).end)
        {
            return false;
        }
    }
    else if (cursor >= start)
    {
        if (cursor <= end)
        {
            return false;
        }

        index++;

        if (index != list.count() && cursor <= list.at(index).end)
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

    if (enabled && list.isEmpty() == false)
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
    // NOTE: If the data is empty we skip the query.
    if (data.isEmpty()) return;

    qDebug("SUBTITLE %d %s", data.length(), extension.C_STR);

    Q_Q(WBackendSubtitle);

    QObject::disconnect(item, 0, q, 0);

    item->deleteLater();

    item = NULL;

    //if (extension == "srt")
    {
        WBackendSubtitleQuery * query = new WBackendSubtitleQuery;

        QObject::connect(query, SIGNAL(loaded(QList<WBackendSubtitleData>)),
                         q,     SLOT(onLoaded(QList<WBackendSubtitleData>)));

        if (thread)
        {
            query->moveToThread(thread);

            method.invoke(query, Q_ARG(const QByteArray &, data));
        }
        else query->extract(data);
    }
    //else emit q->loaded(false);
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

//-------------------------------------------------------------------------------------------------

void WBackendSubtitlePrivate::onLoaded(const QList<WBackendSubtitleData> & list)
{
    Q_Q(WBackendSubtitle);

    this->list = list;

    if (list.isEmpty() == false)
    {
        updateText();

        emit q->loaded(true);
    }
    else emit q->loaded(false);
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

    int elapsed = d->time.elapsed();

    d->cursor += elapsed;

    d->currentTime += elapsed;

    d->time.restart();

    d->updateText();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QThread * WBackendSubtitle::thread() const
{
    Q_D(const WBackendSubtitle); return d->thread;
}

void WBackendSubtitle::setThread(QThread * thread)
{
    Q_D(WBackendSubtitle);

    if (d->thread == thread) return;

    d->thread = thread;

    emit threadChanged();
}

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

    d->cursor = msec + d->delay;

    if (d->enabled && d->checkIndex())
    {
        d->updateText();
    }

    emit currentTimeChanged();
}

int WBackendSubtitle::delay() const
{
    Q_D(const WBackendSubtitle); return d->currentTime;
}

void WBackendSubtitle::setDelay(int msec)
{
    Q_D(WBackendSubtitle);

    if (d->delay == msec) return;

    d->delay = msec;

    d->cursor = d->currentTime + msec;

    if (d->enabled && d->checkIndex())
    {
        d->updateText();
    }

    emit delayChanged();
}

//-------------------------------------------------------------------------------------------------

QString WBackendSubtitle::text() const
{
    Q_D(const WBackendSubtitle); return d->text;
}

#endif // SK_NO_BACKENDSUBTITLE

#include "WBackendSubtitle.moc"
