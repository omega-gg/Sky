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

#ifndef WCONTROLLERVIEW_H
#define WCONTROLLERVIEW_H

// Qt includes
#include <QPixmap>

// Sk includes
#include <WController>

#ifndef SK_NO_CONTROLLERVIEW

// Forward declarations
class QGraphicsObject;
class WControllerViewPrivate;

// Defines
#define wControllerView WControllerView::instance()

class SK_GUI_EXPORT WControllerView : public WController
{
    Q_OBJECT

    Q_ENUMS(LoadMode)

    Q_PROPERTY(LoadMode loadMode READ loadMode WRITE setLoadMode NOTIFY loadModeChanged)

    Q_PROPERTY(int scaleDelay READ scaleDelay WRITE setScaleDelay NOTIFY scaleDelayChanged)

public: // Enums
    enum LoadMode { LoadAlways, LoadVisible };

private:
    WControllerView();

protected: // Initialize
    /* virtual */ void init();

public: // Interface
    Q_INVOKABLE int screenNumber(const QWidget * widget = NULL) const;
    Q_INVOKABLE int screenNumber(const QPoint  & pos)           const;

    Q_INVOKABLE const QRect availableGeometry(int             screen = -1) const;
    Q_INVOKABLE const QRect availableGeometry(const QWidget * widget)      const;
    Q_INVOKABLE const QRect availableGeometry(const QPoint  & pos)         const;

    Q_INVOKABLE const QRect screenGeometry(int             screen = -1) const;
    Q_INVOKABLE const QRect screenGeometry(const QWidget * widget)      const;
    Q_INVOKABLE const QRect screenGeometry(const QPoint  & pos)         const;

    Q_INVOKABLE QPixmap takeItemShot(QGraphicsObject  * item,
                                     bool               recursive    = true,
                                     const QColor     & background   = Qt::transparent,
                                     bool               forceVisible = false) const;

    Q_INVOKABLE bool saveItemShot(const QString    & fileName,
                                  QGraphicsObject  * item,
                                  bool               recursive    = true,
                                  const QColor     & background   = Qt::transparent,
                                  bool               forceVisible = false) const;

public: // Static interface
    Q_INVOKABLE static QImage desaturate(const QImage & image);

signals:
    void loadModeChanged();

    void scaleDelayChanged();

public: // Properties
    LoadMode loadMode() const;
    void     setLoadMode(LoadMode mode);

    int  scaleDelay() const;
    void setScaleDelay(int delay);

private:
    W_DECLARE_PRIVATE   (WControllerView)
    W_DECLARE_CONTROLLER(WControllerView)

    friend class WView;
    friend class WViewPrivate;
};

#include <private/WControllerView_p>

#endif // SK_NO_CONTROLLERVIEW
#endif // WCONTROLLERVIEW_H
