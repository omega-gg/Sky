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

#ifndef WCONTROLLERVIEW_H
#define WCONTROLLERVIEW_H

// Qt includes
#include <QPixmap>

// Sk includes
#include <WControllerFile>

#ifndef SK_NO_CONTROLLERVIEW

// Forward declarations
#ifdef QT_4
class QGraphicsObject;
#else
class QScreen;
class QQuickItem;
#endif
class WControllerViewPrivate;

// Defines
#define wControllerView WControllerView::instance()

class SK_GUI_EXPORT WControllerView : public WController
{
    Q_OBJECT

    Q_ENUMS(LoadMode)

#ifdef QT_4
    Q_PROPERTY(bool opengl READ opengl WRITE setOpengl NOTIFY openglChanged)
#else
    Q_PROPERTY(bool opengl READ opengl CONSTANT)
#endif

    Q_PROPERTY(bool fade READ fade WRITE setFade NOTIFY fadeChanged)

    Q_PROPERTY(int fadeIn  READ fadeIn  WRITE setFadeIn  NOTIFY fadeInChanged)
    Q_PROPERTY(int fadeOut READ fadeOut WRITE setFadeOut NOTIFY fadeOutChanged)

    Q_PROPERTY(LoadMode loadMode READ loadMode WRITE setLoadMode NOTIFY loadModeChanged)

    Q_PROPERTY(int scaleDelay  READ scaleDelay  WRITE setScaleDelay NOTIFY scaleDelayChanged)
    Q_PROPERTY(int filterDelay READ filterDelay WRITE setFilterDelay NOTIFY filterDelayChanged)

public: // Enums
    enum LoadMode { LoadAlways, LoadVisible };

private:
    WControllerView();

protected: // Initialize
    /* virtual */ void init();

public: // Static functions
    Q_INVOKABLE static int screenNumber(const QPoint & pos);
#ifdef QT_4
    Q_INVOKABLE static int screenNumber(const QWidget * widget = NULL);
#endif

    Q_INVOKABLE static const QRect availableGeometry(int            index = 0);
    Q_INVOKABLE static const QRect availableGeometry(const QPoint & pos);
#ifdef QT_4
    Q_INVOKABLE static const QRect availableGeometry(const QWidget * widget);
#endif

    Q_INVOKABLE static const QRect screenGeometry(int             index = 0);
    Q_INVOKABLE static const QRect screenGeometry(const QPoint  & pos);
#ifdef QT_4
    Q_INVOKABLE static const QRect screenGeometry(const QWidget * widget);
#endif

#ifdef QT_NEW
    Q_INVOKABLE static qreal screenRatio     (QScreen * screen);
    Q_INVOKABLE static qreal screenRatioPixel(QScreen * screen);
#endif

#ifdef QT_4
    Q_INVOKABLE static QPixmap takeItemShot(QGraphicsObject * item,
                                            const QColor    & background = Qt::transparent);
#else
    Q_INVOKABLE static QPixmap takeItemShot(QQuickItem   * item,
                                            const QColor & background = Qt::transparent);
#endif

#ifdef QT_4
    Q_INVOKABLE static bool saveItemShot(const QString   & fileName,
                                         QGraphicsObject * item,
                                         const QString   & format = "png", int quality = -1,
                                         const QColor    & background = Qt::transparent);
#else
    Q_INVOKABLE static bool saveItemShot(const QString & fileName,
                                         QQuickItem    * item,
                                         const QString & format = "png", int quality = -1,
                                         const QColor  & background = Qt::transparent);
#endif

    Q_INVOKABLE static QImage desaturate(const QImage & image);

    Q_INVOKABLE static bool compressShot (const QString & fileName, int quality = 0);
    Q_INVOKABLE static bool compressShots(const QString & path,     int quality = 0);

    Q_INVOKABLE static QString getFilterImage();

    //---------------------------------------------------------------------------------------------
    // Urls

    Q_INVOKABLE static bool urlIsImage(const QString & url);

    Q_INVOKABLE static bool extensionIsImage(const QString & extension);

    //---------------------------------------------------------------------------------------------
    // Image actions

    // NOTE: These are not declared in WControllerFile because it depends on QtGui.

    WControllerFileReply * startWriteImage(const QString & fileName,
                                           const QImage  & image,
                                           const QString & format  = "png",
                                           int             quality = -1);

    WControllerFileReply * startWriteImages(const QStringList   & fileNames,
                                            const QList<QImage> & images,
                                            const QString       & format = "png",
                                            int                   quality = -1);

signals:
#ifdef QT_4
    void openglChanged();
#endif

    void fadeChanged();

    void fadeInChanged ();
    void fadeOutChanged();

    void loadModeChanged();

    void scaleDelayChanged ();
    void filterDelayChanged();

public: // Properties
    bool opengl() const;
#ifdef QT_4
    void setOpengl(bool enabled);
#endif

    bool fade() const;
    void setFade(bool enabled);

    int  fadeIn() const;
    void setFadeIn(int msec);

    int  fadeOut() const;
    void setFadeOut(int msec);

    LoadMode loadMode() const;
    void     setLoadMode(LoadMode mode);

    int  scaleDelay() const;
    void setScaleDelay(int delay);

    int  filterDelay() const;
    void setFilterDelay(int delay);

private:
    W_DECLARE_PRIVATE   (WControllerView)
    W_DECLARE_CONTROLLER(WControllerView)

    friend class WView;
    friend class WViewPrivate;
};

#endif // SK_NO_CONTROLLERVIEW
#endif // WCONTROLLERVIEW_H
