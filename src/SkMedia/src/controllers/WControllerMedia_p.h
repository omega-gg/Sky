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

#ifndef WCONTROLLERMEDIA_P_H
#define WCONTROLLERMEDIA_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QDateTime>
#ifdef QT_4
#include <QStringList>
#endif

// Private includes
#include <private/WController_p>

#ifndef SK_NO_CONTROLLERMEDIA

// Forward declarations
class WRemoteData;

//-------------------------------------------------------------------------------------------------
// WPrivateMediaData
//-------------------------------------------------------------------------------------------------

struct WPrivateMediaData
{
    WTrack::Type type;

    QString url;

    int currentTime;
    int duration;

    int timeMedia;

    int timeA;
    int timeB;

    int start;

    WBackendNet * backend;

    WBackendNetQuery query;

    QList<WMediaReply *> replies;

    QIODevice * reply;
};

//-------------------------------------------------------------------------------------------------
// WPrivateMediaSlice
//-------------------------------------------------------------------------------------------------

struct WPrivateMediaSlice
{
    int duration;

    int timeA;
    int timeB;

    int start;
    int end;

    QHash<WAbstractBackend::Quality, QString> medias;
    QHash<WAbstractBackend::Quality, QString> audios;

    QDateTime expiry;
};

//-------------------------------------------------------------------------------------------------
// WPrivateMediaMode
//-------------------------------------------------------------------------------------------------

struct WPrivateMediaMode
{
    QList<WPrivateMediaSlice> slices;
};

//-------------------------------------------------------------------------------------------------
// WPrivateMediaSource
//-------------------------------------------------------------------------------------------------

struct WPrivateMediaSource
{
    WTrack::Type type;

    QHash<WAbstractBackend::SourceMode, WPrivateMediaMode> modes;
};

//-------------------------------------------------------------------------------------------------
// WControllerMediaPrivate
//-------------------------------------------------------------------------------------------------

class SK_MEDIA_EXPORT WControllerMediaPrivate : public WControllerPrivate
{
public:
    WControllerMediaPrivate(WControllerMedia * p);

    /* virtual */ ~WControllerMediaPrivate();

    void init(const QStringList & options);

public: // Functions
    void loadSources(WMediaReply * reply);

    void loadUrl(QIODevice * device, const WBackendNetQuery & query, int time, int duration,
                 int start) const;

    void applyData(WPrivateMediaData * media, const WControllerMediaData & data);

    void applySource(WPrivateMediaData       * media,
                     const WBackendNetSource & source,
                     WAbstractBackend::SourceMode mode,
                     int duration, int timeA, int timeB, int start);

    void appendSlice(const WPrivateMediaSlice & slice,
                     const QString            & url,
                     WAbstractBackend::SourceMode mode, WTrack::Type type);

    void updateSources();

    void clearReply(WMediaReply * reply);

    int checkMax(WPrivateMediaData * media, WBackendNetQuery & query);

    bool resolve(const QString & backendId, WBackendNetQuery & query);

    void getData(WPrivateMediaData * media, WBackendNetQuery * query);

    WPrivateMediaSource * getSource(const QString & url);

    WPrivateMediaMode * getMode(WPrivateMediaSource * source, WAbstractBackend::SourceMode mode);

    const WPrivateMediaSlice * getSlice(WPrivateMediaSource * source,
                                        WAbstractBackend::SourceMode mode, int currentTime);

public: // Slots
    void onLoaded(WRemoteData * data);

    void onSourceLoaded(QIODevice * device, const WBackendNetSource & source);

    void onUrl(QIODevice * device, const WControllerMediaData & data);

public: // Variables
    QThread * thread;

#ifndef SK_NO_PLAYER
    WVlcEngine * engine;
#endif

    WAbstractLoader * loader;

    QList<WPrivateMediaData *> medias;

    QHash<WRemoteData *, WPrivateMediaData *> jobs;

    QHash<QIODevice *, WPrivateMediaData *> queries;

    QStringList                         urls;
    QHash<QString, WPrivateMediaSource> sources;

    QMetaMethod methodVbml;
    QMetaMethod methodM3u;

protected:
    W_DECLARE_PUBLIC(WControllerMedia)
};

#endif // SK_NO_CONTROLLERMEDIA
#endif // WCONTROLLERMEDIA_P_H
