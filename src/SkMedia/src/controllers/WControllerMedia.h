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
#include <WControllerPlaylist>
#include <WBackendNet>
#include <WYamlReader>
#include <WSubtitle>

// Defines
// NOTE iOS: VLC seems to be prefer being run on the main thread, in particular for bonjour
//           chromecast discovery. We can still uncomment this to enable threading for testing.
// NOTE VLC: libvlc_new seems to freeze the UI when it's started from the main thread.
#ifndef Q_OS_IOS
#define CONTROLLERMEDIA_THREAD
#endif

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

    WTrack::Type type() const;
    void         setType(WTrack::Type type);

    WTrack::Type typeSource() const;
    void         setTypeSource(WTrack::Type type);

    bool isVbml() const;

    QString timeZone() const;

    int currentTime() const;
    int duration   () const;

    int timeA() const;
    int timeB() const;

    int start() const;

    QString context  () const;
    QString contextId() const;

    QHash<WAbstractBackend::Quality, QString> medias() const;
    QHash<WAbstractBackend::Quality, QString> audios() const;

    QList<WChapter> chapters() const;

    QString ambient() const;

    QList<WSubtitle> subtitles() const;

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
    QString _contextId;

    QHash<WAbstractBackend::Quality, QString> _medias;
    QHash<WAbstractBackend::Quality, QString> _audios;

    QList<WChapter> _chapters;

    QString _ambient;

    QList<WSubtitle> _subtitles;

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

class WControllerMediaData
{
public:
    WControllerMediaData();

public: // Interface
    void applyVbml(const QByteArray & array, const QString & url, const QString & urlBase);

    void applyM3u(const QByteArray & array, const QString & url);

    void applySource(const WYamlNodeBase & root,
                     const WYamlNode     & node,
                     const QString       & url, const QString & baseUrl, int duration);

    void applyMedia(const WYamlNodeBase & root, const QString & url, const QString & baseUrl);

    void applyData(const WYamlNodeBase & node, const QString & url, const QString & baseUrl);

    void applyEmpty();

    int extractDuration(const WYamlNode & child, QList<int> * starts,
                                                 QList<int> * durations, int & start);

    void extractSource(const WYamlNodeBase    & root,
                       const QList<WYamlNode> & children, const QString & baseUrl);

    void extractSourceDuration(const WYamlNodeBase    & root,
                               const QList<WYamlNode> & children,
                               const QList<int>       & durations,
                               const QList<int>       & starts, const QString & baseUrl);

    int extractSourceTimeline(const WYamlNodeBase                 & root,
                              const QList<WControllerMediaObject> & timeline,
                              const QString                       & baseUrl);

    void interpolateTime(const QList<WYamlNode> & children);

    void addChapter(const WYamlNodeBase & node, int time, const QString & baseUrl);

    int updateCurrentTime(const WYamlNodeBase                 & root,
                          const QList<WControllerMediaObject> & timeline, const QString & baseUrl);

    // NOTE: This function cleans the timeline to ensure that we don't have redundant id(s).
    QString cleanTimeline(QList<WControllerMediaObject> & timeline, int index,
                          const QString                 & baseUrl);

public: // Static functions
    static QVariant extractResult(const WYamlReader & reader, const QString & argument,
                                  const QStringList & context,
                                  const QString     & contextId);

    static QString generateContext(const QList<WControllerMediaObject> & timeline);

    static QString getContext(const QList<WControllerMediaObject> & timeline);

    static void dumpTimeline(const QList<WControllerMediaObject> & timeline);

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
    QString contextId;

    QHash<WAbstractBackend::Quality, QString> medias;

    QList<WChapter> chapters;

    QString ambient;

    QList<WSubtitle> subtitles;
};

//-------------------------------------------------------------------------------------------------
// WControllerMedia
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WControllerMedia : public WController
{
    Q_OBJECT

    Q_PROPERTY(WAbstractLoader * loader READ loader WRITE setLoader NOTIFY loaderChanged)

#ifndef SK_NO_PLAYER
    Q_PROPERTY(QString versionVlc READ versionVlc CONSTANT)
#endif

private:
    WControllerMedia();

public: // Initialize
    virtual void initController(const QStringList & options = QStringList());

public: // Interface
#ifndef SK_NO_PLAYER
    Q_INVOKABLE WVlcPlayer * createVlcPlayer();

    Q_INVOKABLE void startLog();
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
    WAbstractLoader * loader() const;
    void              setLoader(WAbstractLoader * loader);

#ifndef SK_NO_PLAYER
    static QString versionVlc();
#endif

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
