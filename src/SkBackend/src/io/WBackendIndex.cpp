//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkBackend.

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

#include "WBackendIndex.h"

#ifndef SK_NO_BACKENDINDEX

// Qt includes
#include <QCoreApplication>

// Sk includes
#include <WControllerApplication>
#include <WControllerFile>
#include <WControllerNetwork>
#include <WControllerDownload>
#include <WControllerPlaylist>
#include <WBackendUniversal>
#include <WYamlReader>

//-------------------------------------------------------------------------------------------------
// Static variables

static const int BACKENDINDEX_TIMEOUT = 60000; // 1 minute

//=================================================================================================
// WBackendIndexQuery
//=================================================================================================

class WBackendIndexQuery : public QObject
{
    Q_OBJECT

public: // Interface
    Q_INVOKABLE void extract(const QByteArray & array);

private: // Functions
    QStringList extractList(const WYamlReader & reader, const QString & key) const;

    QList<WBackendIndexItem> extractItems(const WYamlReader & reader) const;

    QHash<QString, QString>
    extractHash(const WYamlReader & reader,
                const QString & key, const QString & source,
                const QHash<QString, const WBackendIndexItem *> & hash,
                bool applyUrl = false) const;

signals:
    void loaded(const WBackendIndexData & data);
};

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WBackendIndexQuery::extract(const QByteArray & array)
{
    WBackendIndexData data;

    QString content = Sk::readBml(array);

    //---------------------------------------------------------------------------------------------
    // Api

    QString api = WControllerPlaylist::vbmlVersion(content);

    if (Sk::versionIsHigher(WControllerPlaylist::versionApi(), api))
    {
        WControllerPlaylist::vbmlPatch(content, api);

        extract(content.toUtf8());

        return;
    }

    if (Sk::versionIsLower(WControllerPlaylist::versionApi(), api))
    {
        qWarning("WBackendIndexQuery::extract: The required API is too high.");
    }

    WYamlReader reader(content.toUtf8());

    //---------------------------------------------------------------------------------------------
    // Settings

    data.origin = reader.extractString("origin");

    data.api = api;

    data.version = reader.extractString("version");

    //---------------------------------------------------------------------------------------------
    // Backends

    data.backendsSearch   = extractList(reader, "backends_search");
    data.backendsTrack    = extractList(reader, "backends_track");
    data.backendsCover    = extractList(reader, "backends_cover");
    data.backendsSubtitle = extractList(reader, "backends_subtitle");

    data.backends = extractItems(reader);

    foreach (const WBackendIndexItem & item, data.backends)
    {
        data.hash.insert(item.id, &item);
    }

    data.covers = extractHash(reader, "covers", data.origin, data.hash, true);
    data.hubs   = extractHash(reader, "hubs",   data.origin, data.hash);

    //---------------------------------------------------------------------------------------------

    emit loaded(data);

    deleteLater();
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

QStringList WBackendIndexQuery::extractList(const WYamlReader & reader, const QString & key) const
{
    QString string = reader.extractString(key);

    if (string.isEmpty())
    {
        return QStringList();
    }
    else return string.simplified().split(' ');
}

QList<WBackendIndexItem> WBackendIndexQuery::extractItems(const WYamlReader & reader) const
{
    QList<WBackendIndexItem> items;

    QStringList list = reader.extractList("backends");

    foreach (const QString & string, list)
    {
        QStringList values = string.simplified().split(' ');

        if (values.count() != 4) continue;

        WBackendIndexItem item;

        item.id    = values.at(0);
        item.title = values.at(1);

        item.version = values.at(2);

        item.validate = values.at(3);

        items.append(item);
    }

    return items;
}

QHash<QString, QString>
WBackendIndexQuery::extractHash(const WYamlReader & reader,
                                const QString & key, const QString & source,
                                const QHash<QString, const WBackendIndexItem *> & hash,
                                bool applyUrl) const
{
    QHash<QString, QString> items;

    QString path;

    int index = source.lastIndexOf('/');

    if (index == -1)
    {
         path = source + '/';
    }
    else path = source.mid(0, index + 1);

    QString data = reader.extractString(key);

    QStringList list = data.split('\n');

    if (applyUrl)
    {
        foreach (const QString & string, list)
        {
            QStringList values = string.simplified().split(' ');

            if (values.count() != 2) continue;

            QString id = values.first();

            if (hash.contains(id) == false) continue;

            QString cover = values.last();

            if (WControllerNetwork::textIsUrl(cover))
            {
                 items.insert(id, cover);
            }
            else items.insert(id, path + cover);
        }
    }
    else
    {
        foreach (const QString & string, list)
        {
            QStringList values = string.simplified().split(' ');

            if (values.count() != 2) continue;

            QString id = values.first();

            if (hash.contains(id) == false) continue;

            QString cover = values.last();

            items.insert(id, cover);
        }
    }

    return items;
}

//=================================================================================================
// WBackendIndexPrivate
//=================================================================================================

WBackendIndexPrivate::WBackendIndexPrivate(WBackendIndex * p) : WBackendLoaderPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WBackendIndexPrivate::init(const QString & url)
{
    remote = NULL;

    this->url = url;

    urlBase = WControllerNetwork::extractBaseUrl(url);

    loaded = false;

    qRegisterMetaType<WBackendIndexData>("WBackendIndexData");

    const QMetaObject * meta = WBackendIndexQuery().metaObject();

    method = meta->method(meta->indexOfMethod("extract(QByteArray)"));

    load();
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WBackendIndexPrivate::load()
{
    Q_Q(WBackendIndex);

    remote = wControllerDownload->getData(url, BACKENDINDEX_TIMEOUT);

    QObject::connect(remote, SIGNAL(loaded(WRemoteData *)), q, SLOT(onLoad()));
}

void WBackendIndexPrivate::loadData(const QByteArray & array)
{
    Q_Q(WBackendIndex);

    WBackendIndexQuery * query = new WBackendIndexQuery;

    QObject::connect(query, SIGNAL(loaded(WBackendIndexData)), q, SLOT(onData(WBackendIndexData)));

    query->moveToThread(wControllerPlaylist->thread());

    method.invoke(query, Q_ARG(const QByteArray &, array));
}

//-------------------------------------------------------------------------------------------------

void WBackendIndexPrivate::applyLoaded()
{
    Q_Q(WBackendIndex);

    if (loaded == false)
    {
        loaded = true;

        emit q->loadedChanged();
    }

    emit q->loaded();
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WBackendIndexPrivate::onLoad()
{
    Q_Q(WBackendIndex);

    QByteArray array = remote->readAll();

    QObject::disconnect(remote, 0, q, 0);

    remote->deleteLater();

    remote = NULL;

    if (array.isEmpty())
    {
        data = WBackendIndexData();

        applyLoaded();

        return;
    }

    loadData(array);
}

void WBackendIndexPrivate::onUpdate()
{
    Q_Q(WBackendIndex);

    QObject::disconnect(remote, 0, q, 0);

    if (remote->hasError())
    {
        remote->deleteLater();

        remote = NULL;

        applyLoaded();

        return;
    }

    QByteArray array = remote->readAll();

    remote->deleteLater();

    remote = NULL;

    if (array.isEmpty())
    {
        applyLoaded();

        return;
    }

    loadData(array);

    wControllerFile->startWriteFile(WControllerFile::filePath(url), array);
}

//-------------------------------------------------------------------------------------------------

void WBackendIndexPrivate::onData(const WBackendIndexData & data)
{
    Q_Q(WBackendIndex);

    QString version = this->data.version;

    if (version.isEmpty())
    {
        this->data = data;

        applyLoaded();

        return;
    }

    // NOTE: If the required API version is too high we keep our previous data.
    if (data.valid == false)
    {
        qWarning("WBackendIndexPrivate::onData: Cannot update, the required API is too high.");

        applyLoaded();

        return;
    }

    QString origin = data.origin;

    origin = origin.mid(0, origin.lastIndexOf('/') + 1);

    QString path = WControllerFile::filePath(urlBase) + '/';

    foreach (const WBackendIndexItem & item, data.backends)
    {
        QString id = item.id;

        const WBackendIndexItem * itemHash = this->data.hash.value(id);

        if (itemHash)
        {
            if (itemHash->version == item.version) continue;

            WBackendNet * pointer = q->getBackend(id);

            if (pointer)
            {
                WBackendUniversal * backend = static_cast<WBackendUniversal *> (pointer);

                backend->update();

                continue;
            }
        }

        QString name = id + ".vbml";

        WRemoteData * remote = wControllerDownload->getData(origin + name, BACKENDINDEX_TIMEOUT);

        WBackendIndexFile file;

        file.id = id;

        file.name = path + name;

        jobs.insert(remote, file);

        QObject::connect(remote, SIGNAL(loaded(WRemoteData *)),
                         q,      SLOT(onItemLoad(WRemoteData *)));
    }


    this->data = data;

    applyLoaded();

    if (version != data.version)
    {
        emit q->updated();
    }
}

//-------------------------------------------------------------------------------------------------

void WBackendIndexPrivate::onBackendUpdate()
{
    Q_Q(WBackendIndex);

    WBackendUniversal * backend = static_cast<WBackendUniversal *> (q->sender());

    emit q->backendUpdated(backend->id());
}

//-------------------------------------------------------------------------------------------------

void WBackendIndexPrivate::onItemLoad(WRemoteData * data)
{
    Q_Q(WBackendIndex);

    WBackendIndexFile file = jobs.take(data);

    ids.append(file.id);

    WControllerFileReply * reply = wControllerFile->startWriteFile(file.name, data->readAll());

    QObject::connect(reply, SIGNAL(complete(bool)), q, SLOT(onActionComplete()));

    data->deleteLater();
}

void WBackendIndexPrivate::onActionComplete()
{
    Q_Q(WBackendIndex);

    QString id = ids.takeFirst();

    emit q->backendUpdated(id);
}

//=================================================================================================
// WBackendIndex
//=================================================================================================

WBackendIndex::WBackendIndex(const QString & url, QObject * parent)
    : WBackendLoader(new WBackendIndexPrivate(this), parent)
{
    Q_D(WBackendIndex); d->init(url);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WBackendIndex::update()
{
    Q_D(WBackendIndex);

    QString origin = d->data.origin;

    if (origin.isEmpty())
    {
        qWarning("WBackendIndex::update: 'origin' is empty.");

        return;
    }

    if (d->remote) delete d->remote;

    d->remote = wControllerDownload->getData(origin, BACKENDINDEX_TIMEOUT);

    connect(d->remote, SIGNAL(loaded(WRemoteData *)), this, SLOT(onUpdate()));
}

/* Q_INVOKABLE */ void WBackendIndex::reload()
{
    Q_D(WBackendIndex);

    if (d->remote) delete d->remote;

    d->load();
}

//-------------------------------------------------------------------------------------------------
// WBackendLoader reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ bool WBackendIndex::checkId(const QString & id) const
{
    Q_D(const WBackendIndex);

    return d->data.hash.contains(id);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ void WBackendIndex::createFolderItems(WLibraryFolder * folder,
                                                                WLibraryItem::Type type) const
{
    Q_ASSERT(folder);

    Q_D(const WBackendIndex);

    QString source = "vbml:run?backend=";

    QString cover = d->data.origin;

    cover = cover.mid(0, cover.lastIndexOf('/') + 1);

    foreach (const QString & string, d->data.backendsTrack)
    {
        WLibraryFolderItem item(type, WLocalObject::Default);

        item.source = source + string;

        item.title = d->data.hash.value(string)->title;

        item.cover = cover + "cover/" + string + ".png";

        item.label = string;

        folder->addItem(item);
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendIndex::searchId() const
{
    Q_D(const WBackendIndex);

    const QStringList & list = d->data.backendsSearch;

    if (list.isEmpty())
    {
        return QString();
    }
    else return list.first();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendIndex::coverFromId(const QString & id) const
{
    Q_D(const WBackendIndex);

    return d->data.covers.value(id);
}

/* Q_INVOKABLE virtual */ QString WBackendIndex::hubFromId(const QString & id) const
{
    Q_D(const WBackendIndex);

    return d->data.hubs.value(id);
}

//-------------------------------------------------------------------------------------------------
// Protected WBackendLoader reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ WBackendNet * WBackendIndex::createBackend(const QString & id) const
{
    Q_D(const WBackendIndex);

    QString source = d->urlBase + '/' + id + ".vbml";

    WBackendUniversal * backend = new WBackendUniversal(id, source);

    connect(backend, SIGNAL(updated()), this, SLOT(onBackendUpdate()));

    return backend;
}

/* Q_INVOKABLE virtual */ void WBackendIndex::checkBackend(WBackendNet * backend) const
{
    Q_D(const WBackendIndex);

    WBackendUniversal * backendUniversal = static_cast<WBackendUniversal *> (backend);

    WBackendUniversalPrivate * p = backendUniversal->d_func();

    const WBackendIndexItem * item = d->data.hash.value(p->id);

    if (item && item->version != p->data.version)
    {
        backendUniversal->update();
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendIndex::getId(const QString & url) const
{
    Q_D(const WBackendIndex);

    WRegExp regExp;

    foreach (const WBackendIndexItem & item, d->data.backends)
    {
        regExp.setPattern(item.validate);

        if (url.indexOf(regExp) != -1)
        {
            return item.id;
        }
    }

    return QString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QStringList WBackendIndex::getCoverIds() const
{
    Q_D(const WBackendIndex); return d->data.backendsCover;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

bool WBackendIndex::isLoaded() const
{
    Q_D(const WBackendIndex); return d->loaded;
}

#endif // SK_NO_BACKENDINDEX

#include "WBackendIndex.moc"
