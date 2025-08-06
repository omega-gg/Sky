//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the Sky kit runtime.

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

#include "ControllerCore.h"

// Qt includes
#ifdef QT_4
#include <QCoreApplication>
#include <QDeclarativeEngine>
#else
#include <QQmlEngine>
#endif
//#include <QNetworkDiskCache>
#ifdef SK_DESKTOP
#include <QFileDialog>
#else
#include <QDir>
#if defined(Q_OS_ANDROID) && defined(QT_5) && QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    #include <QtAndroid>
#endif
#endif

// Sk includes
#include <WControllerApplication>
#include <WControllerDeclarative>
#include <WControllerView>
#include <WControllerFile>
#include <WControllerNetwork>
#include <WControllerDownload>
#include <WControllerPlaylist>
#include <WControllerMedia>
#include <WControllerTorrent>
#include <WView>
#include <WViewResizer>
#include <WViewDrag>
#include <WWindow>
#include <WCache>
#include <WActionCue>
#include <WInputCue>
#include <WLoaderNetwork>
#include <WLoaderVbml>
#include <WLoaderBarcode>
//#include <WLoaderWeb>
#include <WLoaderTorrent>
#include <WLoaderSuggest>
#include <WLoaderRecent>
#include <WLoaderTracks>
#include <WHookOutputBarcode>
#include <WHookTorrent>
#include <WLibraryFolderRelated>
#include <WAbstractTabs>
#include <WAbstractTab>
#include <WTabsTrack>
#include <WTabTrack>
#include <WVlcEngine>
#include <WBackendVlc>
#include <WBackendSubtitle>
#include <WBackendIndex>
#include <WBackendTorrent>
#include <WBackendUniversal>
#ifndef QT_4
#include <WFilterBarcode>
#endif
#include <WModelRange>
#include <WModelList>
#include <WModelOutput>
#include <WModelLibraryFolder>
#include <WModelPlaylist>
#include <WModelCompletionGoogle>
#include <WModelContextual>
#include <WModelTabs>
#include <WImageFilterColor>
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
#include <WDeclarativeTextSvg>
#include <WDeclarativeAmbient>
#include <WDeclarativeBarcode>
#include <WDeclarativeScanner>
#ifdef SK_DESKTOP
#include <WDeclarativeScannerHover>
#endif
#include <WBarcodeWriter>

W_INIT_CONTROLLER(ControllerCore)

//-------------------------------------------------------------------------------------------------
// Static variables

// NOTE: Also check DataLocal_patch, version_windows.
static const QString CORE_VERSION = "3.0.0-2";

#ifndef SK_DEPLOY
#ifdef Q_OS_MACOS
static const QString PATH_STORAGE = "/../../../storage";
static const QString PATH_BACKEND = "../../../../../../../backend";
#else
static const QString PATH_STORAGE = "/storage";
static const QString PATH_BACKEND = "../../../../backend";
#endif
#endif

//-------------------------------------------------------------------------------------------------
// Private ctor / dtor
//-------------------------------------------------------------------------------------------------

