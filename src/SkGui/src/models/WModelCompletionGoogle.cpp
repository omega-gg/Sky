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

#include "WModelCompletionGoogle.h"

#ifndef SK_NO_MODELCOMPLETIONGOOGLE

// Qt includes
#include <QDomDocument>

#ifdef QT_LATEST
#include <QUrlQuery>
#endif

// Sk includes
#include <WControllerDownload>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WModelCompletionGooglePrivate::WModelCompletionGooglePrivate(WModelCompletionGoogle * p)
    : WPrivate(p) {}

void WModelCompletionGooglePrivate::init()
{
    data = NULL;

    ready   = false;
    loading = false;

#ifdef QT_4
    Q_Q(WModelCompletionGoogle);

    q->setRoleNames(q->roleNames());
#endif
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WModelCompletionGooglePrivate::setLoading(bool loading)
{
    if (this->loading == loading) return;

    Q_Q(WModelCompletionGoogle);

    this->loading = loading;

    emit q->loadingChanged();
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WModelCompletionGooglePrivate::onLoaded(WRemoteData * data)
{
    Q_Q(WModelCompletionGoogle);

    this->data = NULL;

    if (data->hasError())
    {
        emit q->queryCompleted();

        delete data;

        return;
    }

    QDomDocument document;

    document.setContent(data->readAll());

    QDomNodeList nodes = document.elementsByTagName("suggestion");

    q->beginResetModel();

    listCompletion.clear();

    for (int i = 0; i < nodes.count(); i++)
    {
        QDomElement element = nodes.at(i).toElement();

        listCompletion.append(element.attribute("data"));
    }

    q->endResetModel();

    setLoading(false);

    emit q->queryCompleted();

    delete data;
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WModelCompletionGoogle::WModelCompletionGoogle(QObject * parent)
    : QAbstractListModel(parent), WPrivatable(new WModelCompletionGooglePrivate(this))
{
    Q_D(WModelCompletionGoogle); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ bool WModelCompletionGoogle::runQuery()
{
    Q_D(WModelCompletionGoogle);

    if (d->ready == false) return false;

    if (d->data) delete d->data;

    if (d->query.isEmpty())
    {
        d->data = NULL;

        d->ready = false;

        if (d->listCompletion.count())
        {
            beginResetModel();

            d->listCompletion.clear();

            endResetModel();
        }

        d->setLoading(false);

        emit queryCompleted();
    }
    else
    {
        d->data = wControllerDownload->getData(d->url, this);

        d->ready = false;

        d->setLoading(true);

        connect(d->data, SIGNAL(loaded(WRemoteData *)), this, SLOT(onLoaded(WRemoteData *)));
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WModelCompletionGoogle::resetQuery()
{
    Q_D(WModelCompletionGoogle);

    if (d->query.isEmpty() == false)
    {
        d->url = QUrl("http://google.com/complete/search");

#ifdef QT_4
        d->url.addQueryItem("q", d->query);

        d->url.addQueryItem("output", "toolbar");

        d->url.addQueryItem("hl", "en");
#else
        QUrlQuery query(d->url);

        query.addQueryItem("q", d->query);

        query.addQueryItem("output", "toolbar");

        query.addQueryItem("hl", "en");

        d->url.setQuery(query);
#endif
    }
    else d->url = QUrl();

    d->ready = true;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WModelCompletionGoogle::addQueryItem(const QString & key,
                                                            const QString & value)
{
    Q_D(WModelCompletionGoogle);

#ifdef QT_4
    d->url.addQueryItem(key, value);
#else
    QUrlQuery query(d->url);

    query.addQueryItem(key, value);

    d->url.setQuery(query);
#endif

    emit urlChanged();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ QString WModelCompletionGoogle::getCompletionAt(int index) const
{
    Q_D(const WModelCompletionGoogle);

    if (index < 0 || index > d->listCompletion.count())
    {
         return QString();
    }
    else return d->listCompletion.at(index);
}

//-------------------------------------------------------------------------------------------------
// QAbstractItemModel reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ QHash<int, QByteArray> WModelCompletionGoogle::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles.insert(WModelCompletionGoogle::RoleTitle, "title");

    return roles;
}

/* virtual */ int WModelCompletionGoogle::rowCount(const QModelIndex &) const
{
    Q_D(const WModelCompletionGoogle);

    return d->listCompletion.count();
}

/* virtual */ QVariant WModelCompletionGoogle::data(const QModelIndex & index, int role) const
{
    Q_D(const WModelCompletionGoogle);

    if (index.row() < 0 || index.row() >= d->listCompletion.count())
    {
        return QVariant();
    }

    if (role == RoleTitle)
    {
         return d->listCompletion[index.row()];
    }
    else return QVariant();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QUrl WModelCompletionGoogle::url() const
{
    Q_D(const WModelCompletionGoogle); return d->url;
}

//-------------------------------------------------------------------------------------------------

QString WModelCompletionGoogle::query() const
{
    Q_D(const WModelCompletionGoogle); return d->query;
}

void WModelCompletionGoogle::setQuery(const QString & query)
{
    Q_D(WModelCompletionGoogle);

    QString string = query.simplified();

    if (d->query == string) return;

    d->query = string;

    resetQuery();

    emit queryChanged();
}

//-------------------------------------------------------------------------------------------------

bool WModelCompletionGoogle::isLoading() const
{
    Q_D(const WModelCompletionGoogle); return d->loading;
}

#endif // SK_NO_MODELCOMPLETIONGOOGLE
