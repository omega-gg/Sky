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

#ifndef WBACKENDUNIVERSAL_H
#define WBACKENDUNIVERSAL_H

// Sk includes
#include <WBackendNet>
#include <WControllerPlaylist>
#ifndef SK_NO_TORRENT
#include <WControllerNetwork>
#endif
#include <WRegExp>

#ifndef SK_NO_BACKENDUNIVERSAL

// Forward declarations
class  WBackendUniversalPrivate;
class  WBackendUniversalParameters;
struct WBackendUniversalData;

//-------------------------------------------------------------------------------------------------
// WBackendUniversalNode
//-------------------------------------------------------------------------------------------------

class SK_BACKEND_EXPORT WBackendUniversalNode
{
public: // Enums
    enum Type
    {
        String,
        Variable,
        Function
    };

public:
    WBackendUniversalNode();
    WBackendUniversalNode(Type type);

public: // Interface
    QVariant run(WBackendUniversalParameters * parameters) const;

    void dump(int indent = 0) const;

    void applySlice(WControllerPlaylistData * data, const QStringList & list) const;

    QVariant getVariant(WBackendUniversalParameters * parameters, int index) const;

    int   getInt (WBackendUniversalParameters * parameters, int index) const;
    qreal getReal(WBackendUniversalParameters * parameters, int index) const;

    QByteArray getByteArray(WBackendUniversalParameters * parameters, int index) const;
    QString    getString   (WBackendUniversalParameters * parameters, int index) const;

    QVariantList getList(WBackendUniversalParameters * parameters, int index) const;

#ifndef SK_NO_TORRENT
    QVariantList getTorrentItems  (const QList<WTorrentItemData>   & items)   const;
    QVariantList getTorrentFolders(const QList<WTorrentItemFolder> & folders) const;
#endif

    QStringList getStringList(WBackendUniversalParameters * parameters, int index) const;

    QList<WRegExp> getRegExps(const QVariantList & variants) const;

    QHash<QString, QVariant> getHash(WBackendUniversalParameters * parameters, int index) const;

    QHash<QString, QVariant> getHtml(const WControllerPlaylistData & data) const;

    QVariantList getPlaylists(const QList<WControllerPlaylistUrl> & urls) const;

    QVariant * getKey(WBackendUniversalParameters * parameters, int index) const;

    const QVariantList variants(const QStringList & list) const;

#ifndef SK_NO_TORRENT
    QList<WTorrentItemData> torrentItems(WBackendUniversalParameters * parameters,
                                         int                           index) const;
#endif

public: // Variables
    Type type;

    QString data;

    QList<WBackendUniversalNode> nodes;
};

//-------------------------------------------------------------------------------------------------
// WBackendUniversalScript
//-------------------------------------------------------------------------------------------------

class SK_BACKEND_EXPORT WBackendUniversalScript
{
public:
    WBackendUniversalScript(const QString & data);

public: // Interface
    bool isValid() const;

    QVariant run(WBackendUniversalParameters * parameters) const;

    void dump() const;

private: // Functions
    void load(const QString & data);

    bool loadParameters(WBackendUniversalNode * node,
                        QString               * string, const WRegExp & regExp) const;

    bool loadFunction(WBackendUniversalNode * node,
                      QString               * string, const WRegExp & regExp) const;

    QString extractWord(QString * string) const;

    QString extractString(QString * string, const QChar & character) const;

    bool skipCondition(int * index) const;

    void skipLoop(int * index) const;
    bool skipEnd (int * index) const;

    void skipOperators(int * index) const;

    bool getCondition(WBackendUniversalParameters * parameters,
                      const WBackendUniversalNode & node, int * index) const;

public: // Properties
    QList<WBackendUniversalNode> nodes;
};

//-------------------------------------------------------------------------------------------------
// WBackendUniversalParameters
//-------------------------------------------------------------------------------------------------

class SK_BACKEND_EXPORT WBackendUniversalParameters
{
public:
    WBackendUniversalParameters(const WBackendUniversalScript & script,
                                const QVariant                & global = QVariant());

public: // Interface
    void add(const QString & name, const QVariant & value = QVariant());

    QVariant       * value     (const QString & name);
    const QVariant * valueConst(const QString & name) const;

private: // Functions
    void extract(QStringList * list, const WBackendUniversalNode & node);

public: // Variables
    QHash<QString, QVariant> parameters;
};

//-------------------------------------------------------------------------------------------------
// WBackendUniversal
//-------------------------------------------------------------------------------------------------

class SK_BACKEND_EXPORT WBackendUniversal : public WBackendNet
{
    Q_OBJECT

protected:
    WBackendUniversal(const QString & id, const QString & source);

public: // WBackendNet implementation
    /* Q_INVOKABLE virtual */ QString getId   () const;
    /* Q_INVOKABLE virtual */ QString getTitle() const;

public: // WBackendNet reimplementation
    /* Q_INVOKABLE virtual */ void update();
    /* Q_INVOKABLE virtual */ void reload();