ControllerCore::ControllerCore() : WController()
{
    _index = NULL;

    //---------------------------------------------------------------------------------------------
    // Settings

    sk->setName("Sky runtime");

    sk->setVersion(CORE_VERSION);

#ifdef Q_OS_LINUX
#ifdef SK_DEPLOY
    sk->setIcon(":/icons/icon.svg");
#else
    sk->setIcon("icons/icon.svg");
#endif
#endif

#ifdef SK_DEPLOY
    _path = QDir::fromNativeSeparators(WControllerFile::pathWritable());
#else
    _path = QDir::currentPath() + PATH_STORAGE;
#endif

    wControllerFile->setPathStorage(_path);

    wControllerView->setLoadMode(WControllerView::LoadVisible);

    //---------------------------------------------------------------------------------------------
    // QML
    //---------------------------------------------------------------------------------------------
    // Qt

    qmlRegisterUncreatableType<QAbstractItemModel>("Sky", 1,0, "QAbstractItemModel",
                                                   "QAbstractItemModel is abstract");

    //---------------------------------------------------------------------------------------------
    // Global

    qmlRegisterUncreatableType<WControllerApplication>("Sky", 1,0, "Sk", "Sk is not creatable");

    //---------------------------------------------------------------------------------------------
    // Application

    qmlRegisterType<WDeclarativeApplication>("Sky", 1,0, "Application");

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
    // Image

    qmlRegisterUncreatableType<WImageFilter>("Sky", 1,0, "ImageFilter", "ImageFilter is abstract");

    qmlRegisterType<WImageFilterColor>("Sky", 1,0, "ImageFilterColor");

    qmlRegisterType<WDeclarativeGradient>    ("Sky", 1,0, "ScaleGradient");
    qmlRegisterType<WDeclarativeGradientStop>("Sky", 1,0, "ScaleGradientStop");

    //---------------------------------------------------------------------------------------------
    // Declarative

    qmlRegisterType<WDeclarativeMouseArea>   ("Sky", 1,0, "MouseArea");
    qmlRegisterType<WDeclarativeMouseWatcher>("Sky", 1,0, "MouseWatcher");

    qmlRegisterType<WDeclarativeListHorizontal>("Sky", 1,0, "ListHorizontal");
    qmlRegisterType<WDeclarativeListVertical>  ("Sky", 1,0, "ListVertical");

    qmlRegisterType<WDeclarativeContextualPage>("Sky", 1,0, "ContextualPage");

    qmlRegisterType<WDeclarativeAnimated>("Sky", 1,0, "Animated");

    qmlRegisterType<WDeclarativeBorders>("Sky", 1,0, "Borders");

    qmlRegisterType<WDeclarativeGradient>    ("Sky", 1,0, "ScaleGradient");
    qmlRegisterType<WDeclarativeGradientStop>("Sky", 1,0, "ScaleGradientStop");

    qmlRegisterUncreatableType<WDeclarativeImageBase>("Sky", 1,0, "ImageBase",
                                                      "ImageBase is abstract");

    qmlRegisterType<WDeclarativeImage>     ("Sky", 1,0, "Image");
    qmlRegisterType<WDeclarativeImageScale>("Sky", 1,0, "ImageScale");
    qmlRegisterType<WDeclarativeImageSvg>  ("Sky", 1,0, "ImageSvg");

#ifdef QT_4
    qmlRegisterType<WDeclarativeImageSvgScale>("Sky", 1,0, "ImageSvgScale");
#endif

    qmlRegisterType<WDeclarativeBorderImage>     ("Sky", 1,0, "BorderImage");
    qmlRegisterType<WDeclarativeBorderImageScale>("Sky", 1,0, "BorderImageScale");
    qmlRegisterType<WDeclarativeBorderGrid>      ("Sky", 1,0, "BorderGrid");

    qmlRegisterType<WDeclarativeTextSvg>("Sky", 1,0, "TextSvg");

#ifdef QT_4
    qmlRegisterType<WDeclarativeTextSvgScale>("Sky", 1,0, "TextSvgScale");
#endif

    qmlRegisterType<WDeclarativePlayer> ("Sky", 1,0, "Player");
    qmlRegisterType<WDeclarativeAmbient>("Sky", 1,0, "Ambient");

    qmlRegisterType<WDeclarativeScanner>("Sky", 1,0, "Scanner");

#ifdef SK_DESKTOP
    qmlRegisterType<WDeclarativeScannerHover>("Sky", 1,0, "ScannerHover");
#endif

    qmlRegisterType<WDeclarativeBarcode>("Sky", 1,0, "Barcode");

    //---------------------------------------------------------------------------------------------
    // Models

    qmlRegisterType<WModelRange>("Sky", 1,0, "ModelRange");

    qmlRegisterType<WModelList>("Sky", 1,0, "ModelList");

    qmlRegisterType<WModelOutput>("Sky", 1,0, "ModelOutput");

    qmlRegisterType<WModelLibraryFolder>        ("Sky", 1,0, "ModelLibraryFolder");
    qmlRegisterType<WModelLibraryFolderFiltered>("Sky", 1,0, "ModelLibraryFolderFiltered");

    qmlRegisterType<WModelPlaylist>        ("Sky", 1,0, "ModelPlaylist");
    qmlRegisterType<WModelPlaylistFiltered>("Sky", 1,0, "ModelPlaylistFiltered");

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

    qmlRegisterUncreatableType<WHookOutput>("Sky", 1,0, "HookOutput",
                                            "HookOutput is not creatable");

    qmlRegisterUncreatableType<WLocalObject>("Sky", 1,0, "LocalObject", "LocalObject is abstract");

    qmlRegisterUncreatableType<WLibraryItem>("Sky", 1,0, "LibraryItem", "LibraryItem is abstract");

    qmlRegisterType<WLibraryFolder>       ("Sky", 1,0, "LibraryFolder");
    qmlRegisterType<WLibraryFolderRelated>("Sky", 1,0, "LibraryFolderRelated");

    qmlRegisterType<WPlaylist>("Sky", 1,0, "Playlist");

    qmlRegisterType<WTabsTrack>("Sky", 1,0, "BaseTabsTrack");
    qmlRegisterType<WTabTrack> ("Sky", 1,0, "TabTrack");

    qmlRegisterUncreatableType<WBackendIndex>("Sky", 1,0, "BackendIndex",
                                              "BackendIndex is not creatable");

    qmlRegisterType<WBackendVlc>     ("Sky", 1,0, "BackendVlc");
    qmlRegisterType<WBackendSubtitle>("Sky", 1,0, "BackendSubtitle");

#ifndef QT_4
    qmlRegisterType<WFilterBarcode>("Sky", 1,0, "FilterBarcode");
#endif

    //---------------------------------------------------------------------------------------------
    // Events

    qmlRegisterUncreatableType<WDeclarativeDropEvent>("Sky", 1,0, "DeclarativeDropEvent",
                                                      "DeclarativeDropEvent is not creatable");

    qmlRegisterUncreatableType<WDeclarativeKeyEvent>("Sky", 1,0, "DeclarativeKeyEvent",
                                                     "DeclarativeKeyEvent is not creatable");

    //---------------------------------------------------------------------------------------------
    // Context

    wControllerDeclarative->setContextProperty("sk",   sk);
    wControllerDeclarative->setContextProperty("core", this);

    //---------------------------------------------------------------------------------------------
    // Signals

    //connect(&_watcher, SIGNAL(filesModified(const QString &, const QStringList &)),
    //        this,      SIGNAL(slideUpdated()));
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

#ifdef SK_DESKTOP

/* Q_INVOKABLE */ void ControllerCore::applyArguments(int & argc, char ** argv)
{
    if (argc < 2) return;

    _argument = QString(argv[1]);
}

#endif

/* Q_INVOKABLE */ void ControllerCore::load()
{
    if (_index) return;

    //---------------------------------------------------------------------------------------------
    // Controllers

    W_CREATE_CONTROLLER(WControllerPlaylist);

#ifdef Q_OS_WIN
    QStringList options = WVlcEngine::getOptions();

    // NOTE VLC Windows: This is useful if we want a specific volume for each player.
    options.append("--aout=directsound");

    W_CREATE_CONTROLLER_1(WControllerMedia, options);
#else
    W_CREATE_CONTROLLER(WControllerMedia);
#endif

    //---------------------------------------------------------------------------------------------
    // Log

#ifndef SK_DEPLOY
    wControllerMedia->startLog();
#endif

    //---------------------------------------------------------------------------------------------
    // Backends

    QString path = _path + "/backend";

    if (QFile::exists(path) == false)
    {
        if (QDir().mkpath(path) == false)
        {
            qWarning("ControllerCore::run: Failed to create folder %s.", path.C_STR);

            return;
        }

        WControllerFileReply * reply = copyBackends(path);

        connect(reply, SIGNAL(complete(bool)), this, SLOT(onLoaded()));
    }
    else createIndex();

    //---------------------------------------------------------------------------------------------
    // QML

    wControllerDeclarative->setContextProperty("controllerFile",     wControllerFile);
    wControllerDeclarative->setContextProperty("controllerNetwork",  wControllerNetwork);
    wControllerDeclarative->setContextProperty("controllerPlaylist", wControllerPlaylist);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void ControllerCore::updateBackends() const
{
    if (_index == NULL) return;

    _index->update();
}

/* Q_INVOKABLE */ void ControllerCore::resetBackends() const
{
    WControllerFileReply * reply = copyBackends(_path + "/backend/");

    connect(reply, SIGNAL(complete(bool)), this, SLOT(onReload()));
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void ControllerCore::clearComponentCache() const
{
    wControllerDeclarative->engine()->clearComponentCache();
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ void ControllerCore::applyBackend(WDeclarativePlayer * player)
{
    Q_ASSERT(player);

#ifdef SK_NO_TORRENT
    WBackendManager * backend = new WBackendManager;
#else
    WBackendTorrent * backend = new WBackendTorrent;
#endif

    player->setBackend(backend);
}

/* Q_INVOKABLE static */ QImage ControllerCore::generateTagSource(const QString & source)
{
    // NOTE: We don't want margins surrounding our QR code.
    return WBarcodeWriter::write(source, WBarcodeWriter::Text, QString(), 0).image;
}

/* Q_INVOKABLE static */ QImage ControllerCore::generateTagPath(const QString & path)
{
    return generateTagSource("https://omega.gg/" + path);
}

/* Q_INVOKABLE static */ bool ControllerCore::renameFile(const QString & oldPath,
                                                         const QString & newPath)
{
    return WControllerFile::renameFile(oldPath, newPath);
}

//-------------------------------------------------------------------------------------------------
// Functions private
//-------------------------------------------------------------------------------------------------

void ControllerCore::createIndex()
{
#ifdef SK_NO_TORRENT
    _index = new WBackendIndex(WControllerFile::fileUrl(_path + "/backend/indexLite.vbml"));
#else
    _index = new WBackendIndex(WControllerFile::fileUrl(_path + "/backend/index.vbml"));
#endif

    connect(_index, SIGNAL(loaded()), this, SLOT(onIndexLoaded()));
}

//-------------------------------------------------------------------------------------------------

WControllerFileReply * ControllerCore::copyBackends(const QString & path) const
{
#ifdef SK_DEPLOY
#ifdef Q_OS_ANDROID
    return WControllerPlaylist::copyBackends("assets:/backend", path);
#else
    return WControllerPlaylist::copyBackends(WControllerFile::applicationPath("backend"), path);
#endif
#else
    return WControllerPlaylist::copyBackends(WControllerFile::applicationPath(PATH_BACKEND), path);
#endif
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void ControllerCore::onLoaded()
{
    createIndex();
}

void ControllerCore::onIndexLoaded()
{
    disconnect(_index, SIGNAL(loaded()), this, SLOT(onIndexLoaded()));

#if defined(SK_BACKEND_LOCAL) && defined(SK_DEPLOY) == false
    // NOTE: This makes sure that we have the latest local vbml loaded.
    resetBackends();
#else
    _index->update();
#endif
}

void ControllerCore::onReload()
{
    if (_index == NULL) return;

    _index->clearCache();

    _index->reload();

    _index->reloadBackends();

    WBackendUniversal::clearCache();
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

#ifdef SK_DESKTOP

QString ControllerCore::argument() const
{
    return _argument;
}

#endif
