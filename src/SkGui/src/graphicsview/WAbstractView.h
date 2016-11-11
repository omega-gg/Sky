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

#ifndef WABSTRACTVIEW_H
#define WABSTRACTVIEW_H

// Qt includes
#include <QDeclarativeView>

// Sk includes
#include <Sk>

#ifndef SK_NO_ABSTRACTVIEW

class WAbstractViewPrivate;

class SK_GUI_EXPORT WAbstractView : public QDeclarativeView, public WPrivatable
{
    Q_OBJECT

public:
    WAbstractView(QWidget * parent = NULL, Qt::WindowFlags flags = 0);
protected:
    WAbstractView(WAbstractViewPrivate * p, QWidget * parent = NULL, Qt::WindowFlags flags = 0);

#ifdef Q_OS_WIN
public: // Interface
    Q_INVOKABLE void showNormal    ();
    Q_INVOKABLE void showMaximized ();
    Q_INVOKABLE void showFullScreen();

    Q_INVOKABLE void showMinimized();

    Q_INVOKABLE void raise();
    Q_INVOKABLE void lower();

    Q_INVOKABLE void setGeometry(int x, int y, int width, int height);
    Q_INVOKABLE void setGeometry(const QRect & rect);

    Q_INVOKABLE void move(int x, int y);
    Q_INVOKABLE void move(const QPoint & position);

    Q_INVOKABLE void resize(int width, int height);
    Q_INVOKABLE void resize(const QSize & size);

protected: // Events
    /* virtual */ void showEvent(QShowEvent * event);
    /* virtual */ void hideEvent(QHideEvent * event);

    /* virtual */ void resizeEvent(QResizeEvent * event);

    /* virtual */ void focusInEvent(QFocusEvent * event);

#ifdef QT_LATEST
    /* virtual */ bool nativeEvent(const QByteArray & type, void * message, long * result);
#else
    /* virtual */ bool winEvent(MSG * message, long * result);
#endif

public: // Properties
    WId winId() const;

    int x() const;
    int y() const;

    int width () const;
    int height() const;
#endif

private:
    W_DECLARE_PRIVATE(WAbstractView)
};

#endif // SK_NO_ABSTRACTVIEW
#endif // WABSTRACTVIEW_H
