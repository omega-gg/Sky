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

// Views includes
#include <WMainView>
#include <WWindow>

// Declarative includes
#include <WDeclarativeCoreApplication>
#include <WDeclarativeApplication>
#include <WMainViewResizer>
#include <WMainViewDrag>
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
#include <WModelPlaylistNet>
#include <WModelLibraryFolder>
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
#include <WPlaylistNet>
#include <WTabTrack>
#include <WTabsTrack>

// Image includes
#include <WImageColorFilter>

// 3rd party includes
#include <qtsingleapplication>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WControllerApplication_p>

class SK_GUI_EXPORT WApplicationPrivate : public WPrivate
{
public:
    WApplicationPrivate(WApplication * p);

    void init();

protected:
    W_DECLARE_PUBLIC(WApplication)
};

//-------------------------------------------------------------------------------------------------
// Private ctor / Dtor

WApplicationPrivate::WApplicationPrivate(WApplication * p) : WPrivate(p) {}

void WApplicationPrivate::init() {}

//-------------------------------------------------------------------------------------------------
// Message handler
//-------------------------------------------------------------------------------------------------

void messageHandler(QtMsgType, const char *) {}

//-------------------------------------------------------------------------------------------------
// Private ctor / dtor
//-------------------------------------------------------------------------------------------------

/*!
    Constructs a Sk kernel application. Kernel applications are
    applications without a graphical user interface. These type of
    applications are used at the console or as server processes.

    \warning The data pointed to by \a argc and \a argv must stay
    valid for the entire lifetime of the WApplication object.
*/

WApplication::WApplication(int & argc, char ** argv)
    : QApplication(argc, argv), WPrivatable(new WApplicationPrivate(this))
{
    Q_D(WApplication); d->init();
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* static */
QApplication * WApplication::create(int & argc, char ** argv, Sk::Type type)
{
#if defined(Q_OS_MAC) || defined(Q_OS_UNIX)
    QApplication::setGraphicsSystem("raster");
#endif

#ifdef QT_NO_DEBUG
    //qInstallMsgHandler(messageHandler);
#endif

    QtSingleApplication * app = new QtSingleApplication(argc, argv, true);

    if (app->sendMessage("")) return NULL;

    W_CREATE_CONTROLLER(WControllerApplication);

    sk->d_func()->createApplication(app, argc, argv, type, false);

    //---------------------------------------------------------------------------------------------
    // Controllers

    W_CREATE_CONTROLLER(WControllerView);
    W_CREATE_CONTROLLER(WControllerPlaylist);

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
    // Views

    qmlRegisterUncreatableType<WMainView>("Sky", 1,0, "MainView", "MainView is abstract");

    qmlRegisterType<WWindow>("Sky", 1,0, "BaseWindow");

    //---------------------------------------------------------------------------------------------
    // Declarative

    qmlRegisterType<WMainViewResizer>("Sky", 1,0, "MainViewResizer");
    qmlRegisterType<WMainViewDrag>   ("Sky", 1,0, "MainViewDrag");

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

    qmlRegisterType<WDeclarativeText>        ("Sky", 1,0, "Text");
    qmlRegisterType<WDeclarativeTextSvg>     ("Sky", 1,0, "TextSvg");
    qmlRegisterType<WDeclarativeTextSvgScale>("Sky", 1,0, "TextSvgScale");

    qmlRegisterType<WDeclarativePlayer>("Sky", 1,0, "Player");

    //---------------------------------------------------------------------------------------------
    // Models

    qmlRegisterType<WModelPlaylistNet>("Sky", 1,0, "ModelPlaylistNet");

    qmlRegisterType<WModelLibraryFolder>        ("Sky", 1,0, "ModelLibraryFolder");
    qmlRegisterType<WModelLibraryFolderFiltered>("Sky", 1,0, "ModelLibraryFolderFiltered");

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

    qmlRegisterUncreatableType<WAbstractPlaylist>("Sky", 1,0, "AbstractPlaylist",
                                                  "AbstractPlaylist is abstract");

    qmlRegisterType<WPlaylistNet>("Sky", 1,0, "PlaylistNet");

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
    wControllerDeclarative->setContextProperty("controllerPlaylist", wControllerPlaylist);

    return app;
}

#endif // SK_NO_APPLICATION
