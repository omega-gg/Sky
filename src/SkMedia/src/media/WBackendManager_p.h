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

#ifndef WBACKENDMANAGER_P_H
#define WBACKENDMANAGER_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Sk includes
#include <WTrack>

// Private includes
#include <private/WAbstractBackend_p>

#ifndef SK_NO_BACKENDMANAGER

// Forward declarations
class WMediaReply;

//=================================================================================================
// WBackendManagerItem
//=================================================================================================

struct WBackendManagerItem
{
    WAbstractBackend * backend;
    WAbstractHook    * hook;
};

//=================================================================================================
// WBackendManager
//=================================================================================================

class SK_MEDIA_EXPORT WBackendManagerPrivate : public WAbstractBackendPrivate
{
public:
    WBackendManagerPrivate(WBackendManager * p);

    /* virtual */ ~WBackendManagerPrivate();

    void init();

public: // Functions
    void loadSources ();
    void applySources(bool play);

    void applyBackend(const QString & source);

    void updateLoading();

    void clearActive();

    void clearReply();
    void clearMedia();

    void setBackend(WAbstractBackend * backend);

public: // Slots
    void onLoaded();

    void onState     ();
    void onStateLoad ();
    void onTime      ();
    void onVideos    ();
    void onAudios    ();
    void onTrackVideo();
    void onTrackAudio();

public: // Variables
    QList<WBackendManagerItem> items;

    WAbstractBackend  * backend;
    WBackendInterface * backendInterface;

    WMediaReply * reply;

    QHash<WAbstractBackend::Quality, QString> medias;
    QHash<WAbstractBackend::Quality, QString> audios;

    QString currentMedia;
    QString currentAudio;

protected:
    W_DECLARE_PUBLIC(WBackendManager)
};

#endif // SK_NO_BACKENDMANAGER
#endif // WBACKENDMANAGER_P_H
