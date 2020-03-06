//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WAbstractTab.h"

#ifndef SK_NO_ABSTRACTTAB

// Sk includes
#include <WAbstractTabs>
#include <WTabTrack>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WAbstractTab_p.h"

WAbstractTabPrivate::WAbstractTabPrivate(WAbstractTab * p) : WLocalObjectPrivate(p) {}

void WAbstractTabPrivate::init()
{
    parentTabs = NULL;

    hasFocus = false;
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WAbstractTabPrivate::setFocus(bool hasFocus)
{
    if (this->hasFocus == hasFocus) return;

    Q_Q(WAbstractTab);

    this->hasFocus = hasFocus;

    emit q->focusChanged();
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WAbstractTab::WAbstractTab(WAbstractTabs * parent)
    : WLocalObject(new WAbstractTabPrivate(this), parent)
{
    Q_D(WAbstractTab);

    d->init();

    setParentTabs(parent);
}

//-------------------------------------------------------------------------------------------------
// Protected

WAbstractTab::WAbstractTab(WAbstractTabPrivate * p, WAbstractTabs * parent)
    : WLocalObject(p, parent)
{
    Q_D(WAbstractTab);

    d->init();

    setParentTabs(parent);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

WTabTrack * WAbstractTab::toTabTrack()
{
    return qobject_cast<WTabTrack *> (this);
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

WAbstractTabs * WAbstractTab::parentTabs() const
{
    Q_D(const WAbstractTab); return d->parentTabs;
}

void WAbstractTab::setParentTabs(WAbstractTabs * parent)
{
    Q_D(WAbstractTab);

    if (d->parentTabs == parent) return;

    d->parentTabs = parent;

    setParent(parent);

    emit parentTabsChanged();
}

//-------------------------------------------------------------------------------------------------

bool WAbstractTab::isTabTrack() const
{
    return (qobject_cast<const WTabTrack *> (this) != NULL);
}

//-------------------------------------------------------------------------------------------------

bool WAbstractTab::hasFocus() const
{
    Q_D(const WAbstractTab); return d->hasFocus;
}

#endif // SK_NO_ABSTRACTTAB
