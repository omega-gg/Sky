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

#ifndef WCONTROLLERMEDIA_H
#define WCONTROLLERMEDIA_H

#ifdef QT_4
// Qt includes
#include <QStringList>
#endif

// Sk includes
#include <WController>
#include <WBackendNet>

#ifndef SK_NO_CONTROLLERMEDIA

// Forward declarations
class WControllerMediaPrivate;
class WAbstractLoader;
class WYamlNodeBase;
class WYamlNode;
#ifndef SK_NO_PLAYER
class WVlcEngine;
class WVlcPlayer;
#endif

#ifdef QT_6
#ifndef SK_NO_PLAYER
Q_MOC_INCLUDE("WVlcEngine")
#endif
Q_MOC_INCLUDE("WAbstractLoader")
#endif

// Defines
#define wControllerMedia WControllerMedia::instance()

//-------------------------------------------------------------------------------------------------
// WMediaReply
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WMediaReply : public QObject
{
    Q_OBJECT

private:
    explicit WMediaReply(QObject * parent = NULL);
public:
    /* virtual */ ~WMediaReply();

public: // Virtual interface
    Q_INVOKABLE virtual QString toVbml() const;

signals:
    void loaded(WMediaReply * reply);

public: // Properties
    QString url      () const;
    QString urlSource() const;

    WTrack::Type type      () const;
    WTrack::Type typeSource() const;

    QString timeZone() const;

    int currentTime() const;
    int duration   () const;

    int timeA() const;
    int timeB() const;

    int start() const;

    QHash<WAbstractBackend::Quality, QString> medias() const;
    QHash<WAbstractBackend::Quality, QString> audios() const;

    bool isLoaded() const;

    bool hasError() const;

    QString error() const;

private: // Variables
    QString _url;
    QString _urlSource;

    WAbstractBackend::SourceMode _mode;

    WTrack::Type _type;
    WTrack::Type _typeSource;

    QString _timeZone;

    int _currentTime;
    int _duration;

    int _timeA;
    int _timeB;

    int _start;

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
// WControllerMediaData
//-------------------------------------------------------------------------------------------------

struct WControllerMediaData
{
public:
    WControllerMediaData()
    {
        type = WTrack::Track;

        currentTime = 0;

        duration = -1;

        timeA =  0;
        timeB = -1;

        start = -1;
    }

public: // Interface
    void applyVbml(const QByteArray & array, const QString & url);
    void applyM3u (const QByteArray & array, const QString & url);

private: // Functions
    void extractSource(const QList<WYamlNode> & children);

    void applyMedia(const WYamlNodeBase & node, const QString & url);

    void applyEmpty();

public: // Variables
    WTrack::Type type;

    QString origin;
    QString source;

    QString timeZone;

    int currentTime;
    int duration;

    int timeA;
    int timeB;

    int start;

    QHash<WAbstractBackend::Quality, QString> medias;
};

//-------------------------------------------------------------------------------------------------
// WControllerMedia
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WControllerMedia : public WController
{
    Q_OBJECT

#ifndef SK_NO_PLAYER
    Q_PROPERTY(WVlcEngine * engine READ engine CONSTANT)
#endif

    Q_PROPERTY(WAbstractLoader * loader READ loader WRITE setLoader NOTIFY loaderChanged)

private:
    WControllerMedia();

public: // Initialize
    virtual void initController(const QStringList & options = QStringList());

public: // Interface
#ifndef SK_NO_PLAYER
    Q_INVOKABLE WVlcPlayer * createVlcPlayer() const;
#endif

    Q_INVOKABLE
    WMediaReply * getMedia(const QString              & url,
                           QObject                    * parent = NULL,
                           WAbstractBackend::SourceMode mode   = WAbstractBackend::SourceDefault,
                           int                          currentTime = -1);

    // NOTE: This function is provided for convenient loading of specific media sources.
    Q_INVOKABLE WMediaReply * getMedia(const QString              & url,
                                       WAbstractBackend::SourceMode mode,
                                       int                          currentTime = -1);

    Q_INVOKABLE void clearMedia (const QString & url);
    Q_INVOKABLE void clearMedias();

public: // Static functions
    Q_INVOKABLE static QString generateSource(const QString & url);

signals:
    void loaderChanged();

public: // Properties
#ifndef SK_NO_PLAYER
    WVlcEngine * engine() const;
#endif

    WAbstractLoader * loader() const;
    void              setLoader(WAbstractLoader * loader);

private:
    W_DECLARE_PRIVATE   (WControllerMedia)
    W_DECLARE_CONTROLLER(WControllerMedia)

    Q_PRIVATE_SLOT(d_func(), void onLoaded(WRemoteData *))

    Q_PRIVATE_SLOT(d_func(), void onSourceLoaded(QIODevice *, const WBackendNetSource &))

    Q_PRIVATE_SLOT(d_func(), void onUrl(QIODevice *, const WControllerMediaData &))

    friend class WMediaReply;
};

#include <private/WControllerMedia_p>

#endif // SK_NO_CONTROLLERMEDIA
#endif // WCONTROLLERMEDIA_H
