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

#ifndef WABSTRACTVIEW_H
#define WABSTRACTVIEW_H

// Qt includes
#ifdef QT_4
#include <QDeclarativeView>
#else
#include <QQuickWindow>
#endif

// Sk includes
#include <Sk>

#ifndef SK_NO_ABSTRACTVIEW

class WAbstractViewPrivate;

#ifdef QT_4
class SK_GUI_EXPORT WAbstractView : public QDeclarativeView, public WPrivatable
#else
class SK_GUI_EXPORT WAbstractView : public QQuickWindow, public WPrivatable
#endif
{
    Q_OBJECT

public:
#ifdef QT_4
    WAbstractView(QWidget * parent = NULL, Qt::WindowFlags flags = Qt::Widget);
#else
    WAbstractView(QWindow * parent = NULL, Qt::WindowFlags flags = Qt::Widget);
#endif
protected:
#ifdef QT_4
    WAbstractView(WAbstractViewPrivate * p,
                  QWidget * parent = NULL, Qt::WindowFlags flags = Qt::Widget);
#else
    WAbstractView(WAbstractViewPrivate * p,
                  QWindow * parent = NULL, Qt::WindowFlags flags = Qt::Widget);
#endif

#ifdef SK_WIN_NATIVE
public: // Interface
    Q_INVOKABLE void show();
    Q_INVOKABLE void hide();

    Q_INVOKABLE void showNormal    ();
    Q_INVOKABLE void showMaximized ();
    Q_INVOKABLE void showFullScreen();

    Q_INVOKABLE void showMinimized();

    Q_INVOKABLE void raise();
    Q_INVOKABLE void lower();

#ifdef QT_4
    Q_INVOKABLE void move(int x, int y);
    Q_INVOKABLE void move(const QPoint & position);
#else
    Q_INVOKABLE void setPosition(int x, int y);
    Q_INVOKABLE void setPosition(const QPoint & position);
#endif

    Q_INVOKABLE void resize(int width, int height);
    Q_INVOKABLE void resize(const QSize & size);

    Q_INVOKABLE void setGeometry(int x, int y, int width, int height);
    Q_INVOKABLE void setGeometry(const QRect & rect);

    Q_INVOKABLE void setMinimumSize(int width, int height);
    Q_INVOKABLE void setMaximumSize(int width, int height);

    Q_INVOKABLE void setMinimumSize(const QSize & size);
    Q_INVOKABLE void setMaximumSize(const QSize & size);

    Q_INVOKABLE void setMinimumWidth (int width);
    Q_INVOKABLE void setMinimumHeight(int height);

    Q_INVOKABLE void setMaximumWidth (int width);
    Q_INVOKABLE void setMaximumHeight(int height);

    Q_INVOKABLE void setVisible(bool visible);

    Q_INVOKABLE void setFocus();

#ifdef QT_4
    Q_INVOKABLE void setWindowIcon (const QIcon   & icon);
    Q_INVOKABLE void setWindowTitle(const QString & title);
#else
    Q_INVOKABLE void setIcon (const QIcon   & icon);
    Q_INVOKABLE void setTitle(const QString & title);
#endif
#endif

#ifdef Q_OS_WIN
    Q_INVOKABLE void setWindowSnap    (bool enabled);
    Q_INVOKABLE void setWindowMaximize(bool enabled);
    Q_INVOKABLE void setWindowClip    (bool enabled);
#endif

#ifdef SK_WIN_NATIVE
public: // Virtual interface
    Q_INVOKABLE virtual bool close();

protected: // Events
#ifdef QT_4
    /* virtual */ bool winEvent(MSG * msg, long * result);
#elif defined(QT_5)
    /* virtual */ bool nativeEvent(const QByteArray & event, void * msg, long * result);
#else
    /* virtual */ bool nativeEvent(const QByteArray & event, void * msg, qintptr * result);
#endif

protected: // Virtual functions
    virtual void onStateChanged(Qt::WindowState state); // {}

    virtual void onClose();

public: // Properties
    WId winId() const;

#ifdef QT_LATEST
    QScreen * screen() const;
#endif

    QRect geometry() const;

    int x() const;
    int y() const;

    int width () const;
    int height() const;

    int minimumWidth () const;
    int minimumHeight() const;

    int maximumWidth () const;
    int maximumHeight() const;

#ifdef QT_4
    qreal windowOpacity() const;
    void  setWindowOpacity(qreal level);
#else
    qreal opacity() const;
    void  setOpacity(qreal level);
#endif
#endif

private:
    W_DECLARE_PRIVATE(WAbstractView)

#ifdef SK_WIN_NATIVE
#ifdef QT_LATEST
    Q_PRIVATE_SLOT(d_func(), void onCreate())

    Q_PRIVATE_SLOT(d_func(), void onMove())
#endif

    Q_PRIVATE_SLOT(d_func(), void onFocus())
#endif
};

#include <private/WAbstractView_p>

#endif // SK_NO_ABSTRACTVIEW
#endif // WABSTRACTVIEW_H
