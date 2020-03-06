//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkBackend module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
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
    extractCovers(const WYamlReader & reader,
                  const QString     & source,
                  const QHash<QString, const WBackendIndexItem *> & hash) const;

signals:
    void loaded(const WBackendIndexData & data);
};

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WBackendIndexQuery::extract(const QByteArray & array)
{
    WBackendIndexData data;

    QString content = Sk::readUtf8(array);

    content.remove('\r');

    content.remove("\\\n");

    content.replace('\t', ' ');

    WYamlReader reader(content.toUtf8());

    //---------------------------------------------------------------------------------------------
    // Settings

    data.source = WYamlReader::extractString(reader, "source");

    data.api     = WYamlReader::extractString(reader, "api");
    data.version = WYamlReader::extractString(reader, "version");

    //---------------------------------------------------------------------------------------------
    // Backends

    data.backendSearch   = extractList(reader, "backend_search");
    data.backendTrack    = extractList(reader, "backend_track");
    data.backendCover    = extractList(reader, "backend_cover");
    data.backendSubtitle = extractList(reader, "backend_subtitle");

    data.backends = extractItems(reader);

    foreach (const WBackendIndexItem & item, data.backends)
    {
        data.hash.insert(item.id, &item);
    }

    data.covers = extractCovers(reader, data.source, data.hash);

    //---------------------------------------------------------------------------------------------

    emit loaded(data);

    deleteLater();
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

QStringList WBackendIndexQuery::extractList(const WYamlReader & reader, const QString & key) const
{
    QString string = WYamlReader::extractString(reader, key);

    return string.simplified().split(' ');
}

QList<WBackendIndexItem> WBackendIndexQuery::extractItems(const WYamlReader & reader) const
{
    QList<WBackendIndexItem> items;

    QString data = WYamlReader::extractString(reader, "backends");

    QStringList list = data.split('\n');

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
WBackendIndexQuery::extractCovers(const WYamlReader & reader,
                                  const QString     & source,
                                  const QHash<QString, const WBackendIndexItem *> & hash) const
{
    QHash<QString, QString> covers;

    QString path;

    int index = source.lastIndexOf('/');

    if (index == -1)
    {
         path = source + '/';
    }
    else path = source.mid(0, index + 1);

    QString data = WYamlReader::extractString(reader, "covers");

    QStringList list = data.split('\n');

    foreach (const QString & string, list)
    {
        QStringList values = string.simplified().split(' ');

        if (values.count() != 2) continue;

        QString id = values.first();

        if (hash.contains(id) == false) continue;

        QString cover = values.last();

        if (WControllerNetwork::textIsUrl(cover))
        {
             covers.insert(id, cover);
        }
        else covers.insert(id, path + cover);
    }

    return covers;
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

    remote = wControllerDownload->getData(url + "/index.vbml", BACKENDINDEX_TIMEOUT);

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

        emit q->loaded();

        return;
    }

    loadData(array);
}

void WBackendIndexPrivate::onUpdate()
{
    Q_Q(WBackendIndex);

    QByteArray array = remote->readAll();

    QObject::disconnect(remote, 0, q, 0);

    remote->deleteLater();

    remote = NULL;

    if (array.isEmpty())
    {
        emit q->loaded();

        return;
    }

    loadData(array);

    wControllerFile->startWriteFile(WControllerFile::filePath(url + "/index.vbml"), array);
}

//-------------------------------------------------------------------------------------------------

void WBackendIndexPrivate::onData(const WBackendIndexData & data)
{
    Q_Q(WBackendIndex);

    QString version = this->data.version;

    if (version.isEmpty())
    {
        this->data = data;

        emit q->loaded();

        return;
    }

    QString source = data.source;

    source = source.mid(0, source.lastIndexOf('/') + 1);

    QString path = WControllerFile::filePath(url) + '/';

    foreach (const WBackendIndexItem & item, data.backends)
    {
        const WBackendIndexItem * itemHash = this->data.hash.value(item.id);

        if (itemHash)
        {
            if (itemHash->version == item.version) continue;

            WBackendNet * pointer = q->getBackend(item.id);

            if (pointer)
            {
                WBackendUniversal * backend = static_cast<WBackendUniversal *> (pointer);

                backend->update();

                continue;
            }
        }

        QString name = item.id + ".vbml";

        WRemoteData * remote = wControllerDownload->getData(source + name, BACKENDINDEX_TIMEOUT);

        WBackendIndexFile file;

        file.id = item.id;

        file.name = path + name;

        jobs.insert(remote, file);

        QObject::connect(remote, SIGNAL(loaded(WRemoteData *)),
                         q,      SLOT(onItemLoad(WRemoteData *)));
    }


    this->data = data;

    emit q->loaded();

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

    QObject::connect(reply, SIGNAL(actionComplete(bool)), q, SLOT(onActionComplete()));

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

/* Q_INVOKABLE virtual */ void WBackendIndex::update()
{
    Q_D(WBackendIndex);

    QString source = d->data.source;

    if (source.isEmpty())
    {
        qWarning("WBackendIndex::update: Source is empty.");

        return;
    }

    if (d->remote) delete d->remote;

    d->remote = wControllerDownload->getData(source, BACKENDINDEX_TIMEOUT);

    connect(d->remote, SIGNAL(loaded(WRemoteData *)), this, SLOT(onUpdate()));
}

/* Q_INVOKABLE virtual */ void WBackendIndex::reload()
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

/* Q_INVOKABLE virtual */ void WBackendIndex::createFolderItems(WLibraryFolder * folder) const
{
    Q_ASSERT(folder);

    Q_D(const WBackendIndex);

    QString source = sk->applicationUrl() + "?backend=";

    QString cover = d->data.source;

    cover = cover.mid(0, cover.lastIndexOf('/') + 1);

    foreach (const QString & string, d->data.backendTrack)
    {
        WLibraryFolderItem item(WLibraryItem::FolderSearchable, WLocalObject::Default);

        item.source = source + string;

        item.title = d->data.hash.value(string)->title;

        item.cover = cover + "cover/" + string + ".png";

        item.label = string;

        folder->addItem(item);
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendIndex::coverFromId(const QString & id) const
{
    Q_D(const WBackendIndex);

    return d->data.covers.value(id);
}

//-------------------------------------------------------------------------------------------------
// Protected WBackendLoader reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ WBackendNet * WBackendIndex::createBackend(const QString & id) const
{
    Q_D(const WBackendIndex);

    QString source = d->url + '/' + id + ".vbml";

    WBackendUniversal * backend = new WBackendUniversal(id, source);

    connect(backend, SIGNAL(updated()), this, SLOT(onBackendUpdate()));

    while (backend->isLoaded() == false)
    {
        QCoreApplication::processEvents();
    }

    const WBackendIndexItem * item = d->data.hash.value(id);

    if (item && item->version != backend->d_func()->data.version)
    {
        backend->update();
    }

    return backend;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendIndex::getId(const QString & url) const
{
    Q_D(const WBackendIndex);

    QRegExp regExp;

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
    Q_D(const WBackendIndex); return d->data.backendCover;
}

#endif // SK_NO_BACKENDINDEX

#include "WBackendIndex.moc"
