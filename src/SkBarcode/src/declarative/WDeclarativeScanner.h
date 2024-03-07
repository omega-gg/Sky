//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkBarcode.

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

#ifndef WDECLARATIVESCANNER_H
#define WDECLARATIVESCANNER_H

// Sk includes
#include <WDeclarativeItem>

#ifndef SK_NO_DECLARATIVESCANNER

// Forward declarations
class  WDeclarativeScannerPrivate;
class  WDeclarativePlayer;
class  WDeclarativeImage;
struct WBarcodeResult;

#ifdef QT_6
Q_MOC_INCLUDE("WDeclarativePlayer")
Q_MOC_INCLUDE("WDeclarativeImage")
#endif

class SK_BARCODE_EXPORT WDeclarativeScanner : public WDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(WDeclarativePlayer * player READ player WRITE setPlayer NOTIFY playerChanged)
    Q_PROPERTY(WDeclarativeImage  * cover  READ cover  WRITE setCover  NOTIFY coverChanged)

    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged)
    Q_PROPERTY(int duration READ duration WRITE setDuration NOTIFY durationChanged)

    Q_PROPERTY(QString text READ text NOTIFY loaded)

    Q_PROPERTY(QRectF rect READ rect NOTIFY loaded)

    Q_PROPERTY(QPointF topLeft     READ topLeft     NOTIFY loaded)
    Q_PROPERTY(QPointF topRight    READ topRight    NOTIFY loaded)
    Q_PROPERTY(QPointF bottomLeft  READ bottomLeft  NOTIFY loaded)
    Q_PROPERTY(QPointF bottomRight READ bottomRight NOTIFY loaded)

public:
#ifdef QT_4
    explicit WDeclarativeScanner(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeScanner(QQuickItem * parent = NULL);
#endif

public: // Interface
    Q_INVOKABLE bool scanFrame(int x, int y, int size = 1, int count = 1);

protected: // Events
    /* virtual */ void timerEvent(QTimerEvent * event);

signals:
    void loaded();

    void playerChanged();
    void coverChanged ();

    void intervalChanged();
    void durationChanged();

public: // Properties
    WDeclarativePlayer * player() const;
    void                 setPlayer(WDeclarativePlayer * player);

    WDeclarativeImage * cover() const;
    void                setCover(WDeclarativeImage * cover);

    int  interval() const;
    void setInterval(int interval);

    int  duration() const;
    void setDuration(int duration);

    QString text() const;

    QRectF rect() const;

    QPointF topLeft    () const;
    QPointF topRight   () const;
    QPointF bottomLeft () const;
    QPointF bottomRight() const;

private:
    W_DECLARE_PRIVATE(WDeclarativeScanner)

    Q_PRIVATE_SLOT(d_func(), void onLoaded(const WBarcodeResult &))

    Q_PRIVATE_SLOT(d_func(), void onClearPlayer())
    Q_PRIVATE_SLOT(d_func(), void onClearCover ())
};

#include <private/WDeclarativeScanner_p>

#endif // SK_NO_DECLARATIVESCANNER
#endif // WDECLARATIVESCANNER_H
