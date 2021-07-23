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
    QString url;

    WBackendNet * backend;

    WBackendNetQuery query;

    QList<WMediaReply *> replies;

    QIODevice * reply;
};

//-------------------------------------------------------------------------------------------------
// WPrivateMediaMode
//-------------------------------------------------------------------------------------------------

struct WPrivateMediaMode
{
    QHash<WAbstractBackend::Quality, QString> medias;
    QHash<WAbstractBackend::Quality, QString> audios;

    QDateTime expiry;
};

//-------------------------------------------------------------------------------------------------
// WPrivateMediaSource
//-------------------------------------------------------------------------------------------------

struct WPrivateMediaSource
{
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

    void updateSources();

    void clearReply(WMediaReply * reply);

    void deleteMedia(WPrivateMediaData * media);

    void getData(WPrivateMediaData * media, WBackendNetQuery * query);

public: // Slots
    void onLoaded(WRemoteData * data);

    void onSourceLoaded(QIODevice * device, const WBackendNetSource & source);

public: // Variables
    QThread * thread;

    WVlcEngine * engine;

    WAbstractLoader * loader;

    QList<WPrivateMediaData *> medias;

    QHash<WRemoteData *, WPrivateMediaData *> jobs;

    QHash<QIODevice *, WPrivateMediaData *> queries;

    QStringList                         urls;
    QHash<QString, WPrivateMediaSource> sources;

protected:
    W_DECLARE_PUBLIC(WControllerMedia)
};

#endif // SK_NO_CONTROLLERMEDIA
#endif // WCONTROLLERMEDIA_P_H