    //---------------------------------------------------------------------------------------------

    /* Q_INVOKABLE virtual */ bool hasSearch() const;

    /* Q_INVOKABLE virtual */ bool isSearchEngine() const;
    /* Q_INVOKABLE virtual */ bool isSearchCover () const;

    /* Q_INVOKABLE virtual */ QString validate() const;

    /* Q_INVOKABLE virtual */ bool checkCover(const QString & label, const QString & q) const;

    /* Q_INVOKABLE virtual */ QString getHost () const;
    /* Q_INVOKABLE virtual */ QString getCover() const;
    /* Q_INVOKABLE virtual */ QString getHub  () const;

    /* Q_INVOKABLE virtual */ QList<WLibraryFolderItem> getLibraryItems() const;

    /* Q_INVOKABLE virtual */ WLibraryFolderItem getLibraryItem(const QString & id) const;

    /* Q_INVOKABLE virtual */ QString getTrackId(const QString & url) const;

    /* Q_INVOKABLE virtual */ WAbstractBackend::Output getTrackOutput(const QString & url) const;

    /* Q_INVOKABLE virtual */ WBackendNetPlaylistInfo getPlaylistInfo(const QString & url) const;

    /* Q_INVOKABLE virtual */ QString getUrlTrack(const QString & id) const;

    /* Q_INVOKABLE virtual */ QString getUrlPlaylist(const WBackendNetPlaylistInfo & info) const;

    //---------------------------------------------------------------------------------------------

    /* Q_INVOKABLE virtual */ WBackendNetQuery getQuerySource  (const QString & url) const;
    /* Q_INVOKABLE virtual */ WBackendNetQuery getQueryTrack   (const QString & url) const;
    /* Q_INVOKABLE virtual */ WBackendNetQuery getQueryPlaylist(const QString & url) const;
    /* Q_INVOKABLE virtual */ WBackendNetQuery getQueryFolder  (const QString & url) const;
    /* Q_INVOKABLE virtual */ WBackendNetQuery getQueryItem    (const QString & url) const;

    //---------------------------------------------------------------------------------------------

    /* Q_INVOKABLE virtual */
    WBackendNetQuery createQuery(const QString & method,
                                 const QString & label,
                                 const QString & q, const QString & t = QString()) const;

    //---------------------------------------------------------------------------------------------

    /* Q_INVOKABLE virtual */
    WBackendNetSource extractSource(const QByteArray       & data,
                                    const WBackendNetQuery & query) const;

    /* Q_INVOKABLE virtual */
    WBackendNetTrack extractTrack(const QByteArray       & data,
                                  const WBackendNetQuery & query) const;

    /* Q_INVOKABLE virtual */
    WBackendNetPlaylist extractPlaylist(const QByteArray       & data,
                                        const WBackendNetQuery & query) const;

    /* Q_INVOKABLE virtual */
    WBackendNetFolder extractFolder(const QByteArray       & data,
                                    const WBackendNetQuery & query) const;

    /* Q_INVOKABLE virtual */
    WBackendNetItem extractItem(const QByteArray       & data,
                                const WBackendNetQuery & query) const;

    //---------------------------------------------------------------------------------------------

    /* Q_INVOKABLE virtual */ void queryFailed(const WBackendNetQuery & query);

    //---------------------------------------------------------------------------------------------

    /* Q_INVOKABLE virtual */ void applySource(const WBackendNetQuery  & query,
                                               const WBackendNetSource & source);

    /* Q_INVOKABLE virtual */ void applyTrack(const WBackendNetQuery & query,
                                              const WBackendNetTrack & track);

    /* Q_INVOKABLE virtual */ void applyPlaylist(const WBackendNetQuery    & query,
                                                 const WBackendNetPlaylist & playlist);

    /* Q_INVOKABLE virtual */ void applyFolder(const WBackendNetQuery  & query,
                                               const WBackendNetFolder & folder);

    /* Q_INVOKABLE virtual */ void applyItem(const WBackendNetQuery & query,
                                             const WBackendNetItem  & item);

signals:
    void loaded ();
    void updated();

    void backendUpdated(const QString & id);

private:
    W_DECLARE_PRIVATE(WBackendUniversal)

    Q_PRIVATE_SLOT(d_func(), void onLoad  ())
    Q_PRIVATE_SLOT(d_func(), void onUpdate())

    Q_PRIVATE_SLOT(d_func(), void onData(const WBackendUniversalData &))

    friend class WBackendIndex;
};

#include <private/WBackendUniversal_p>

#endif // SK_NO_BACKENDUNIVERSAL
#endif // WBACKENDUNIVERSAL_H
