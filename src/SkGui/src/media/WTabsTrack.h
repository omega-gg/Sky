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

#ifndef WTABSTRACK_H
#define WTABSTRACK_H

// Sk includes
#include <WAbstractTabs>

#ifndef SK_NO_TABSTRACK

// Forward declarations
class WTabsTrackPrivate;

class SK_GUI_EXPORT WTabsTrack : public WAbstractTabs
{
    Q_OBJECT

    Q_PROPERTY(WTabTrack * highlightedTab READ highlightedTab WRITE setHighlightedTab
               NOTIFY highlightedTabChanged)

    Q_PROPERTY(int highlightedIndex READ highlightedIndex WRITE setHighlightedIndex
               NOTIFY highlightedIndexChanged)

public:
    explicit WTabsTrack(QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE void closeTabs();

    Q_INVOKABLE void closeOtherTabs(WAbstractTab * tab);

    Q_INVOKABLE WTabTrack * currentTabTrack() const;

    Q_INVOKABLE WTabTrack * tabBookmarkAt(int index) const;

    Q_INVOKABLE WTabTrack * tabFromSource(const QString & source) const;

public: // WLocalObject reimplementation
    /* Q_INVOKABLE virtual */ QString getParentPath() const;

protected: // WAbstractTabs implementation
    /* virtual */ WAbstractTab * createTab(WAbstractTabs * parent = NULL) const;

protected: // WLocalObject reimplementation
    /* virtual */ WAbstractThreadAction * onSave(const QString & path);
    /* virtual */ WAbstractThreadAction * onLoad(const QString & path);

    /* virtual */ bool hasFolder() const;

protected: // WAbstractTabs reimplementation
    /* virtual */ void updateIndex();

signals:
    void highlightedTabChanged  ();
    void highlightedIndexChanged();

public: // Properties
    WTabTrack * highlightedTab() const;
    void        setHighlightedTab(WTabTrack * tab);

    int  highlightedIndex() const;
    void setHighlightedIndex(int index);

private:
    W_DECLARE_PRIVATE(WTabsTrack)

    Q_PRIVATE_SLOT(d_func(), void onHighlightedTabDestroyed())
};

#include <private/WTabsTrack_p>

#endif // SK_NO_TABSTRACK
#endif // WTABSTRACK_H
