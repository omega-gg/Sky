//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WDECLARATIVEIMAGEBASE_H
#define WDECLARATIVEIMAGEBASE_H

// Sk includes
#include <WDeclarativeItem>

#ifndef SK_NO_DECLARATIVEIMAGEBASE

// Forward declarations
class WDeclarativeImageBasePrivate;
class WImageFilter;

class SK_GUI_EXPORT WDeclarativeImageBase : public WDeclarativeItem
{
    Q_OBJECT

    Q_ENUMS(Status)
    Q_ENUMS(LoadMode)
    Q_ENUMS(Asynchronous)

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

    Q_PROPERTY(bool isNull    READ isNull    NOTIFY statusChanged)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY statusChanged)
    Q_PROPERTY(bool isReady   READ isReady   NOTIFY statusChanged)

    Q_PROPERTY(bool isSourceDefault READ isSourceDefault NOTIFY defaultChanged)
    Q_PROPERTY(bool isExplicitSize  READ isExplicitSize  NOTIFY explicitSizeChanged)

    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)

    Q_PROPERTY(QUrl sourceDefault READ sourceDefault WRITE setSourceDefault
               NOTIFY sourceDefaultChanged)

    Q_PROPERTY(QSize sourceSize READ sourceSize WRITE setSourceSize RESET resetSourceSize
               NOTIFY sourceSizeChanged)

    Q_PROPERTY(QSize sourceArea READ sourceArea WRITE setSourceArea RESET resetSourceArea
               NOTIFY sourceAreaChanged)

    Q_PROPERTY(LoadMode loadMode READ loadMode WRITE setLoadMode NOTIFY loadModeChanged)

    Q_PROPERTY(Asynchronous asynchronous READ asynchronous WRITE setAsynchronous
               NOTIFY asynchronousChanged)

    Q_PROPERTY(bool cache READ cache WRITE setCache NOTIFY cacheChanged)

    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)

    Q_PROPERTY(WImageFilter * filter READ filter WRITE setFilter NOTIFY filterChanged)

    Q_PROPERTY(bool smooth READ smooth WRITE setSmooth NOTIFY smoothChanged)

    Q_PROPERTY(qreal ratioWidth  READ ratioWidth  NOTIFY sourceSizeChanged)
    Q_PROPERTY(qreal ratioHeight READ ratioHeight NOTIFY sourceSizeChanged)

public: // Enums
    enum Status { Null, Loading, Ready, Error };

    enum LoadMode { LoadAlways, LoadVisible };

    enum Asynchronous
    {
        AsynchronousOff,
        AsynchronousOn,
        AsynchronousDefault
    };

public:
    explicit WDeclarativeImageBase(QDeclarativeItem * parent = NULL);
protected:
    WDeclarativeImageBase(WDeclarativeImageBasePrivate * p, QDeclarativeItem * parent = NULL);

public: // Interface
    Q_INVOKABLE void loadSource(const QUrl & url, bool force = false);

    Q_INVOKABLE void loadNow(const QUrl & url = QUrl());

    Q_INVOKABLE void setItemShot(QGraphicsObject * object);

public: // QDeclarativeItem reimplementation
    /* virtual */ void componentComplete();

protected: // Functions
    const QPixmap & currentPixmap() const;

protected: // Virtual functions
    virtual void load();

    virtual void applyUrl(const QUrl & url, bool asynchronous);

    virtual void clearUrl(WDeclarativeImageBase::Status status);

    virtual void pixmapChange();
    virtual void pixmapClear (); /* {} */

protected slots:
    virtual void requestFinished();
    virtual void requestProgress(qint64 received, qint64 total);

protected: // QGraphicsItem reimplementation
    /* virtual */ QVariant itemChange(GraphicsItemChange change, const QVariant & value);

signals:
    void loaded();

    void statusChanged();

    void defaultChanged     ();
    void explicitSizeChanged();

    void sourceChanged       ();
    void sourceDefaultChanged();

    void sourceSizeChanged();
    void sourceAreaChanged();

    void loadModeChanged();

    void asynchronousChanged();
    void cacheChanged       ();

    void progressChanged();

    void filterChanged();
    void smoothChanged();

public: // Properties
    QPixmap pixmap() const;
    void    setPixmap(const QPixmap & pixmap);

    Status status() const;

    bool isNull   () const;
    bool isLoading() const;
    bool isReady  () const;

    bool isSourceDefault() const;
    bool isExplicitSize () const;

    QUrl source() const;
    void setSource(const QUrl & url);

    QUrl sourceDefault() const;
    void setSourceDefault(const QUrl & url);

    QSize sourceSize() const;
    void  setSourceSize(const QSize & size);
    void  resetSourceSize();

    QSize sourceArea() const;
    void  setSourceArea(const QSize & size);
    void  resetSourceArea();

    LoadMode loadMode() const;
    void     setLoadMode(LoadMode mode);

    Asynchronous asynchronous() const;
    void         setAsynchronous(Asynchronous asynchronous);

    bool cache() const;
    void setCache(bool enabled);

    qreal progress() const;

    WImageFilter * filter() const;
    void           setFilter(WImageFilter * filter);

    bool smooth() const;
    void setSmooth(bool smooth);

    qreal ratioWidth () const;
    qreal ratioHeight() const;

private:
    W_DECLARE_PRIVATE(WDeclarativeImageBase)

    Q_PRIVATE_SLOT(d_func(), void onLoaded       (WCacheFile *))
    Q_PRIVATE_SLOT(d_func(), void onFilterUpdated())
};

#include <private/WDeclarativeImageBase_p>

#endif // SK_NO_DECLARATIVEIMAGEBASE
#endif // WDECLARATIVEIMAGEBASE_H
