//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkMedia module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WCONTROLLERMEDIA_H
#define WCONTROLLERMEDIA_H

// Sk includes
#include <WController>
#include <WBackendNet>

#ifndef SK_NO_CONTROLLERMEDIA

// Forward declarations
class WControllerMediaPrivate;
class WAbstractLoader;
class WVlcEngine;
class WVlcPlayer;

// Defines
#define wControllerMedia WControllerMedia::instance()

//-------------------------------------------------------------------------------------------------
// WMediaReply
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WMediaReply : public QObject
{
    Q_OBJECT

private:
    WMediaReply(const QString & url, QObject * parent);
public:
    /* virtual */ ~WMediaReply();

signals:
    void loaded(WMediaReply * reply);

public: // Properties
    QString url() const;

    QHash<WAbstractBackend::Quality, QString> medias() const;
    QHash<WAbstractBackend::Quality, QString> audios() const;

    bool isLoaded() const;

    bool hasError() const;

    QString error() const;

private: // Variables
    QString _url;

    QHash<WAbstractBackend::Quality, QString> _medias;
    QHash<WAbstractBackend::Quality, QString> _audios;

    bool _loaded;

    QString _error;

    WBackendNet * _backend;

    WBackendNetQuery _query;

private:
    friend class WControllerMedia;
    friend class WControllerMediaPrivate;
};

//-------------------------------------------------------------------------------------------------
// WControllerMedia
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WControllerMedia : public WController
{
    Q_OBJECT

    Q_PROPERTY(WVlcEngine * engine READ engine CONSTANT)

    Q_PROPERTY(WAbstractLoader * loader READ loader WRITE setLoader NOTIFY loaderChanged)

private:
    WControllerMedia();

protected: // Initialize
    /* virtual */ void init();

public: // Interface
    Q_INVOKABLE WVlcPlayer * createVlcPlayer() const;

    Q_INVOKABLE WMediaReply * getMedia(const QString & url, QObject * parent = NULL);

    Q_INVOKABLE void clearMedia (const QString & url);
    Q_INVOKABLE void clearMedias();

signals:
    void loaderChanged();

public: // Properties
    WVlcEngine * engine() const;

    WAbstractLoader * loader() const;
    void              setLoader(WAbstractLoader * loader);

private:
    W_DECLARE_PRIVATE   (WControllerMedia)
    W_DECLARE_CONTROLLER(WControllerMedia)

    Q_PRIVATE_SLOT(d_func(), void onLoaded(WRemoteData *))

    Q_PRIVATE_SLOT(d_func(), void onSourceLoaded(QIODevice *, const WBackendNetSource &))

    friend class WMediaReply;
};

#include <private/WControllerMedia_p>

#endif // SK_NO_CONTROLLERMEDIA
#endif // WCONTROLLERMEDIA_H
