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

#include "WDeclarativeBarcode.h"

#ifndef SK_NO_DECLARATIVEBARCODE

// Sk includes
#include <WBarcodeWriter>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

WDeclarativeBarcodePrivate::WDeclarativeBarcodePrivate(WDeclarativeBarcode * p)
    : WDeclarativeImagePrivate(p) {}

void WDeclarativeBarcodePrivate::init()
{
    Q_Q(WDeclarativeBarcode);

    type = WDeclarativeBarcode::Text;

    margins = 1;

    loadLater = false;

    QObject::connect(q, SIGNAL(loadModeChanged()), q, SLOT(onLoadModeChanged()));
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WDeclarativeBarcodePrivate::load()
{
    if (loadMode != WDeclarativeImageBase::LoadVisible)
    {
        loadText();

        return;
    }

    Q_Q(WDeclarativeBarcode);

    if (q->isVisible())
    {
        loadText();
    }
    else loadLater = true;
}

void WDeclarativeBarcodePrivate::reload()
{
    Q_Q(WDeclarativeBarcode);

    if (q->isComponentComplete()) load();
}

void WDeclarativeBarcodePrivate::loadVisible()
{
    if (loadLater == false) return;

    loadLater = false;

    loadText();
}

void WDeclarativeBarcodePrivate::loadText()
{
    Q_Q(WDeclarativeBarcode);

    q->applyImage(WBarcodeWriter::write(text, static_cast<WBarcodeWriter::Type> (type), prefix,
                                        margins));
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WDeclarativeBarcodePrivate::onLoadModeChanged()
{
    if (loadMode == WDeclarativeImageBase::LoadVisible) return;

    loadVisible();
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* explicit */ WDeclarativeBarcode::WDeclarativeBarcode(QDeclarativeItem * parent)
#else
/* explicit */ WDeclarativeBarcode::WDeclarativeBarcode(QQuickItem * parent)
#endif
    : WDeclarativeImage(new WDeclarativeBarcodePrivate(this), parent)
{
    Q_D(WDeclarativeBarcode); d->init();
}

//-------------------------------------------------------------------------------------------------
// QDeclarativeItem / QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

/* virtual */ void WDeclarativeBarcode::componentComplete()
{
    Q_D(WDeclarativeBarcode);

    WDeclarativeImage::componentComplete();

    if (d->text.isEmpty() == false) d->load();
}

//-------------------------------------------------------------------------------------------------
// Protected QGraphicsItem / QQuickItem reimplementation
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* virtual */ QVariant WDeclarativeBarcode::itemChange(GraphicsItemChange change,
                                                       const QVariant &   value)
#else
/* virtual */ void WDeclarativeBarcode::itemChange(ItemChange             change,
                                                   const ItemChangeData & value)
#endif
{
#ifdef QT_4
    if (change == ItemVisibleHasChanged)
    {
        Q_D(WDeclarativeImageBase);

#else
    Q_D(WDeclarativeImageBase);

    if (d->view && change == ItemVisibleHasChanged && value.boolValue)
    {
#endif
        d->loadVisible();
    }

#ifdef QT_4
    return WDeclarativeImage::itemChange(change, value);
#else
    WDeclarativeImage::itemChange(change, value);
#endif
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

QString WDeclarativeBarcode::text() const
{
    Q_D(const WDeclarativeBarcode); return d->text;
}

void WDeclarativeBarcode::setText(const QString & text)
{
    Q_D(WDeclarativeBarcode);

    if (d->text == text) return;

    d->text = text;

    if (isComponentComplete()) d->load();

    emit textChanged();
}

QString WDeclarativeBarcode::prefix() const
{
    Q_D(const WDeclarativeBarcode); return d->prefix;
}

void WDeclarativeBarcode::setPrefix(const QString & prefix)
{
    Q_D(WDeclarativeBarcode);

    if (d->prefix == prefix) return;

    d->prefix = prefix;

    d->reload();

    emit prefixChanged();
}

WDeclarativeBarcode::Type WDeclarativeBarcode::barcodeType() const
{
    Q_D(const WDeclarativeBarcode); return d->type;
}

void WDeclarativeBarcode::setBarcodeType(Type type)
{
    Q_D(WDeclarativeBarcode);

    if (d->type == type) return;

    d->type = type;

    d->reload();

    emit typeChanged();
}

int WDeclarativeBarcode::margins() const
{
    Q_D(const WDeclarativeBarcode); return d->margins;
}

void WDeclarativeBarcode::setMargins(int margins)
{
    Q_D(WDeclarativeBarcode);

    if (d->margins == margins) return;

    d->margins = margins;

    d->reload();

    emit marginsChanged();
}

#endif // SK_NO_DECLARATIVEBARCODE
