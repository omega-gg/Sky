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

#ifndef WPLUGIN_P_H
#define WPLUGIN_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

#include <private/Sk_p>

// Forward declarations
class QPluginLoader;

class SK_CORE_EXPORT WPluginPrivate : public WPrivate
{
public:
    WPluginPrivate(WPlugin * p);

    virtual ~WPluginPrivate();

    void init();

private: // Functions
    bool loadSpecs(const QString & specFileName);

protected: // Variables
    QString fileName;

    QPluginLoader * loader;

    // Name
    QString name;

    // Version
    QString version;
    QString compatVersion;

    // Description
    QString description;
    QString url;

    // License
    QString license;

protected:
    W_DECLARE_PUBLIC(WPlugin)
};

#endif // WPLUGIN_P_H
