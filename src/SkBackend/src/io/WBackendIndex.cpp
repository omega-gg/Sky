//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

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
#include <WControllerDownload>
#include <WControllerPlaylist>
#include <WBackendUniversal>
#include <WYamlReader>

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

//=================================================================================================
// WBackendIndexPrivate
//=================================================================================================

WBackendIndexPrivate::WBackendIndexPrivate(WBackendIndex * p) : WBackendLoaderPrivate(p) {}

//-------------------------------------------------------------------------------------------------

void WBackendIndexPrivate::init(const QString & url)
{
    W_GET_CONTROLLER(WControllerPlaylist, controller);

    if (controller == NULL)
    {
        qWarning("WBackendIndexPrivate::init: WControllerPlaylist does not exist.");

        thread = NULL;
    }
    else thread = controller->thread();

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

    remote = wControllerDownload->getData(url + "/index.vbml");

    QObject::connect(remote, SIGNAL(loaded(WRemoteData *)), q, SLOT(onLoaded()));
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WBackendIndexPrivate::onLoaded()
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

    WBackendIndexQuery * query = new WBackendIndexQuery;

    QObject::connect(query, SIGNAL(loaded(WBackendIndexData)), q, SLOT(onData(WBackendIndexData)));

    if (thread)
    {
        query->moveToThread(thread);

        method.invoke(query, Q_ARG(const QByteArray &, array));
    }
    else query->extract(array);
}

void WBackendIndexPrivate::onData(const WBackendIndexData & data)
{
    Q_Q(WBackendIndex);

    this->data = data;

    emit q->loaded();
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
// WBackendLoader reimplementation
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
// Protected WBackendLoader reimplementation
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ WBackendNet * WBackendIndex::createBackend(const QString & id) const
{
    Q_D(const WBackendIndex);

    QString source = d->url + '/' + id + ".vbml";

    WBackendUniversal * backend = new WBackendUniversal(id, source);

    while (backend->isLoaded() == false)
    {
        QCoreApplication::processEvents();
    }

    return backend;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE virtual */ QString WBackendIndex::matchBackend(const QString & source) const
{
    Q_D(const WBackendIndex);

    QRegExp regExp;

    foreach (const WBackendIndexItem & item, d->data.backends)
    {
        regExp.setPattern(item.validate);

        if (source.indexOf(regExp) != -1)
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
