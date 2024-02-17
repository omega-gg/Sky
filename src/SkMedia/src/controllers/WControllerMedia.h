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
#include <WYamlReader>

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

    bool isVbml() const;

    QString timeZone() const;

    int currentTime() const;
    int duration   () const;

    int timeA() const;
    int timeB() const;

    int start() const;

    QString context() const;

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

    bool _vbml;

    QString _timeZone;

    int _currentTime;
    int _duration;

    int _timeA;
    int _timeB;

    int _start;

    QString _context;

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

class WControllerMediaSource
{
public:
    WControllerMediaSource(const WYamlNode * node, int index);

public: // Interface
    int getDuration(int at) const;

public: // Variables
    const WYamlNode * node;

    QString id;

    int index;

    int duration;

    int at;
    int end;
};

struct WControllerMediaObject
{
    QString id;

    WControllerMediaSource * media;

    int duration;
    int at;
};

class WControllerMediaData
{
public:
    WControllerMediaData();

public: // Interface
    void applyVbml(const QByteArray & array, const QString & url,
                                             const QString & argument);

    void applyM3u(const QByteArray & array, const QString & url);

public: // Static functions
    static QStringList extractTags(const WYamlReader & reader);

    static QList<WControllerMediaSource> extractSources(const WYamlReader & reader);

    static QString extractResult(const WYamlReader & reader, const QString     & argument,
                                                             const QStringList & context,
                                                             const QString     & currentId);

    static int applyDurations(QList<WControllerMediaObject> * timeline);

    static QHash<QString, WControllerMediaSource *>
    generateHash(QList<WControllerMediaSource> & sources);

    static QList<WControllerMediaObject>
    generateTimeline(const QHash<QString, WControllerMediaSource *> & hash,
                     const QStringList                              & context,
                     const QStringList                              & tags);

    static QString generateContext(const QList<WControllerMediaObject> & timeline,
                                   const QString                       & currentId);

    static WControllerMediaSource *
    getMediaSource(const QHash<QString, WControllerMediaSource *> & hash, const QString & id);

    static QString getContext(const QList<WControllerMediaObject> & timeline,
                              const QString                       & currentId);

    // NOTE: This function extracts the context as a list and populates the currentId. Both
    //       curentId:value,value and value,value formats are supported. Passing the 'clear'
    //       argument sets the currentId to "".
    static QStringList getContextList(const QString & context,
                                      const QString & argument, QString * currentId);

    static void dumpTimeline(const QList<WControllerMediaObject> & timeline);

public: // Functions
    void extractSourceDuration(const QList<WYamlNode> & children,
                               const QList<int>       & durations,
                               const QList<int>       & starts);

    int extractSourceTimeline(const QList<WControllerMediaObject> & timeline, QString * currentId);

    void extractSource(const QList<WYamlNode> & children);

    void applySource(const WYamlNode & node, const QString & url, int duration);

    void applyMedia(const WYamlNodeBase & node, const QString & url);

    void applyEmpty();

    QString updateCurrentTime(const QList<WControllerMediaObject> & timeline,
                              const QString                       & currentId);

    // NOTE: This function cleans the timeline to ensure that we don't have redundant id(s).
    QString cleanTimeline(QList<WControllerMediaObject> & timeline,
                          const QString                 & currentId, int index);

private: // Functions
    QStringList getContext(const QList<WControllerMediaObject> & timeline, int count) const;

    int getRedundancy(const QList<WControllerMediaObject> & timeline, int index) const;

    int getIndexFromId(const QList<WControllerMediaObject> & timeline,
                       const QString                       & id, int from) const;

public: // Variables
    WTrack::Type type;
    WTrack::Type typeSource;

    bool vbml;

    QString origin;
    QString source;

    QString timeZone;

    int currentTime;
    int duration;

    int timeA;
    int timeB;

    int start;

    QString context;

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
