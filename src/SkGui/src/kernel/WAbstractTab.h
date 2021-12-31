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

#ifndef WABSTRACTTAB_H
#define WABSTRACTTAB_H

// Sk includes
#include <WLocalObject>

#ifndef SK_NO_ABSTRACTTAB

// Forward declarations
class WAbstractTabPrivate;
class WAbstractTabs;
class WTabTrack;

#ifdef QT_6
Q_MOC_INCLUDE("WAbstractTabs")
#endif

class SK_GUI_EXPORT WAbstractTab : public WLocalObject
{
    Q_OBJECT

    Q_PROPERTY(WAbstractTabs * parentTabs READ parentTabs WRITE setParentTabs
               NOTIFY parentTabsChanged)

    Q_PROPERTY(bool isTabTrack READ isTabTrack CONSTANT)

    Q_PROPERTY(bool hasFocus READ hasFocus NOTIFY focusChanged)

public:
    explicit WAbstractTab(WAbstractTabs * parent = NULL);
protected:
    WAbstractTab(WAbstractTabPrivate * p, WAbstractTabs * parent = NULL);

public: // Interface
    Q_INVOKABLE WTabTrack * toTabTrack();

signals:
    void parentTabsChanged();

    void focusChanged();

public: // Properties
    WAbstractTabs * parentTabs() const;
    void            setParentTabs(WAbstractTabs * parent);

    bool isTabTrack() const;

    bool hasFocus() const;

private:
    W_DECLARE_PRIVATE(WAbstractTab)

    friend class WAbstractTabs;
    friend class WAbstractTabsPrivate;
};

#endif // SK_NO_ABSTRACTTAB
#endif // WABSTRACTTAB_H
