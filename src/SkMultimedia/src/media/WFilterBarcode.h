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
#include <QAbstractVideoFilter>

// Sk includes
#include <Sk>

#ifndef SK_NO_FILTERBARCODE

class WFilterBarcodePrivate;

class SK_MULTIMEDIA_EXPORT WFilterBarcode : public QAbstractVideoFilter, public WPrivatable
{
    Q_OBJECT

    Q_ENUMS(FillMode)

    Q_PROPERTY(QRect target READ target WRITE setTarget NOTIFY targetChanged)

public: // Enums
    enum FillMode
    {
        Stretch,
        PreserveAspectFit,
        PreserveAspectCrop
    };

public:
    explicit WFilterBarcode(QObject * parent = NULL);

public: // QAbstractVideoFilter implementation
    /* virtual */ QVideoFilterRunnable * createFilterRunnable();

signals:
    void loaded(const QString & text);

    void targetChanged();

public: // Properties
    QRect target() const;
    void setTarget(const QRect & target);

private:
    W_DECLARE_PRIVATE(WFilterBarcode)

    Q_PRIVATE_SLOT(d_func(), void onLoaded(const QString &))

    friend class WFilterRunnable;
};

#include <private/WFilterBarcode_p>

#endif // SK_NO_FILTERBARCODE
#endif // WFILTERBARCODE_H
