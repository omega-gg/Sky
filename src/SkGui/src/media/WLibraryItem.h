//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkGui.

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

#ifndef WLIBRARYITEM_H
#define WLIBRARYITEM_H

// Sk includes
#include <WLocalObject>

#ifndef SK_NO_LIBRARYITEM

// Forward declarations
class WLibraryItemPrivate;
class WLibraryFolder;
class WPlaylist;
class WTabTrack;
class WBackendNetQuery;

class SK_GUI_EXPORT WLibraryItem : public WLocalObject
{
    Q_OBJECT

    Q_ENUMS(Type)

    Q_PROPERTY(QList<int> idFull READ idFull NOTIFY idFullChanged)

    Q_PROPERTY(WLibraryFolder * parentFolder READ parentFolder WRITE setParentFolder
               NOTIFY parentFolderChanged)

    Q_PROPERTY(Type type READ type CONSTANT)

    Q_PROPERTY(bool isFolder   READ isFolder   CONSTANT)
    Q_PROPERTY(bool isPlaylist READ isPlaylist CONSTANT)

    Q_PROPERTY(bool isFolderBase       READ isFolderBase       CONSTANT)
    Q_PROPERTY(bool isFolderSearch     READ isFolderSearch     CONSTANT)
    Q_PROPERTY(bool isFolderSearchable READ isFolderSearchable CONSTANT)
    Q_PROPERTY(bool isFolderRelated    READ isFolderRelated    CONSTANT)

    Q_PROPERTY(bool isPlaylistBase   READ isPlaylistBase   CONSTANT)
    Q_PROPERTY(bool isPlaylistFeed   READ isPlaylistFeed   CONSTANT)
    Q_PROPERTY(bool isPlaylistSearch READ isPlaylistSearch CONSTANT)

    Q_PROPERTY(State stateQuery READ stateQuery NOTIFY stateQueryChanged)

    Q_PROPERTY(bool queryIsDefault READ queryIsDefault NOTIFY stateQueryChanged)
    Q_PROPERTY(bool queryIsLoading READ queryIsLoading NOTIFY stateQueryChanged)
    Q_PROPERTY(bool queryIsLoaded  READ queryIsLoaded  NOTIFY stateQueryChanged)

    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)

    Q_PROPERTY(bool isLocal  READ isLocal  NOTIFY sourceChanged)
    Q_PROPERTY(bool isOnline READ isOnline NOTIFY sourceChanged)

    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString cover READ cover WRITE setCover NOTIFY coverChanged)

    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)

public: // Enums
    enum Type
    {
        Item             = 0x00,
        Folder           = 0x01,
        FolderSearch     = 0x02,
        FolderSearchable = 0x04,
        FolderRelated    = 0x08,
        Playlist         = 0x10,
        PlaylistFeed     = 0x20,
        PlaylistSearch   = 0x40,
        UserType         = 0x10000
    };
    Q_DECLARE_FLAGS(Types, Type)

public:
    /* explicit */ WLibraryItem(WLibraryFolder * parent = NULL);
protected:
    WLibraryItem(WLibraryItemPrivate * p, Type type, WLibraryFolder * parent = NULL);

public: // Interface
    Q_INVOKABLE WLibraryFolder * toFolder  ();
    Q_INVOKABLE WPlaylist      * toPlaylist();

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE bool loadSource  (const QString & source, bool load = true);
    Q_INVOKABLE bool reloadSource(const QString & source);

    Q_INVOKABLE void clearSource();

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE bool loadQuery  ();
    Q_INVOKABLE bool reloadQuery();

    Q_INVOKABLE bool abortQuery();

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE void setFutureId(int id);

    Q_INVOKABLE void setCurrentIds(const QList<int> & ids);

    Q_INVOKABLE void setCurrentTabIds(WTabTrack * tab);

public: // Static functions
    Q_INVOKABLE static bool typeIsFolder  (Type type);
    Q_INVOKABLE static bool typeIsPlaylist(Type type);

    Q_INVOKABLE static QString typeToString(Type type);

public: // Virtual interface
    Q_INVOKABLE virtual QString toVbml() const; // {}

public: // WLocalObject reimplementation
    /* Q_INVOKABLE virtual */ QString getParentPath() const;

protected: // Virtual functions
    virtual bool applySource(const QString          & source);
    virtual bool applyQuery (const WBackendNetQuery & query);

    virtual bool stopQuery();

    virtual void onApplyCurrentIds(const QList<int> & ids); // {}

protected: // WLocalObject reimplementation
    /* virtual */ void applyId(int id);

    /* virtual */ void setLoaded(bool ok);

    /* virtual */ void onStateChanged(WLocalObject::State state);
    /* virtual */ void onLockChanged (bool                locked);

    /* virtual */ void onFileDeleted();

signals:
    void queryData(const QByteArray & data, const QString & extension);

    void queryStarted();
    void queryEnded  ();

    void queryCompleted();

    void idFullChanged();

    void parentFolderChanged();

    void stateQueryChanged();

    void sourceChanged();

    void titleChanged();
    void coverChanged();

    void labelChanged();

public: // Properties
    QList<int> idFull() const;

    WLibraryFolder * parentFolder() const;
    void             setParentFolder(WLibraryFolder * folder);

    Type type() const;

    bool isFolder  () const;
    bool isPlaylist() const;

    bool isFolderBase      () const;
    bool isFolderSearch    () const;
    bool isFolderSearchable() const;
    bool isFolderRelated   () const;

    bool isPlaylistBase  () const;
    bool isPlaylistFeed  () const;
    bool isPlaylistSearch() const;

    State stateQuery() const;

    bool queryIsDefault() const;
    bool queryIsLoading() const;
    bool queryIsLoaded () const;

    QString source() const;
    void    setSource(const QString & source);

    bool isLocal () const;
    bool isOnline() const;

    QString title() const;
    void    setTitle(const QString & title);

    QString cover() const;
    void    setCover(const QString & cover);

    QString label() const;
    void    setLabel(const QString & label);

private:
    W_DECLARE_PRIVATE(WLibraryItem)

    friend class WControllerPlaylist;
    friend class WControllerPlaylistPrivate;
    friend class WLibraryFolder;
    friend class WLibraryFolderPrivate;
    friend class WBackendNet;
    friend class WBackendNetPrivate;
};

#endif // SK_NO_LIBRARYITEM
#endif // WLIBRARYITEM_H
