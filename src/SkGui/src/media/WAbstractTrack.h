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

#ifndef WABSTRACTTRACK_H
#define WABSTRACTTRACK_H

// Qt includes
#include <QVariant>

// Sk includes
#include <WAbstractBackend>

#ifndef SK_NO_ABSTRACTTRACK

// Forward declarations
class QDateTime;
class WAbstractTrackPrivate;
class WAbstractPlaylist;

class SK_GUI_EXPORT WAbstractTrack : public WPrivatable
{
public: // Enums
    enum State { Default, Loading, Loaded };

protected:
    WAbstractTrack(WAbstractTrackPrivate * p, State state = Loaded);

public: // Interface
    bool isValid() const;

    void copyDataTo (WAbstractTrack * other) const;
    void applyDataTo(WAbstractTrack * other) const;

public: // Virtual interface
    virtual QVariantMap toMap() const;

public: // Operators
    bool operator==(const WAbstractTrack & other) const;

    WAbstractTrack & operator=(const WAbstractTrack & other);

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

    WAbstractPlaylist * playlist() const;

private:
    W_DECLARE_PRIVATE_COPY(WAbstractTrack)
};

#endif // SK_NO_ABSTRACTTRACK
#endif // WABSTRACTTRACK_H
