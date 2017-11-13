//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WTRACK_H
#define WTRACK_H

// Qt includes
#include <QVariant>

// Sk includes
#include <WAbstractBackend>

#ifndef SK_NO_TRACK

// Forward declarations
class WTrackPrivate;
class WPlaylist;

class SK_GUI_EXPORT WTrack : public WPrivatable
{
public: // Enums
    enum State { Default, Loading, Loaded };

public:
    explicit WTrack(const QUrl & source = QUrl(), State state = Loaded);

public: // Interface
    bool isValid() const;

    void copyDataTo (WTrack * other) const;
    void applyDataTo(WTrack * other) const;

public: // Virtual interface
    virtual QVariantMap toMap() const;

public: // Operators
    WTrack(const WTrack & other);

    bool operator==(const WTrack & other) const;

    WTrack & operator=(const WTrack & other);

public: // Properties
    int id() const;

    State state() const;
    void  setState(State state);

    bool isDefault() const;
    bool isLoading() const;
    bool isLoaded () const;

    QUrl source() const;
    void setSource(const QUrl & url);

    QString title() const;
    void    setTitle(const QString & title);

    QUrl cover() const;
    void setCover(const QUrl & cover);

    QString author() const;
    void    setAuthor(const QString & author);

    QString feed() const;
    void    setFeed(const QString & feed);

    int  duration() const;
    void setDuration(int msec);

    QDateTime date() const;
    void      setDate(const QDateTime & date);

    WAbstractBackend::Quality quality() const;
    void                      setQuality(WAbstractBackend::Quality quality);

    WPlaylist * playlist() const;

private:
    W_DECLARE_PRIVATE_COPY(WTrack)

    friend class WPlaylist;
    friend class WPlaylistPrivate;
    friend class WPlaylistRead;
};

#endif // SK_NO_TRACK
#endif // WTRACK_H
