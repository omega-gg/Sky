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

#ifndef WBACKENDUNIVERSAL_P_H
#define WBACKENDUNIVERSAL_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Private includes
#include <private/WBackendNet_p>

#ifndef SK_NO_BACKENDUNIVERSAL

// Forward declarations
class WRemoteData;

//-------------------------------------------------------------------------------------------------
// WBackendUniversalData
//-------------------------------------------------------------------------------------------------

struct WBackendUniversalData
{
public: // Enums
    enum Engine
    {
        None       = 0x0,
        Tracks     = 0x1,
        CoverAudio = 0x2,
        CoverVideo = 0x4
    };
    Q_DECLARE_FLAGS(Engines, Engine)

public:
    WBackendUniversalData()
    {
        valid = true;

        isSearchEngine = false;
        isSearchCover  = false;
    }

public: // Variables
    bool valid;

    QString origin;

    QString api;
    QString version;

    Engines engines;

    bool isSearchEngine;
    bool isSearchCover;

    QString title;

    QString host;
    QString cover;
    QString hub;

    QList<WLibraryFolderItem> items;

    QString validate;

    //---------------------------------------------------------------------------------------------

    QString trackId;
    QString trackOutput;

    QString playlistInfo;

    QString urlTrack;
    QString urlPlaylist;

    QString querySource;
    QString queryTrack;
    QString queryPlaylist;
    QString queryFolder;
    QString queryItem;

    QString createQuery;

    QString extractSource;
    QString extractTrack;
    QString extractPlaylist;
    QString extractFolder;
    QString extractItem;

    QString queryFailed;

    QString applySource;
    QString applyTrack;
    QString applyPlaylist;
    QString applyFolder;
    QString applyItem;
};

//-------------------------------------------------------------------------------------------------
// WBackendUniversalPrivate
//-------------------------------------------------------------------------------------------------

class SK_BACKEND_EXPORT WBackendUniversalPrivate : public WBackendNetPrivate
{
public:
    WBackendUniversalPrivate(WBackendUniversal * p);

    ///* virtual */ ~WBackendUniversalPrivate();

    void init(const QString & id, const QString & source);

public: // Functions
    void load();

    void loadData(const QByteArray & array);

    void applyLoaded();

    void runQuery(WBackendNetQuery * query, const QString & name,
                                            const QString & source, const QString & url) const;

    void applyQueryParameters(WBackendUniversalParameters * parameters,
                              const WBackendNetQuery      & query) const;

    void applyQueryResults(WBackendUniversalParameters * parameters,
                           WBackendNetQuery            * query) const;

    //---------------------------------------------------------------------------------------------

    void applySourceParameters(WBackendUniversalParameters * parameters,
                               const QByteArray            & data,
                               const WBackendNetQuery      & query,
                               const WBackendNetSource     & reply) const;

    void applySourceResults(WBackendUniversalParameters * parameters,
                            WBackendNetSource           * reply) const;

    //---------------------------------------------------------------------------------------------

    void applyTrackParameters(WBackendUniversalParameters * parameters,
                              const QByteArray            & data,
                              const WBackendNetQuery      & query,
                              const WBackendNetTrack      & reply) const;

    void applyTrackResults(WBackendUniversalParameters * parameters,
                           WBackendNetTrack            * reply) const;

    //---------------------------------------------------------------------------------------------

    void applyPlaylistParameters(WBackendUniversalParameters * parameters,
                                 const QByteArray            & data,
                                 const WBackendNetQuery      & query,
                                 const WBackendNetPlaylist   & reply) const;

    void applyPlaylistResults(WBackendUniversalParameters * parameters,
                              WBackendNetPlaylist         * reply) const;

    //---------------------------------------------------------------------------------------------

    void applyFolderParameters(WBackendUniversalParameters * parameters,
                               const QByteArray            & data,
                               const WBackendNetQuery      & query,
                               const WBackendNetFolder     & reply) const;

    void applyFolderResults(WBackendUniversalParameters * parameters,
                            WBackendNetFolder           * reply) const;

    //---------------------------------------------------------------------------------------------

    void applyItemParameters(WBackendUniversalParameters * parameters,
                             const QByteArray            & data,
                             const WBackendNetQuery      & query,
                             const WBackendNetItem       & reply) const;

    void applyItemResults(WBackendUniversalParameters * parameters,
                          WBackendNetItem             * reply) const;

    //---------------------------------------------------------------------------------------------

    void applyTrack(QList<WTrack>             * tracks, const QVariant & value) const;
    void applyItem (QList<WLibraryFolderItem> * items,  const QVariant & value) const;

    void applyQueries(QList<WBackendNetQuery> * queries, QVariant * value) const;

    void applyQuery(WBackendNetQuery * query, const QVariant & value) const;

    void applyQualities(QHash<WAbstractBackend::Quality, QString> * qualities,
                        QVariant                                  * value) const;

    void applyChapters(QList<WChapter> * chapters, QVariant * value) const;

    //---------------------------------------------------------------------------------------------

    WBackendNetQuery getQuery(const QVariant & value) const;

    WAbstractBackend::Quality getQuality(const QString & string) const;

    QDateTime getDate(const QVariant & value) const;

    WLibraryItem::Type     getType     (const QString & string) const;
    WBackendNetQuery::Type getTypeQuery(const QString & string) const;
    WTrack::Type           getTypeTrack(const QString & string) const;

    WLocalObject::State getState     (const QString & string) const;
    WTrack::State       getStateTrack(const QString & string) const;

    const QVariant * getVariant(const QHash<QString, QVariant> * hash, const QString & name) const;

public: // Events
    void onLoad  ();
    void onUpdate();

    void onData(const WBackendUniversalData & data);

public: // Variables
    WRemoteData * remote;

    WBackendUniversalData data;

    QString id;
    QString source;

    QVariant global;

    QMetaMethod method;

protected:
    W_DECLARE_PUBLIC(WBackendUniversal)
};

#endif // SK_NO_BACKENDUNIVERSAL
#endif // WBACKENDUNIVERSAL_P_H
