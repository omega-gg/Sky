//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WCONTROLLERDOWNLOAD_P_H
#define WCONTROLLERDOWNLOAD_P_H

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
#include <private/WController_p>

#ifndef SK_NO_CONTROLLERDOWNLOAD

class WControllerDownloadPrivate : public WControllerPrivate
{
public:
    WControllerDownloadPrivate(WControllerDownload * p);

    /* virtual */ ~WControllerDownloadPrivate();

    void init();

public: // Functions
    void processJobs();

    bool checkJobs(const QString & host, int maxHost) const;

    int checkDelay(WRemoteData * data, const QString & host);

    void completeJob(WRemoteData * data);

    void clearLoader(WAbstractLoader * loader);

public: // Slots
    void onProcess();

public: // Variables
    WLoaderNetwork * loader;

    QList<WRemoteData *> jobsPending;
    QList<WRemoteData *> jobs;

    int maxJobs;

    QHash<QString, QTime> delays;

    QTimer timer;

protected:
    W_DECLARE_PUBLIC(WControllerDownload)
};

#endif // SK_NO_CONTROLLERDOWNLOAD
#endif // WCONTROLLERDOWNLOAD_P_H
