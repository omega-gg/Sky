//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkMedia.

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

#ifndef WVLCENGINE_H
#define WVLCENGINE_H

// Qt includes
#include <QObject>
#ifdef QT_4
#include <QStringList>
#endif

// Sk includes
#include <WAbstractBackend>

#ifndef SK_NO_VLCENGINE

class WVlcEnginePrivate;

class SK_MEDIA_EXPORT WVlcEngine : public QObject, public WPrivatable
{
    Q_OBJECT

public:
    explicit WVlcEngine(const QStringList & options = QStringList(),
                        QThread * thread = NULL, QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE void deleteInstance();

public: // Static functions
    Q_INVOKABLE static QStringList getOptions();

protected: // Events
    /* virtual */ bool event(QEvent * event);

signals:
    void outputAdded(const WBackendOutput & output);

    void outputRemoved(int index);

    void outputCleared();

private:
    W_DECLARE_PRIVATE(WVlcEngine)

    friend class WVlcPlayer;
    friend class WVlcPlayerPrivate;
};

#endif // SK_NO_VLCENGINE
#endif // WVLCENGINE_H
