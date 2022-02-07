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

#ifndef WWINDOW_H
#define WWINDOW_H

// Sk includes
#include <WView>

#ifndef SK_NO_WINDOW

// Forward declarations
class WWindowPrivate;

class SK_GUI_EXPORT WWindow : public WView
{
    Q_OBJECT

#ifdef QT_4
    Q_PROPERTY(QDeclarativeListProperty<QObject> children READ children)
#else
    Q_PROPERTY(QQmlListProperty<QObject> children READ children)
#endif

    Q_CLASSINFO("DefaultProperty", "children")

    //---------------------------------------------------------------------------------------------

    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)

    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)

    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)

public:
#ifdef QT_4
    explicit WWindow(QWidget * parent = NULL);
#else
    explicit WWindow(QWindow * parent = NULL);
#endif

public: // Interface
    //---------------------------------------------------------------------------------------------
    // Focus

    Q_INVOKABLE bool getFocus() const;

    Q_INVOKABLE void setFocus(bool focus);

    Q_INVOKABLE void clearFocus();

#ifdef QT_4
    Q_INVOKABLE void clearFocusItem(QDeclarativeItem * item);
#else
    Q_INVOKABLE void clearFocusItem(QQuickItem * item);
#endif

public: // Static functions
#if defined(QT_NEW) && defined(Q_OS_ANDROID)
    Q_INVOKABLE static void hideSplash(int duration = 0);
#endif

protected: // Events
//#ifdef QT_4
//    /* virtual */ void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
//    /* virtual */ void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
//#else
//    /* virtual */ void hoverEnterEvent(QHoverEvent * event);
//    /* virtual */ void hoverLeaveEvent(QHoverEvent * event);

//    /* virtual */ void dragEnterEvent(QDragEnterEvent * event);
//    /* virtual */ void dragLeaveEvent(QDragLeaveEvent * event);

//    /* virtual */ void dragMoveEvent(QDragMoveEvent * event);

//    /* virtual */ void dropEvent(QDropEvent * event);
//#endif

private: // Declarative
#ifdef QT_4
    static void childrenAppend(QDeclarativeListProperty<QObject> * property, QObject * item);
    static void childrenClear (QDeclarativeListProperty<QObject> * property);

    static int childrenCount(QDeclarativeListProperty<QObject> * property);

    static QObject * childrenAt(QDeclarativeListProperty<QObject> * property, int index);
#else
    static void childrenAppend(QQmlListProperty<QObject> * property, QObject * item);
    static void childrenClear (QQmlListProperty<QObject> * property);

#ifdef QT_5
    static int childrenCount(QQmlListProperty<QObject> * property);

    static QObject * childrenAt(QQmlListProperty<QObject> * property, int index);
#else // QT_6
    static qsizetype childrenCount(QQmlListProperty<QObject> * property);

    static QObject * childrenAt(QQmlListProperty<QObject> * property, qsizetype index);
#endif
#endif

signals:
    void iconChanged();

    void visibleChanged();

    void opacityChanged();

public: // Properties
#ifdef QT_4
    QDeclarativeListProperty<QObject> children();
#else
    QQmlListProperty<QObject> children();
#endif

    QString icon() const;
    void    setIcon(const QString & icon);

    bool isVisible() const;
    void setVisible(bool visible);

    qreal opacity() const;
    void  setOpacity(qreal opacity);

    void setLocked(bool locked);

private:
    W_DECLARE_PRIVATE(WWindow)
};

#endif // SK_NO_WINDOW
#endif // WWINDOW_H
