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
#ifdef Q_OS_MAC
#include <QFileOpenEvent>
#endif

// Sk includes
#include <WControllerView>

// 3rdparty includes
#include <qtsingleapplication>

#ifdef Q_OS_MAC

//=================================================================================================
// WSingleApplication
//=================================================================================================

// NOTE macOS: We need this class to forward file events to WControllerApplication.
class WSingleApplication : public QtSingleApplication
{
    Q_OBJECT

public:
    WSingleApplication(int & argc, char ** argv);

protected: // Events
    /* virtual */ bool event(QEvent * event);
};

WSingleApplication::WSingleApplication(int & argc, char ** argv)
    : QtSingleApplication(argc, argv) {}

/* virtual */ bool WSingleApplication::event(QEvent * event)
{
    if (event->type() == QEvent::FileOpen)
    {
        QFileOpenEvent * eventFile = static_cast<QFileOpenEvent *> (event);

        QUrl url = eventFile->url();

        if (file.isEmpty())
        {
            sendMessage(url.toString());
        }
        else sendMessage(file);
    }

    return QApplication::event(event);
}

#endif

//=================================================================================================
// WApplication
//=================================================================================================
// Static functions
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* static */ QApplication * WApplication::create(int & argc, char ** argv, Sk::Type type)
#elif defined(QT_5)
/* static */ QGuiApplication * WApplication::create(int & argc, char ** argv, Sk::Type type)
#else
/* static */ QGuiApplication * WApplication::create(int & argc, char ** argv, Sk::Type type,
                                                    QSGRendererInterface::GraphicsApi api)
#endif
{
#ifdef QT_4
    QCoreApplication::setAttribute(Qt::AA_ImmediateWidgetCreation);
#elif defined(QT_5)
#ifdef SK_SOFTWARE
    QQuickWindow::setSceneGraphBackend(QSGRendererInterface::Software);
#else
    //QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
#endif
#else // QT_6
    // NOTE: We want to handle dpi scaling ourselves.
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "0");

    if (api == QSGRendererInterface::Null)
    {
#ifdef SK_SOFTWARE
        QQuickWindow::setGraphicsApi(QSGRendererInterface::Software);
#else
        // NOTE Qt6: Let's stick to OpenGL by default.
        QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
#endif
    }
    else QQuickWindow::setGraphicsApi(api);

    // NOTE: We want the view geometry to be integer based at all time. We don't need this when
    //       setting QT_ENABLE_HIGHDPI_SCALING to 0.
    //QGuiApplication
    //    ::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Round);
#endif

#ifdef Q_OS_MAC
    //QApplication::setGraphicsSystem("raster");
#endif

#ifdef QT_4
    QApplication * application = createApplication(argc, argv, type);
#else
    QGuiApplication * application = createApplication(argc, argv, type);
#endif

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

#ifdef QT_4
/* static */ QApplication * WApplication::createApplication(int & argc, char ** argv,
                                                            Sk::Type type)
#else
/* static */ QGuiApplication * WApplication::createApplication(int & argc, char ** argv,
                                                               Sk::Type type)
#endif
{
    if (type == Sk::Single)
    {
#ifdef Q_OS_MAC
        WSingleApplication * application = new WSingleApplication(argc, argv);
#else
        QtSingleApplication * application = new QtSingleApplication(argc, argv);
#endif

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
#if SK_DESKTOP || QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
    else return new QApplication(argc, argv);
#else
    else return new QGuiApplication(argc, argv);
#endif
}

#endif // SK_NO_APPLICATION

#ifdef Q_OS_MAC
#include "WApplication.moc"
#endif
