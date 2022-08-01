//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkMultimedia.

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

#ifndef WFILTERBARCODE_H
#define WFILTERBARCODE_H

// Qt includes
#ifdef QT_5
#include <QAbstractVideoFilter>
#else
#include <QVideoSink>
#endif

// Sk includes
#include <Sk>

#ifndef SK_NO_FILTERBARCODE

// Forward declarations
class WFilterBarcodePrivate;

#ifdef QT_5
class SK_MULTIMEDIA_EXPORT WFilterBarcode : public QAbstractVideoFilter, public WPrivatable
#else
class SK_MULTIMEDIA_EXPORT WFilterBarcode : public QObject, public WPrivatable
#endif
{
    Q_OBJECT

#ifdef QT_6
    Q_PROPERTY(QVideoSink * videoSink READ videoSink WRITE setVideoSink NOTIFY videoSinkChanged)
#endif

    Q_PROPERTY(QRect target READ target WRITE setTarget NOTIFY targetChanged)

    Q_PROPERTY(bool interval READ interval WRITE setInterval NOTIFY intervalChanged)

public:
    explicit WFilterBarcode(QObject * parent = NULL);

public: // Static functions
    Q_INVOKABLE static QPoint mapPointToSource(const QRect & source,
                                               const QRect & content, const QPoint & point,
                                               int orientation = 0);

    Q_INVOKABLE static QRect mapRectToSource(const QRect & source,
                                             const QRect & content, const QRect & target,
                                             int orientation = 0);

#ifdef QT_5
public: // QAbstractVideoFilter implementation
    /* virtual */ QVideoFilterRunnable * createFilterRunnable();
#endif

signals:
    void loaded(const QString & text);

#ifdef QT_6
    void videoSinkChanged();
#endif

    void targetChanged();

    void intervalChanged();

public: // Properties
#ifdef QT_6
    QVideoSink * videoSink() const;
    void         setVideoSink(QVideoSink * videoSink);
#endif

    QRect target() const;
    void  setTarget(const QRect & target);

    bool interval() const;
    void setInterval(bool interval);

private:
    W_DECLARE_PRIVATE(WFilterBarcode)

#ifdef QT_6
    Q_PRIVATE_SLOT(d_func(), void onUpdated(const QVideoFrame &))
#endif

    Q_PRIVATE_SLOT(d_func(), void onLoaded(const QString &))

    friend class WFilterRunnable;
};

#include <private/WFilterBarcode_p>

#endif // SK_NO_FILTERBARCODE
#endif // WFILTERBARCODE_H
