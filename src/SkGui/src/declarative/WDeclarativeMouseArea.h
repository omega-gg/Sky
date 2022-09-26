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

#ifndef WDECLARATIVEMOUSEAREA_H
#define WDECLARATIVEMOUSEAREA_H

// Sk includes
#include <Sk>

// Qt private includes
#ifdef QT_4
#include <private/qdeclarativemousearea_p.h>
#include <private/qdeclarativeevents_p_p.h>
#else
#include <private/qquickmousearea_p.h>
#include <private/qquickevents_p_p.h>
#endif

#ifndef SK_NO_DECLARATIVEMOUSEAREA

// Forward declarations
class WDeclarativeMouseAreaPrivate;
class WView;
class WDeclarativeDropEvent;

#ifdef QT_6
Q_MOC_INCLUDE("WView")
#endif

#ifdef QT_4
class SK_GUI_EXPORT WDeclarativeMouseArea : public QDeclarativeMouseArea, public WPrivatable
#else
class SK_GUI_EXPORT WDeclarativeMouseArea : public QQuickMouseArea, public WPrivatable
#endif
{
    Q_OBJECT

    Q_PROPERTY(WView * view READ view NOTIFY viewChanged)

    // NOTE: This 'hoverActive' implementation is our custom 'containsMouse' implementation.
    Q_PROPERTY(bool hoverActive READ hoverActive NOTIFY hoverActiveChanged)

    Q_PROPERTY(bool hoverRetain READ hoverRetain WRITE setHoverRetain NOTIFY hoverRetainChanged)

    Q_PROPERTY(bool wheelEnabled READ wheelEnabled WRITE setWheelEnabled
               NOTIFY wheelEnabledChanged)

    Q_PROPERTY(bool dropEnabled READ dropEnabled WRITE setDropEnabled NOTIFY dropEnabledChanged)

    Q_PROPERTY(bool dragAccepted READ dragAccepted NOTIFY dragAcceptedChanged)

    Q_PROPERTY(Qt::CursorShape cursor READ cursor WRITE setCursor NOTIFY cursorChanged)

    Q_PROPERTY(Qt::CursorShape cursorDrop READ cursorDrop WRITE setCursorDrop
               NOTIFY cursorDropChanged)

public:
#ifdef QT_4
    explicit WDeclarativeMouseArea(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeMouseArea(QQuickItem * parent = NULL);
#endif

protected:
#ifdef QT_4
    WDeclarativeMouseArea(WDeclarativeMouseAreaPrivate * p, QDeclarativeItem * parent = NULL);
#else
    WDeclarativeMouseArea(WDeclarativeMouseAreaPrivate * p, QQuickItem * parent = NULL);
#endif

public: // Interface
    Q_INVOKABLE void press  (Qt::MouseButton button = Qt::LeftButton);
    Q_INVOKABLE void release(Qt::MouseButton button = Qt::LeftButton);

    Q_INVOKABLE void click(Qt::MouseButton button = Qt::LeftButton);

    //---------------------------------------------------------------------------------------------
    // QML

    Q_INVOKABLE void press  (int button);
    Q_INVOKABLE void release(int button);

    Q_INVOKABLE void click(int button);

protected: // QGraphicsItem / QQuickItem reimplementation
#ifdef QT_4
    /* virtual */ QVariant itemChange(GraphicsItemChange change, const QVariant & value);
#else
    /* virtual */ void itemChange(ItemChange change, const ItemChangeData & value);
#endif

protected: // Events
#ifdef QT_4
    /* virtual */ void wheelEvent(QGraphicsSceneWheelEvent * event);
#else
    /* virtual */ void mousePressEvent  (QMouseEvent * event);
    /* virtual */ void mouseReleaseEvent(QMouseEvent * event);

    /* virtual */ void mouseDoubleClickEvent(QMouseEvent * event);

    /* virtual */ void touchEvent(QTouchEvent * event);

    /* virtual */ void touchUngrabEvent();

    /* virtual */ void wheelEvent(QWheelEvent * event);
#endif

signals:
    void viewChanged();

    void hoverActiveChanged();
    void hoverRetainChanged();

    void wheelEnabledChanged();

    void dropEnabledChanged();

    void dragAcceptedChanged();

    void cursorChanged    ();
    void cursorDropChanged();

    // NOTE: These custom 'entered / exited' signals are tied to 'hoverActive'.
    void hoverEntered();
    void hoverExited ();

    void dragEntered(WDeclarativeDropEvent * event);
    void dragExited (WDeclarativeDropEvent * event);

    void dragMove(WDeclarativeDropEvent * event);

    void drop(WDeclarativeDropEvent * event);

    void wheeled(qreal steps);

public: // Properties
    WView * view() const;

    bool hoverActive() const;

    bool hoverRetain() const;
    void setHoverRetain(bool retain);

    bool wheelEnabled() const;
    void setWheelEnabled(bool enabled);

    bool dropEnabled() const;
    void setDropEnabled(bool enabled);

    bool dragAccepted() const;

    Qt::CursorShape cursor() const;
    void            setCursor(Qt::CursorShape shape);

    Qt::CursorShape cursorDrop() const;
    void            setCursorDrop(Qt::CursorShape shape);

private:
    W_DECLARE_PRIVATE(WDeclarativeMouseArea)

    Q_PRIVATE_SLOT(d_func(), void onHoverEnabledChanged())

    friend class WView;
    friend class WViewPrivate;
};

#include <private/WDeclarativeMouseArea_p>

#endif // SK_NO_DECLARATIVEMOUSEAREA
#endif // WDECLARATIVEMOUSEAREA_H
