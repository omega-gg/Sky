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

#ifndef WDECLARATIVEBARCODE_H
#define WDECLARATIVEBARCODE_H

// Sk includes
#include <WDeclarativeImage>
#include <WBarcodeWriter>

#ifndef SK_NO_DECLARATIVEBARCODE

class WDeclarativeBarcodePrivate;

class SK_BARCODE_EXPORT WDeclarativeBarcode : public WDeclarativeImage
{
    Q_OBJECT

    Q_PROPERTY(QString text   READ text   WRITE setText   NOTIFY textChanged)
    Q_PROPERTY(QString prefix READ prefix WRITE setPrefix NOTIFY prefixChanged)

    // NOTE Qt4: A virtual 'type' function already exists in the QGraphicsItem.
    Q_PROPERTY(WBarcodeWriter::Type type READ barcodeType WRITE setBarcodeType NOTIFY typeChanged)

    Q_PROPERTY(int margins READ margins WRITE setMargins NOTIFY marginsChanged)

public:
#ifdef QT_4
    explicit WDeclarativeBarcode(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeBarcode(QQuickItem * parent = NULL);
#endif

public: // QDeclarativeItem / QQuickItem reimplementation
    /* virtual */ void componentComplete();

protected: // QGraphicsItem / QQuickItem reimplementation
#ifdef QT_4
    /* virtual */ QVariant itemChange(GraphicsItemChange change, const QVariant & value);
#else
    /* virtual */ void itemChange(ItemChange change, const ItemChangeData & value);
#endif

signals:
    void textChanged  ();
    void prefixChanged();

    void typeChanged();

    void marginsChanged();

public: // Properties
    QString text() const;
    void    setText(const QString & text);

    QString prefix() const;
    void    setPrefix(const QString & prefix);

    WBarcodeWriter::Type barcodeType() const;
    void                 setBarcodeType(WBarcodeWriter::Type type);

    int  margins() const;
    void setMargins(int margins);

private:
    W_DECLARE_PRIVATE(WDeclarativeBarcode)

    Q_PRIVATE_SLOT(d_func(), void onLoadModeChanged())
};

#include <private/WDeclarativeBarcode_p>

#endif // SK_NO_DECLARATIVEBARCODE
#endif // WDECLARATIVEBARCODE_H
