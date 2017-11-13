//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WApplication.h"

#ifndef SK_NO_APPLICATION

// Qt includes
#include <QDeclarativeComponent>

// Controllers includes
#include <WControllerView>
#include <WControllerDeclarative>
#include <WControllerNetwork>
#include <WControllerPlugin>
#include <WControllerPlaylist>

// Kernel includes
#include <WAbstractTabs>
#include <WAbstractTab>
#include <WActionCue>
#include <WInputCue>

// View includes
#include <WView>
#include <WViewResizer>
#include <WViewDrag>
#include <WWindow>

// Declarative includes
#include <WDeclarativeCoreApplication>
#include <WDeclarativeApplication>
#include <WDeclarativeMouseArea>
#include <WDeclarativeMouseWatcher>
#include <WDeclarativeListView>
#include <WDeclarativeContextualPage>
#include <WDeclarativeAnimated>
#include <WDeclarativeBorders>
#include <WDeclarativeImage>
#include <WDeclarativeImageSvg>
#include <WDeclarativeBorderImage>
#include <WDeclarativeText>
#include <WDeclarativeTextSvg>
#include <WDeclarativePlayer>

// Models includes
#include <WModelLibraryFolder>
#include <WModelPlaylist>
#include <WModelRange>
#include <WModelCompletionGoogle>
#include <WModelContextual>
#include <WModelTabs>

// Media includes
#include <WAbstractBackend>
#include <WAbstractHook>
#include <WLibraryItem>
#include <WLibraryFolder>
#include <WLibraryFolderRelated>
#include <WPlaylist>
#include <WTabTrack>
#include <WTabsTrack>

// Image includes
#include <WImageColorFilter>

