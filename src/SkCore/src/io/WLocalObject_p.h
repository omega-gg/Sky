//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkCore.

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

#ifndef WLOCALOBJECT_P_H
#define WLOCALOBJECT_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Qt includes
#include <QTimer>

// Private includes
#include <private/Sk_p>

#ifndef SK_NO_LOCALOBJECT

class SK_CORE_EXPORT WLocalObjectPrivate : public WPrivate
{
public:
    WLocalObjectPrivate(WLocalObject * p);

    /* virtual */ ~WLocalObjectPrivate();

    void init();

public: // Functions
    void updateProcessing();

    void setSaving(bool saving);

    void setState(WLocalObject::State state);

    void setSaved (bool ok);
    void setLoaded(bool ok);

    bool startSave(bool instant);

    void createPath  ();
    void createFolder();

    void setLockCount(int count);

    void updateLock();

public: // Slots
    void onSaveTimeout();

public: // Variables
    int id;

    bool pathCreated;
    bool folderCreated;

    bool processing;

    WAbstractThreadAction * actionSave;
    WAbstractThreadAction * actionLoad;

    bool saveEnabled;
    bool saving;

    QTimer timer;

    WLocalObject::State state;

    bool blocked;
    int  lockCount;

    bool cacheLoad;
    bool instant;

    bool toDelete;

    bool locked;

protected:
    W_DECLARE_PUBLIC(WLocalObject)
};

#endif // SK_NO_LOCALOBJECT
#endif // WLOCALOBJECT_P_H
