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

#ifndef WABSTRACTTABS_P_H
#define WABSTRACTTABS_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QHash>

// Sk includes
#include <private/WLocalObject_p>
#include <WListId>

#ifndef SK_NO_ABSTRACTTABS

class SK_GUI_EXPORT WAbstractTabsPrivate : public WLocalObjectPrivate
{
public:
    WAbstractTabsPrivate(WAbstractTabs * p);

    /* virtual */ ~WAbstractTabsPrivate();

    void init();

public: // WTabsTrackReadReply interface
    WAbstractTab * createTab(WAbstractTabs * parent = NULL) const;

    void loadTabs(const QList<WAbstractTab *> & tabs);

public: // Watchers functions
    void beginTabsInsert(int first, int last) const;
    void endTabsInsert  ()                    const;

    void beginTabsMove(int first, int last, int to) const;
    void endTabsMove  ()                            const;

    void beginTabsRemove(int first, int last) const;
    void endTabsRemove  ()                    const;

    void beginTabsClear() const;
    void endTabsClear  () const;

public: // Variables
    QList<WAbstractTab *> tabs;

    WListId                    ids;
    QHash<int, WAbstractTab *> idHash;

    WAbstractTab * currentTab;

    int currentIndex;

    QList<WAbstractTabsWatcher *> watchers;

    int maxCount;

protected:
    W_DECLARE_PUBLIC(WAbstractTabs)
};

#endif // SK_NO_ABSTRACTTABS
#endif // WABSTRACTTABS_P_H