// 3rdparty includes
#include <qtsingleapplication>

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* static */
QApplication * WApplication::create(int & argc, char ** argv, Sk::Type type)
{
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

    //---------------------------------------------------------------------------------------------
    // QML
    //---------------------------------------------------------------------------------------------
    // Qt

    qmlRegisterUncreatableType<QAbstractItemModel>("Sky", 1,0, "QAbstractItemModel",
                                                   "QAbstractItemModel is abstract");

    //---------------------------------------------------------------------------------------------
    // Global

    qmlRegisterUncreatableType<WControllerDeclarative>("Sky", 1,0, "Sk", "Sk is not creatable");

    //---------------------------------------------------------------------------------------------
    // Applications

    qmlRegisterType<WDeclarativeCoreApplication>("Sky", 1,0, "CoreApplication");
    qmlRegisterType<WDeclarativeApplication>    ("Sky", 1,0, "Application");

    //---------------------------------------------------------------------------------------------
    // Kernel

    qmlRegisterUncreatableType<WAbstractTabs>("Sky", 1,0, "AbstractTabs",
                                              "AbstractTabs is abstract");

    qmlRegisterUncreatableType<WAbstractTab>("Sky", 1,0, "AbstractTab",
                                             "AbstractTab is abstract");

    qmlRegisterType<WActionCue>("Sky", 1,0, "ActionCue");
    qmlRegisterType<WInputCue> ("Sky", 1,0, "InputCue");

    //---------------------------------------------------------------------------------------------
    // View

    qmlRegisterUncreatableType<WView>("Sky", 1,0, "View", "View is abstract");

    qmlRegisterType<WViewResizer>("Sky", 1,0, "ViewResizer");
    qmlRegisterType<WViewDrag>   ("Sky", 1,0, "ViewDrag");

    qmlRegisterType<WWindow>("Sky", 1,0, "BaseWindow");

    //---------------------------------------------------------------------------------------------
    // Declarative

    qmlRegisterType<WDeclarativeMouseArea>   ("Sky", 1,0, "MouseArea");
    qmlRegisterType<WDeclarativeMouseWatcher>("Sky", 1,0, "MouseWatcher");
    qmlRegisterType<WDeclarativeDrag>        ("Sky", 1,0, "Drag");

    qmlRegisterType<WDeclarativeListHorizontal>("Sky", 1,0, "ListHorizontal");
    qmlRegisterType<WDeclarativeListVertical>  ("Sky", 1,0, "ListVertical");

    qmlRegisterType<WDeclarativeContextualPage>("Sky", 1,0, "ContextualPage");

    qmlRegisterType<WDeclarativeAnimated>("Sky", 1,0, "Animated");

    qmlRegisterType<WDeclarativeBorders>("Sky", 1,0, "Borders");

    qmlRegisterType<WDeclarativeImage>        ("Sky", 1,0, "Image");
    qmlRegisterType<WDeclarativeImageScale>   ("Sky", 1,0, "ImageScale");
    qmlRegisterType<WDeclarativeImageSvg>     ("Sky", 1,0, "ImageSvg");
    qmlRegisterType<WDeclarativeImageSvgScale>("Sky", 1,0, "ImageSvgScale");

    qmlRegisterType<WDeclarativeBorderImage>     ("Sky", 1,0, "BorderImage");
    qmlRegisterType<WDeclarativeBorderImageScale>("Sky", 1,0, "BorderImageScale");
    qmlRegisterType<WDeclarativeBorderGrid>      ("Sky", 1,0, "BorderGrid");

    qmlRegisterType<WDeclarativeText>        ("Sky", 1,0, "Text");
    qmlRegisterType<WDeclarativeTextSvg>     ("Sky", 1,0, "TextSvg");
    qmlRegisterType<WDeclarativeTextSvgScale>("Sky", 1,0, "TextSvgScale");

    qmlRegisterType<WDeclarativePlayer>("Sky", 1,0, "Player");

    //---------------------------------------------------------------------------------------------
    // Models

    qmlRegisterType<WModelLibraryFolder>        ("Sky", 1,0, "ModelLibraryFolder");
    qmlRegisterType<WModelLibraryFolderFiltered>("Sky", 1,0, "ModelLibraryFolderFiltered");

    qmlRegisterType<WModelPlaylist>("Sky", 1,0, "ModelPlaylist");

    qmlRegisterType<WModelRange>("Sky", 1,0, "ModelRange");

    qmlRegisterType<WModelCompletionGoogle>("Sky", 1,0, "ModelCompletionGoogle");

    qmlRegisterType<WModelContextual>("Sky", 1,0, "ModelContextual");

    qmlRegisterType<WModelTabs>("Sky", 1,0, "ModelTabs");

    //---------------------------------------------------------------------------------------------
    // Multimedia

    qmlRegisterUncreatableType<WBackendNet>("Sky", 1,0, "BackendNet", "BackendNet is abstract");

    qmlRegisterUncreatableType<WAbstractBackend>("Sky", 1,0, "AbstractBackend",
                                                 "AbstractBackend is abstract");

    qmlRegisterUncreatableType<WAbstractHook>("Sky", 1,0, "AbstractHook",
                                              "AbstractHook is abstract");

    qmlRegisterUncreatableType<WLocalObject>("Sky", 1,0, "LocalObject", "LocalObject is abstract");

    qmlRegisterUncreatableType<WLibraryItem>("Sky", 1,0, "LibraryItem", "LibraryItem is abstract");

    qmlRegisterType<WLibraryFolder>       ("Sky", 1,0, "LibraryFolder");
    qmlRegisterType<WLibraryFolderRelated>("Sky", 1,0, "LibraryFolderRelated");

    qmlRegisterType<WPlaylist>("Sky", 1,0, "Playlist");

    qmlRegisterType<WTabTrack> ("Sky", 1,0, "TabTrack");
    qmlRegisterType<WTabsTrack>("Sky", 1,0, "BaseTabsTrack");

    //---------------------------------------------------------------------------------------------
    // Image

    qmlRegisterType<WImageFilter>     ();
    qmlRegisterType<WImageColorFilter>("Sky", 1,0, "ImageColorFilter");

    qmlRegisterType<WDeclarativeGradient>    ("Sky", 1,0, "ScaleGradient");
    qmlRegisterType<WDeclarativeGradientStop>("Sky", 1,0, "ScaleGradientStop");

    //---------------------------------------------------------------------------------------------
    // Events

    qmlRegisterUncreatableType<WDeclarativeMouseEvent>("Sky", 1,0, "DeclarativeMouseEvent",
                                                       "DeclarativeMouseEvent is not creatable");

    qmlRegisterUncreatableType<WDeclarativeDropEvent>("Sky", 1,0, "DeclarativeDropEvent",
                                                      "DeclarativeDropEvent is not creatable");

    qmlRegisterUncreatableType<WDeclarativeKeyEvent>("Sky", 1,0, "DeclarativeKeyEvent",
                                                     "DeclarativeKeyEvent is not creatable");

    //---------------------------------------------------------------------------------------------
    // QML Context
    //---------------------------------------------------------------------------------------------

    wControllerDeclarative->setContextProperty("sk",                 sk);
    wControllerDeclarative->setContextProperty("controllerNetwork",  wControllerNetwork);

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
