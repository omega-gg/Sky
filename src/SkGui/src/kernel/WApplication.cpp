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

#include "WApplication.h"

#ifndef SK_NO_APPLICATION

// Qt includes
#ifdef QT_4
#include <QDeclarativeComponent>
#else
#include <QQmlComponent>
#endif
#ifdef QT_6
//#include <QQuickWindow>
#endif

// Sk includes
#include <WControllerView>

// 3rdparty includes
#include <qtsingleapplication>

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* static */
QApplication * WApplication::create(int & argc, char ** argv, Sk::Type type)
{
#ifdef QT_4
    QCoreApplication::setAttribute(Qt::AA_ImmediateWidgetCreation);
#elif defined(QT_5)
    //QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
#else // QT_6
    //QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
#endif

#ifdef Q_OS_MAC
    //QApplication::setGraphicsSystem("raster");
#endif

#ifdef QT_NO_DEBUG
    //qInstallMsgHandler(messageHandler);
#endif

    QApplication * application = createApplication(argc, argv, type);

    if (application == NULL) return NULL;

    W_CREATE_CONTROLLER(WControllerApplication);

    sk->d_func()->initApplication(application, type, true);

    //---------------------------------------------------------------------------------------------
    // Controllers

    W_CREATE_CONTROLLER(WControllerView);

    //---------------------------------------------------------------------------------------------
    // MetaType registering
    //---------------------------------------------------------------------------------------------

    qRegisterMetaType<QIODevice *>("QIODevice *");

    qRegisterMetaType<QList<QUrl> >("QList<QUrl>");

    return application;
}

//-------------------------------------------------------------------------------------------------
// Protected static functions
//-------------------------------------------------------------------------------------------------

/* static */ QApplication * WApplication::createApplication(int  &  argc,
                                                            char ** argv, Sk::Type type)
{
    if (type == Sk::Single)
    {
        QtSingleApplication * application = new QtSingleApplication(argc, argv);

        QString message;

        for (int i = 0; i < argc; i++)
        {
            if (i == argc - 1)
            {
                 message.append(QString(argv[i]));
            }
            else message.append(QString(argv[i]) + ' ');
        }

        if (application->sendMessage(message))
        {
             return NULL;
        }
        else return application;
    }
    else return new QApplication(argc, argv);
}

#endif // SK_NO_APPLICATION
