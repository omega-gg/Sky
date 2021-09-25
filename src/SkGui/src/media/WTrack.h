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
    enum Type { Unknown, Media, Live };

    enum State { Default, Loading, Loaded, Cover };

public:
    explicit WTrack(const QString & source = QString(), State state = Loaded);

public: // Interface
    bool isValid() const;

    void copyDataTo (WTrack * other) const;
    void applyDataTo(WTrack * other) const;

public: // Static functions
    static Type typeFromString(const QString & string);

    static QString typeToString(Type type);

public: // Virtual interface
    virtual QVariantMap toMap() const;

    virtual QString toVbml() const;

public: // Operators
    WTrack(const WTrack & other);

    bool operator==(const WTrack & other) const;

    WTrack & operator=(const WTrack & other);

public: // Properties
    int id() const;

    Type type() const;
    void setType(Type type);

    State state() const;
    void  setState(State state);

    bool isDefault() const;
    bool isLoading() const;
    bool isLoaded () const;

    QString source() const;
    void    setSource(const QString & url);

    QString title() const;
    void    setTitle(const QString & title);

    QString cover() const;
    void    setCover(const QString & cover);

    QString author() const;
    void    setAuthor(const QString & author);

    QString feed() const;
    void    setFeed(const QString & feed);

    int  duration() const;
    void setDuration(int msec);

    QDateTime date() const;
    void      setDate(const QDateTime & date);

    WPlaylist * playlist() const;

private:
    W_DECLARE_PRIVATE_COPY(WTrack)

    friend class WPlaylist;
    friend class WPlaylistPrivate;
    friend class WPlaylistRead;
};

#endif // SK_NO_TRACK
#endif // WTRACK_H
