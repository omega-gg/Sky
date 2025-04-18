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

/*!
    \class WControllerApplication

    \brief The WControllerApplication class provides a controller for WCoreApplication and
           WApplication.

    \sa WCoreApplication, WApplication
*/

#include "WControllerApplication.h"

#ifndef SK_NO_CONTROLLERAPPLICATION

// Qt includes
#ifndef SK_CONSOLE
#ifdef QT_4
    #include <QApplication>
    #include <QDeclarativeEngine>
    #include <QDeclarativeComponent>
#else
    #include <QGuiApplication>
    #include <QQmlEngine>
    #include <QQmlComponent>
    #include <QCursor>
    #if defined(Q_OS_LINUX) && defined(Q_OS_ANDROID) == false
        #include <QDBusInterface>
    #endif
#endif
#include <QFontMetrics>
#include <QClipboard>
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 1)
    #include <QLoggingCategory>
#endif
#else
#include <QCoreApplication>
#endif
#include <QCryptographicHash>
#include <QTextCodec>
#include <QMimeData>
#include <QDir>
#include <QProcess>
#ifdef QT_NEW
#include <QTimeZone>
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#include <QRandomGenerator>
#endif
#ifdef Q_OS_WIN
#include <QSettings>
#elif defined(Q_OS_IOS)
#include <QDesktopServices>
    #ifdef QT_6
        #include <QPermissions>
    #endif
#elif defined (Q_OS_ANDROID)
#ifdef QT_5
    #include <QtAndroid>
    #include <QAndroidJniEnvironment>
#else
    #include <QJniObject>
#endif
#endif

// Sk incudes
#include <WControllerFile>
#include <WControllerNetwork>
#include <WControllerDownload>
#include <WControllerXml>
#ifndef SK_NO_QML
#include <WControllerDeclarative>
#endif
#include <WRegExp>

// 3rdparty includes
#ifdef SK_CHARSET
#include <charsetdetect.h>
#endif

// Windows includes
#ifdef Q_OS_WIN
#include <qt_windows.h>
#ifdef _MSC_VER
#include <shellapi.h>
#endif
#endif

// macOS includes
#ifdef Q_OS_MACOS
#include <CoreServices/CoreServices.h>
#endif

// Qt private includes
#if defined(Q_OS_ANDROID) && defined(QT_6)
#include <QtCore/private/qandroidextras_p.h>
#endif

// Typedefs
#if defined(Q_OS_ANDROID) && defined(QT_5)
typedef QAndroidJniObject QJniObject;
#endif

W_INIT_CONTROLLER(WControllerApplication)

//-------------------------------------------------------------------------------------------------
// Defines

#ifdef Q_OS_ANDROID
#ifdef QT_5
#define W_ANDROID_ACTIVITY(Name)                    \
                                                    \
    QJniObject Name = QtAndroid::androidActivity(); \

#else
#define W_ANDROID_ACTIVITY(Name)                                        \
                                                                        \
    QJniObject Name = QNativeInterface::QAndroidApplication::context(); \

#endif
#endif

//-------------------------------------------------------------------------------------------------
// Static variables

static const QString CONTROLLERAPPLICATION_CHARACTERS = "abcdefghijklmnopqrstuvwxyz"
                                                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                                        "0123456789";

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include "WControllerApplication_p.h"

WControllerApplicationPrivate::WControllerApplicationPrivate(WControllerApplication * p)
    : WPrivate(p) {}

/* virtual */ WControllerApplicationPrivate::~WControllerApplicationPrivate()
{
#ifndef SK_CONSOLE
    Q_Q(WControllerApplication);

    q->setScreenDimEnabled  (true);
    q->setScreenSaverEnabled(true);
#endif

    W_CLEAR_CONTROLLER(WControllerApplication);
}

//-------------------------------------------------------------------------------------------------

void WControllerApplicationPrivate::init()
{
    application = NULL;

    type = Sk::Single;
    gui  = false;

    version = "1.0";

    qrc = true;

#ifndef SK_CONSOLE
    defaultMode = WControllerApplication::Normal;

    defaultScreen = -1;

    defaultWidth  = -1;
    defaultHeight = -1;

    defaultMargins = 8;

    screenDimEnabled   = true;
    screenSaverEnabled = true;

    cursorVisible = true;
#endif

    object = NULL;

    applicationUrl = "app://sk";

#ifdef Q_OS_WIN
    timeoutLowPower   = 0;
    timeoutPowerOff   = 0;
    timeoutScreenSave = 0;
#endif

#ifdef QT_4
    qsrand(QTime::currentTime().msec());
#endif
}

//-------------------------------------------------------------------------------------------------

void WControllerApplicationPrivate::declareController(WController * controller)
{
    //Q_Q(WControllerApplication);

    controller->setParent(application);

    controllers.append(controller);

    //emit q->controllerCreated(controller);
}

void WControllerApplicationPrivate::undeclareController(WController * controller)
{
    //Q_Q(WControllerApplication);

    controllers.removeOne(controller);

    //emit q->controllerDestroyed(controller);
}

//-------------------------------------------------------------------------------------------------
// Private WControllerApplication and WApplication interface
//-------------------------------------------------------------------------------------------------

void WControllerApplicationPrivate::initApplication(QCoreApplication * application, Sk::Type type,
                                                                                    bool     gui)
{
    Q_Q(WControllerApplication);

    this->application = application;

    this->type = type;
    this->gui  = gui;

    //---------------------------------------------------------------------------------------------
    // Controllers - declaration order matters

    W_CREATE_CONTROLLER(WControllerFile);
    //W_CREATE_CONTROLLER(WControllerScript);
    W_CREATE_CONTROLLER(WControllerNetwork);
    W_CREATE_CONTROLLER(WControllerDownload);
    //W_CREATE_CONTROLLER(WControllerPlugin);
    W_CREATE_CONTROLLER(WControllerXml);
#ifndef SK_NO_QML
    W_CREATE_CONTROLLER(WControllerDeclarative);
#endif

    //---------------------------------------------------------------------------------------------

    QObject::connect(qApp, SIGNAL(aboutToQuit()), q, SLOT(onAboutToQuit()));
}

//-------------------------------------------------------------------------------------------------
// Private static functions
//-------------------------------------------------------------------------------------------------

#ifdef QT_4
/* static */ void WControllerApplicationPrivate::messageHandler(QtMsgType, const char *) {}
#else
/* static */ void WControllerApplicationPrivate::messageHandler(QtMsgType,
                                                                const QMessageLogContext &,
                                                                const QString            &) {}
#endif

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WControllerApplicationPrivate::onAboutToQuit()
{
    Q_Q(WControllerApplication);

    emit q->aboutToQuit();

    wControllerFile->waitActions();

    QtMsgType verbosity = wControllerFile->verbosity();

    wControllerFile->d_func()->clearMessageHandler();

    // NOTE: When the verbosity is set, we apply a dummy handler to avoid printing messages after
    //       the WControlleFile deletion.
    if (verbosity != QtDebugMsg)
    {
#ifdef QT_4
        qInstallMsgHandler(WControllerApplicationPrivate::messageHandler);
#else
        qInstallMessageHandler(WControllerApplicationPrivate::messageHandler);
#endif
    }

    if (object)
    {
        delete object;

        object = NULL;
    }

    for (int i = controllers.count() - 1; i > -1; i--)
    {
        qDebug("Deleting %s", controllers[i]->metaObject()->className());

        delete controllers[i];
    }

    controllers.clear();

    qDebug("Done");
}

//-------------------------------------------------------------------------------------------------
// Private ctor / dtor
//-------------------------------------------------------------------------------------------------

WControllerApplication::WControllerApplication()
    : QObject(), WPrivatable(new WControllerApplicationPrivate(this)) {}

void WControllerApplication::initController()
{
    Q_D(WControllerApplication); d->init();
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

#if defined(SK_CONSOLE) == false && defined(SK_NO_QML) == false

/* Q_INVOKABLE */ void WControllerApplication::startScript()
{
    Q_D(WControllerApplication);

    if (d->object) return;

#if defined(QT_5) && QT_VERSION >= QT_VERSION_CHECK(5, 15, 1)
    // NOTE: This removes the infamous QML Connections warning.
    QLoggingCategory::setFilterRules("qt.qml.connections.warning=false");
#endif

#ifdef QT_4
    QDeclarativeComponent component(wControllerDeclarative->engine());
#else
    QQmlComponent component(wControllerDeclarative->engine());
#endif

    if (d->qrc)
    {
        component.loadUrl(QUrl("qrc:/Main.qml"));
    }
#ifdef Q_OS_MACOS
    // NOTE macOS: We have to take the application bundle into account.
    else if (d->gui)
    {
        component.loadUrl(QUrl("../../../Main.qml"));
    }
    else component.loadUrl(QUrl("Main.qml"));
#else
    else component.loadUrl(QUrl("Main.qml"));
#endif

    d->object = component.create();

    if (d->object == NULL)
    {
        qFatal("Cannot create Main QML object: %s.", component.errorString().C_STR);
    }
}

/* Q_INVOKABLE */ void WControllerApplication::restartScript()
{
    Q_D(WControllerApplication);

    QObject * objectOld = d->object;

    wControllerDeclarative->engine()->clearComponentCache();

#ifdef QT_4
    QDeclarativeComponent component(wControllerDeclarative->engine());
#else
    QQmlComponent component(wControllerDeclarative->engine());
#endif

    if (d->qrc)
    {
         component.loadUrl(QUrl("qrc:/Main.qml"));
    }
    else component.loadUrl(QUrl("Main.qml"));

    d->object = component.create();

    if (d->object == NULL)
    {
        qWarning("WControllerApplication::restartScript: Cannot reload Main.qml. %s.",
                 component.errorString().C_STR);
    }

    if (objectOld) objectOld->deleteLater();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerApplication::clearComponentCache() const
{
    wControllerDeclarative->engine()->clearComponentCache();
}

#endif

/* Q_INVOKABLE static */
WControllerApplication::Permission WControllerApplication::checkPermission(const QString & id)
{
#ifdef Q_OS_ANDROID
    QString string;

    if (id == "camera")
    {
        string = "android.permission.CAMERA";
    }
    else if (id == "vibrate")
    {
        string = "android.permission.VIBRATE";
    }
    else return Granted;

#ifdef QT_OLD
    if (QtAndroid::checkPermission(string) == QtAndroid::PermissionResult::Granted) return Granted;

    QtAndroid::requestPermissions(QStringList(string),
                                  [this, id, string](QtAndroid::PermissionResultMap hash)
    {
        if (hash.value(string) == QtAndroid::PermissionResult::Granted)
        {
             emit permissionUpdated(id, RequestedGranted);
        }
        else emit permissionUpdated(id, Denied);
    });

    return Requested;
#else
    QtAndroidPrivate::PermissionResult result = QtAndroidPrivate::checkPermission(string).result();

    if (result == QtAndroidPrivate::Authorized) return Granted;

    result = QtAndroidPrivate::requestPermission(string).result();

    if (result == QtAndroidPrivate::Authorized) return RequestedGranted;
    else                                        return Denied;
#endif
#elif defined(Q_OS_IOS) && defined(QT_6)
    QPermission permission;

    if (id == "camera")
    {
        permission = QCameraPermission();
    }
    else return Granted;

    if (qApp->checkPermission(permission) == Qt::PermissionStatus::Granted) return Granted;

    qApp->requestPermission(permission, [this, id](const QPermission & permission)
    {
        if (permission.status() == Qt::PermissionStatus::Granted)
        {
             emit permissionUpdated(id, RequestedGranted);
        }
        else emit permissionUpdated(id, Denied);
    });

    return Requested;
#else
    Q_UNUSED(id);

    return Granted;
#endif
}


#ifdef Q_OS_IOS

void WControllerApplication::applyUrlHandler(const QString & scheme, bool enabled)
{
    if (enabled)
    {
        QDesktopServices::setUrlHandler(scheme, this, "onUrl");
    }
    else QDesktopServices::unsetUrlHandler(scheme);
}

#endif

/* Q_INVOKABLE */ void WControllerApplication::quit()
{
    qApp->quit();
}

//-------------------------------------------------------------------------------------------------

///* Q_INVOKABLE */ QPoint WControllerApplication::cursorPosition() const
//{
//    Q_D(const WControllerApplication);

//    if (d->cursorPosition.isNull())
//    {
//         return QCursor::pos();
//    }
//    else return d->cursorPosition;
//}

///* Q_INVOKABLE */ void WControllerApplication::setCursorPosition(const QPoint & position)
//{
//    Q_D(WControllerApplication);

//    if (d->cursorPosition == position) return;

//    d->cursorPosition = position;
//}

///* Q_INVOKABLE */ void WControllerApplication::clearCursorPosition()
//{
//    Q_D(WControllerApplication);

//    d->cursorPosition = QPoint();
//}

//-------------------------------------------------------------------------------------------------

#ifndef SK_CONSOLE

/* Q_INVOKABLE */ QString WControllerApplication::clipboardText() const
{
#ifdef QT_4
    QClipboard * clipboard = QApplication::clipboard();
#else
    QClipboard * clipboard = QGuiApplication::clipboard();
#endif

    return clipboard->text();
}

/* Q_INVOKABLE */ void WControllerApplication::setClipboardText(const QString & text)
{
#ifdef QT_4
    QClipboard * clipboard = QApplication::clipboard();
#else
    QClipboard * clipboard = QGuiApplication::clipboard();
#endif

    clipboard->setText(text);
}

#endif

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

#ifdef Q_OS_WIN

/* Q_INVOKABLE static */ bool WControllerApplication::isUwp()
{
    return QCoreApplication::applicationFilePath().contains("/WindowsApps/");
}

#endif

/* Q_INVOKABLE static */ bool WControllerApplication::isPortrait()
{
#ifdef Q_OS_ANDROID
    W_ANDROID_ACTIVITY(jni)

    if (jni.isValid() == false) return false;

    QJniObject contentResolver = jni.callObjectMethod
    (
        "getContentResolver", "()Landroid/content/ContentResolver;"
    );

    jint rotate = QJniObject::callStaticMethod<jint>
    (
        "android/provider/Settings$System",
        "getInt",
        "(Landroid/content/ContentResolver;Ljava/lang/String;)I",
        contentResolver.object(),
        QJniObject::fromString("accelerometer_rotation").object()
    );

    return (rotate == 0);
#else
    return false;
#endif
}

/* Q_INVOKABLE static */ void WControllerApplication::forceLandscape(bool enabled)
{
#ifdef Q_OS_ANDROID
    W_ANDROID_ACTIVITY(jni)

    if (jni.isValid() == false) return;

    if (enabled) jni.callMethod<void>("setRequestedOrientation", "(I)V",  0);
    else         jni.callMethod<void>("setRequestedOrientation", "(I)V", -1);
#else
    Q_UNUSED(enabled);
#endif
}

/* Q_INVOKABLE static */
QString WControllerApplication::extractParameter(const QString & argument)
{
    int index = argument.indexOf('=');

    if (index == -1)
    {
        return QString();
    }
    else return argument.mid(index + 1);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ bool WControllerApplication::fuzzyCompare(qreal valueA, qreal valueB)
{
    return qFuzzyCompare(valueA, valueB);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */
QList<int> WControllerApplication::variantsToInts(const QVariantList & variants)
{
    QList<int> list;

    foreach (QVariant variant, variants)
    {
        list.append(variant.toInt());
    }

    return list;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */
QMimeData * WControllerApplication::duplicateMime(const QMimeData * source)
{
    Q_ASSERT(source);

    QMimeData * mime = new QMimeData;

    foreach (const QString & format, source->formats())
    {
        mime->setData(format, source->data(format));
    }

    return mime;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */
#ifdef QT_4
int WControllerApplication::keypad(int modifiers)
#else
Qt::KeyboardModifiers WControllerApplication::keypad(Qt::KeyboardModifiers flags)
#endif
{
#ifdef QT_4
    Qt::KeyboardModifiers flags = static_cast<Qt::KeyboardModifiers> (modifiers);
#endif

#ifdef Q_OS_MACOS
    // NOTE: On macOS an arrow key is considered part of the keypad.
    return (flags | Qt::KeypadModifier);
#else
    return flags;
#endif
}

#ifndef Q_OS_IOS

/* Q_INVOKABLE static */ void WControllerApplication::showPlayback(const QString &,
                                                                   const QString &) {}

/* Q_INVOKABLE static */ void WControllerApplication::hidePlayback() {}

#endif

//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------

#ifndef Q_OS_IOS

/* Q_INVOKABLE static */ void WControllerApplication::vibrate(int msec)
{
#ifdef Q_OS_ANDROID
#ifdef QT_5
    if (QtAndroid::checkPermission("android.permission.VIBRATE")
        ==
        QtAndroid::PermissionResult::Denied) return;
#else
    if (sk->checkPermission("vibrate") == Denied) return;
#endif

    W_ANDROID_ACTIVITY(jni)

    if (jni.isValid() == false) return;

    QJniObject service
        = QJniObject::getStaticObjectField<jstring>("android/content/Context",
                                                    "VIBRATOR_SERVICE");

    if (jni.isValid() == false) return;

    jni = jni.callObjectMethod("getSystemService",
                               "(Ljava/lang/String;)Ljava/lang/Object;",
                               service.object<jobject>());

    if (jni.isValid() == false) return;

    jni.callMethod<void>("vibrate", "(J)V", msec);
#else
    Q_UNUSED(msec);
#endif
}

#endif // Q_OS_IOS

/* Q_INVOKABLE static */ void WControllerApplication::shutdown()
{
#ifdef Q_OS_WIN
    QProcess::startDetached("shutdown", QStringList() << "/s" << "/f" << "/t" << "0");
#elif defined(Q_OS_IOS) == false
    QProcess::startDetached("shutdown", QStringList() << "-h" << "now");
#endif
}

//-------------------------------------------------------------------------------------------------

#ifdef SK_MOBILE

#ifdef Q_OS_ANDROID

/* Q_INVOKABLE static */ void WControllerApplication::openGallery()
{
    W_ANDROID_ACTIVITY(jni)

    if (jni.isValid() == false) return;

    jni.callMethod<void>("openGallery");
}

#endif

/* Q_INVOKABLE static */ void WControllerApplication::share(const QString & title,
                                                            const QString & text,
                                                            const QString & fileName,
                                                            const QString & type)
{
#ifdef Q_OS_ANDROID
    W_ANDROID_ACTIVITY(jni)

    if (jni.isValid() == false) return;

    QJniObject jniTitle    = QJniObject::fromString(title);
    QJniObject jniText     = QJniObject::fromString(text);
    QJniObject jniFileName = QJniObject::fromString(fileName);
    QJniObject jniType     = QJniObject::fromString(type);

    jni.callMethod<void>("share",
                         "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;"
                         "Ljava/lang/String;)V",
                         jniTitle.object<jstring>(), jniText.object<jstring>(),
                         jniFileName.object<jstring>(), jniType.object<jstring>());
#else // Q_OS_IOS
    if (fileName.isEmpty()) shareText(text);
    else                    shareFile(fileName);
#endif
}

#endif // SK_MOBILE

#ifdef Q_OS_ANDROID

/* Q_INVOKABLE static */ bool WControllerApplication::saveMedia(const QString    & name,
                                                                const QString    & type,
                                                                const QString    & mime,
                                                                const QString    & path,
                                                                const QByteArray & data)
{
    W_ANDROID_ACTIVITY(jni)

    if (jni.isValid() == false) return false;

    QJniObject contentResolver = jni.callObjectMethod
    (
        "getContentResolver", "()Landroid/content/ContentResolver;"
    );

    QJniObject uri;

    // NOTE: Use MediaStore.Files for non-media files.
    if (type == "Files")
    {
        uri = QJniObject::callStaticObjectMethod
        (
            "android/provider/MediaStore$Files",
            "getContentUri",
            "(Ljava/lang/String;)Landroid/net/Uri;",
            QJniObject::fromString("external").object<jstring>()
        );
    }
    else uri = QJniObject::getStaticObjectField
    (
        QString("android/provider/MediaStore$" + type).C_STR,
        "EXTERNAL_CONTENT_URI",
        "Landroid/net/Uri;"
    );

    QJniObject contentValues("android/content/ContentValues");

    contentValues.callMethod<void>("put", "(Ljava/lang/String;Ljava/lang/String;)V",
                                   QJniObject::getStaticObjectField
                                   (
                                       "android/provider/MediaStore$MediaColumns",
                                       "DISPLAY_NAME",
                                       "Ljava/lang/String;"
                                   ).object(),
                                   QJniObject::fromString(name).object());

    contentValues.callMethod<void>("put", "(Ljava/lang/String;Ljava/lang/String;)V",
                                   QJniObject::getStaticObjectField
                                   (
                                       "android/provider/MediaStore$MediaColumns",
                                       "MIME_TYPE",
                                       "Ljava/lang/String;"
                                   ).object(),
                                   QJniObject::fromString(mime).object());

    contentValues.callMethod<void>("put", "(Ljava/lang/String;Ljava/lang/String;)V",
                                   QJniObject::getStaticObjectField
                                   (
                                       "android/provider/MediaStore$MediaColumns",
                                       "RELATIVE_PATH",
                                       "Ljava/lang/String;"
                                   ).object(),
                                   QJniObject::fromString(path).object());

    const char * signature = "(Landroid/net/Uri;Landroid/content/ContentValues;)Landroid/net/Uri;";

    uri = contentResolver.callObjectMethod("insert", signature, uri.object(),
                                           contentValues.object());

    if (uri.isValid() == false) return false;

    QJniObject stream
        = contentResolver.callObjectMethod("openOutputStream",
                                           "(Landroid/net/Uri;)Ljava/io/OutputStream;",
                                           uri.object());

    if (stream.isValid() == false) return false;

#ifdef QT_5
    QAndroidJniEnvironment env;
#else
    QJniEnvironment env;
#endif

    jbyteArray array = env->NewByteArray(data.size());

    env->SetByteArrayRegion(array, 0, data.size(), reinterpret_cast<const jbyte*> (data.data()));

    stream.callMethod<void>("write", "([B)V", array);
    stream.callMethod<void>("close", "()V");

    env->DeleteLocalRef(array);

    return true;
}

/* Q_INVOKABLE static */ void WControllerApplication::scanFile(const QString & fileName)
{
    W_ANDROID_ACTIVITY(jni)

    if (jni.isValid() == false) return;

    QJniObject jniName = QJniObject::fromString(fileName);

    jni.callMethod<void>("scanFile", "(Ljava/lang/String;)V", jniName.object<jstring>());
}

/* Q_INVOKABLE static */ void WControllerApplication::prepareFullScreen(bool enabled)
{
#ifdef QT_5
    QtAndroid::runOnAndroidThread([enabled]
#else
    QNativeInterface::QAndroidApplication::runOnAndroidMainThread([enabled]
#endif
    {
        W_ANDROID_ACTIVITY(jni)

        if (jni.isValid() == false) return;

        jni.callMethod<void>("prepareFullScreen", "(Z)V", enabled);
    });
}

/* Q_INVOKABLE static */ void WControllerApplication::goBack()
{
    W_ANDROID_ACTIVITY(jni)

    if (jni.isValid() == false) return;

    jni.callMethod<void>("goBack");
}

#endif

//-------------------------------------------------------------------------------------------------
// Version

/* Q_INVOKABLE static */ int WControllerApplication::versionCheck(const QString & versionA,
                                                                  const QString & versionB)
{
    // NOTE: We can't check an empty string, so we consider it equal to the other one.
    if (versionA.isEmpty() || versionB.isEmpty()) return 0;

    QString stringA = versionA;
    QString stringB = versionB;

    // NOTE: Points and dashes are equivalent to us.
    stringA.replace('-', '.');
    stringB.replace('-', '.');

    QStringList listA = stringA.split('.');
    QStringList listB = stringB.split('.');

    int lengthA = listA.length();
    int lengthB = listB.length();

    int length = qMin(lengthA, lengthB);

    for (int i = 0; i < length; i++)
    {
        int valueA = listA.takeFirst().toInt();
        int valueB = listB.takeFirst().toInt();

        if (valueA < valueB)
        {
            return -1;
        }
        else if (valueA > valueB)
        {
            return 1;
        }
    }

    // NOTE: A longer version is considered higher than a shorter one.
    if (lengthA < lengthB)
    {
        return -1;
    }
    else if (lengthA > lengthB)
    {
        return 1;
    }
    else return 0;
}

/* Q_INVOKABLE static */ bool WControllerApplication::versionIsLower(const QString & versionA,
                                                                     const QString & versionB)
{
    return (versionCheck(versionA, versionB) == -1);
}

/* Q_INVOKABLE static */ bool WControllerApplication::versionIsHigher(const QString & versionA,
                                                                      const QString & versionB)
{
    return (versionCheck(versionA, versionB) == 1);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerApplication::getVersionLite(const QString & version)
{
    return sliceIn(version, "", "-");
}

//-------------------------------------------------------------------------------------------------
// File

/* Q_INVOKABLE static */ void WControllerApplication::setCurrentPath(const QString & path)
{
    QDir::setCurrent(path);
}

/* Q_INVOKABLE static */ bool WControllerApplication::fileExists(const QString & fileName)
{
    return WControllerFile::exists(fileName);
}

//-------------------------------------------------------------------------------------------------
// Process

/* Q_INVOKABLE static */ bool WControllerApplication::runUpdate()
{
#ifdef QT_NO_PROCESS
    // NOTE iOS: QProcess is not supported.
    return false;
#else
#ifdef Q_OS_WIN
    QString path = QCoreApplication::applicationDirPath() + "/setup.exe";
#else
    QString path = QCoreApplication::applicationDirPath() + "/setup";
#endif

    if (WControllerFile::tryAppend(WControllerFile::fileUrl(path)))
    {
        if (QProcess::startDetached(quote(path), QStringList("--updater")) == false)
        {
            return false;
        }
    }
    else if (runAdmin(path, "--updater") == false)
    {
        return false;
    }

    return true;
#endif
}

/* Q_INVOKABLE static */ bool WControllerApplication::runAdmin(const QString & fileName,
                                                               const QString & parameters)
{
#ifdef Q_OS_WIN
    intptr_t result = (intptr_t) ShellExecuteA(NULL, "runas", fileName.toUtf8().constData(),
                                               parameters.toLatin1().constData(), NULL,
                                               SW_SHOWNORMAL);

    return (result > 32);
#else
    Q_UNUSED(fileName); Q_UNUSED(parameters);

    return false;
#endif
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ void WControllerApplication::wait(int                            msec,
                                                           QEventLoop::ProcessEventsFlags flags)
{
    QTime time = QTime::currentTime().addMSecs(msec);

    while (time > QTime::currentTime())
    {
        QCoreApplication::processEvents(flags);
    }
}

/* Q_INVOKABLE static */
void WControllerApplication::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    QCoreApplication::processEvents(flags);
}

/* Q_INVOKABLE static */
void WControllerApplication::processEvents(QEventLoop::ProcessEventsFlags flags, int maxtime)
{
    QCoreApplication::processEvents(flags, maxtime);
}

//-------------------------------------------------------------------------------------------------
// String

/* Q_INVOKABLE static */ int WControllerApplication::indexBefore(const QString & string,
                                                                 const QChar   & match, int from)
{
    if (from < -1) return -1;

    int length = string.length() - 1;

    if (from == -1 || from > length)
    {
        from = length;
    }

    while (from != -1)
    {
        if (string[from] == match) return from;

        from--;
    }

    return -1;
}

/* Q_INVOKABLE static */ int WControllerApplication::indexSkip(const QString & string,
                                                               const QString & match, int skip)
{
    if (skip < 1)
    {
        return string.indexOf(match);
    }

    int index = 0;

    while (skip != -1)
    {
        index = string.indexOf(" ", index);

        if (index == -1)
        {
            if (skip)
            {
                 return -1;
            }
            else return string.length();
        }

        index++;

        skip--;
    }

    return index - 1;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerApplication::trim(const QString & string)
{
    return string.trimmed();
}

/* Q_INVOKABLE static */ QString WControllerApplication::simplify(const QString & string)
{
    return string.simplified();
}

/* Q_INVOKABLE static */ QString WControllerApplication::reverse(const QString & string)
{
    QString result;

    for (int i = string.length() - 1; i > -1; i--)
    {
        result.append(string.at(i));
    }

    return result;
}

/* Q_INVOKABLE static */ QString WControllerApplication::capitalize(const QString & string)
{
    if (string.length())
    {
        QString result = string;

        result[0] = result.at(0).toUpper();

        return result;
    }
    else return QString();
}

/* Q_INVOKABLE static */ QString WControllerApplication::quote(const QString & string)
{
    return '"' + string + '"';
}

/* Q_INVOKABLE static */ QString WControllerApplication::tabs(int count, int size)
{
    return QString().fill(' ', count * size);
}

/* Q_INVOKABLE static */ void WControllerApplication::replaceFirst(QString       * string,
                                                                   const QString & before,
                                                                   const QString & after,
                                                                   int             from)
{
    string->replace(string->indexOf(before, from), before.length(), after);
}

/* Q_INVOKABLE static */ QStringList WControllerApplication::split(const QString & string,
                                                                   const QString & separator)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    return string.split(separator, Qt::SkipEmptyParts);
#else
    return string.split(separator, QString::SkipEmptyParts);
#endif
}

/* Q_INVOKABLE static */ QStringList WControllerApplication::split(const QString & string,
                                                                   const QChar   & separator)
{
    return split(string, QString(separator));
}

/* Q_INVOKABLE static */
QStringList WControllerApplication::splitArguments(const QString & arguments)
{
    QStringList list;

    QString argument;

    bool escape = false;

    int state = 0; // Idle, Quote, Argument

    foreach (const QChar & character, arguments)
    {
        if (escape == false && character == '\\')
        {
            escape = true;

            continue;
        }

        if (state == 0) // Idle
        {
            if (character == '"')
            {
                state = 2; // Quote
            }
            else if (character.isSpace() == false)
            {
                state = 1; // Argument

                argument += character;
            }
        }
        else if (state == 1) // Argument
        {
            if (escape)
            {
                argument += character;
            }
            else if (character == '"')
            {
                state = 2; // Quote
            }
            else if (character.isSpace() == false)
            {
                argument += character;
            }
            else
            {
                state = 0; // Idle

                list.append(argument);

                argument.clear();
            }
        }
        else if (state == 2) // Quote
        {
            if (escape == false && character == '"')
            {
                if (argument.isEmpty()) state = 0; // Idle
                else                    state = 1; // Argument
            }
            else argument += character;
        }

        escape = false;
    }

    if (argument.isEmpty() == false)
    {
        list.append(argument);
    }

    return list;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ void WControllerApplication::insertLine(QString       * string,
                                                                 const QString & line,
                                                                 const QString & pattern,
                                                                 int             from)
{
    int index = string->indexOf(pattern, from);

    if (index == -1) return;

    index = string->indexOf('\n', index + 1);

    if (index == -1) return;

    string->insert(index + 1, line + '\n');
}

/* Q_INVOKABLE static */ void WControllerApplication::insertLines(QString       * string,
                                                                  const QString & line,
                                                                  const QString & pattern,
                                                                  int             from,
                                                                  int             to)
{
    QString text = line + '\n';

    int length = text.length();

    if (to == -1) to = string->length();

    int index = string->indexOf(pattern, from);

    while (index != -1 && index < to)
    {
        index = string->indexOf('\n', index + 1);

        if (index == -1 || index >= to) return;

        index++;

        string->insert(index, text);

        // NOTE: We've just added 'text' so we increment the maximum index.
        to += length;

        // NOTE: We want to skip the line to avoid an infinite loop if it contains the pattern.
        index = string->indexOf(pattern, index + length);
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ void WControllerApplication::removeLine(QString       * string,
                                                                 const QString & pattern,
                                                                 int             from)
{
    int indexA = string->indexOf(pattern, from);

    if (indexA == -1) return;

    int indexB = string->indexOf('\n', indexA);

    if (indexB == -1)
    {
        indexB = string->length();
    }
    else indexB++;

    indexA = indexBefore(*string, '\n', indexA);

    if (indexA == -1)
    {
        indexA = 0;
    }
    else indexA++;

    string->remove(indexA, indexB - indexA);
}

/* Q_INVOKABLE static */ void WControllerApplication::removeLines(QString       * string,
                                                                  const QString & pattern,
                                                                  int             from,
                                                                  int             to)
{
    int length = string->length();

    if (to == -1) to = length;

    int indexA = string->indexOf(pattern, from);

    while (indexA != -1)
    {
        int indexB = string->indexOf('\n', indexA);

        if (indexB == -1)
        {
            indexB = length;
        }
        else indexB++;

        indexA = indexBefore(*string, '\n', indexA);

        if (indexA == -1)
        {
            indexA = 0;
        }
        else indexA++;

        string->remove(indexA, indexB - indexA);

        indexA = string->indexOf(pattern, indexA);
    }
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerApplication::slice(const QString & string, int start,
                                                                                       int end)
{
    if (start > end)
    {
         return reverse(string.mid(end - 1, start - end));
    }
    else return string.mid(start, end - start);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerApplication::slice(const QString & string,
                                                               const QString & start,
                                                               const QString & end,
                                                               int             from)
{
    int indexA = string.indexOf(start, from);

    if (indexA == -1) return QString();

    int indexB = string.indexOf(end, indexA + start.length());

    if (indexB == -1)
    {
         return string.mid(indexA);
    }
    else return string.mid(indexA, indexB + end.length() - indexA);
}

/* Q_INVOKABLE static */ QString WControllerApplication::slice(const QString & string,
                                                               const WRegExp & start,
                                                               const WRegExp & end,
                                                               int             from)
{
    int indexA = start.indexIn(string, from);

    if (indexA == -1) return QString();

    int indexB = end.indexIn(string, indexA + start.matchedLength());

    if (indexB == -1)
    {
         return string.mid(indexA);
    }
    else return string.mid(indexA, indexB + end.matchedLength() - indexA);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerApplication::sliceIn(const QString & string,
                                                                 const QString & start,
                                                                 const QString & end,
                                                                 int             from)
{
    int indexA = string.indexOf(start, from);

    if (indexA == -1) return QString();

    indexA += start.length();

    int indexB = string.indexOf(end, indexA);

    if (indexB == -1)
    {
         return string.mid(indexA);
    }
    else return string.mid(indexA, indexB - indexA);
}

/* Q_INVOKABLE static */ QString WControllerApplication::sliceIn(const QString & string,
                                                                 const WRegExp & start,
                                                                 const WRegExp & end,
                                                                 int             from)
{
    int indexA = start.indexIn(string, from);

    if (indexA == -1) return QString();

    indexA += start.matchedLength();

    int indexB = end.indexIn(string, indexA);

    if (indexB == -1)
    {
         return string.mid(indexA);
    }
    else return string.mid(indexA, indexB - indexA);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QStringList WControllerApplication::slices(const QString & string,
                                                                    const QString & start,
                                                                    const QString & end,
                                                                    int             from)
{
    QStringList list;

    int length = start.length();

    // NOTE: length must be valid to avoid an infinite loop.
    if (length == 0) return list;

    int indexA = string.indexOf(start, from);

    if (indexA == -1) return list;

    int indexB = string.indexOf(end, indexA + length);

    while (1)
    {
        if (indexB == -1)
        {
             indexB = string.length();
        }
        else indexB += end.length();

        QString result = string.mid(indexA, indexB - indexA);

        list.append(result);

        indexA = string.indexOf(start, indexB);

        if (indexA == -1) return list;

        indexB = string.indexOf(end, indexA + length);
    }

    return list;
}

/* Q_INVOKABLE static */ QStringList WControllerApplication::slices(const QString & string,
                                                                    const WRegExp & start,
                                                                    const WRegExp & end,
                                                                    int             from)
{
    QStringList list;

    int indexA = start.indexIn(string, from);

    if (indexA == -1) return list;

    int length = start.matchedLength();

    // NOTE: length must be valid to avoid an infinite loop.
    if (length == 0) return list;

    int indexB = end.indexIn(string, indexA + length);

    while (1)
    {
        if (indexB == -1)
        {
             indexB = string.length();
        }
        else indexB += end.matchedLength();

        QString result = string.mid(indexA, indexB - indexA);

        list.append(result);

        indexA = start.indexIn(string, indexB);

        if (indexA == -1) return list;

        indexB = end.indexIn(string, indexA + length);
    }

    return list;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QStringList WControllerApplication::slicesIn(const QString & string,
                                                                      const QString & start,
                                                                      const QString & end,
                                                                      int             from)
{
    QStringList list;

    int indexA = string.indexOf(start, from);

    if (indexA == -1) return list;

    int length = start.length();

    int indexB = string.indexOf(end, indexA + length);

    while (1)
    {
        indexA += start.length();

        if (indexB == -1)
        {
            indexB = string.length();
        }

        QString result = string.mid(indexA, indexB - indexA);

        list.append(result);

        indexA = string.indexOf(start, indexB + end.length());

        if (indexA == -1) return list;

        indexB = string.indexOf(end, indexA + length);
    }

    return list;
}

/* Q_INVOKABLE static */ QStringList WControllerApplication::slicesIn(const QString & string,
                                                                      const WRegExp & start,
                                                                      const WRegExp & end,
                                                                      int             from)
{
    QStringList list;

    int indexA = start.indexIn(string, from);

    if (indexA == -1) return list;

    int length = start.matchedLength();

    int indexB = end.indexIn(string, indexA + length);

    while (1)
    {
        indexA += start.matchedLength();

        if (indexB == -1)
        {
            indexB = string.length();
        }

        QString result = string.mid(indexA, indexB - indexA);

        list.append(result);

        indexA = start.indexIn(string, indexB + end.matchedLength());

        if (indexA == -1) return list;

        indexB = end.indexIn(string, indexA + length);
    }

    return list;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerApplication::extractText(QString       * string,
                                                                     const QString & pattern,
                                                                     int             from)
{
    QString result;

    int index = string->indexOf(pattern, from);

    if (index == -1)
    {
        result = string->mid(from);

        string->clear();
    }
    else
    {
        result = string->mid(from, index - from);

        string->remove(0, index);
    }

    return result;
}

/* Q_INVOKABLE static */ QString WControllerApplication::extractText(QString       * string,
                                                                     const WRegExp & regExp,
                                                                     int             from)
{
    QString result;

    int index = regExp.indexIn(*string, from);

    if (index == -1)
    {
        result = string->mid(from);

        string->clear();
    }
    else
    {
        result = string->mid(from, index - from);

        string->remove(0, index);
    }

    return result;
}

/* Q_INVOKABLE static */ QString WControllerApplication::extractLine(QString * string, int from)
{
    QString result;

    int index = string->indexOf('\n', from);

    if (index == -1)
    {
        result = string->mid(from);

        string->clear();
    }
    else
    {
        result = string->mid(from, index - from);

        string->remove(0, index + 1);
    }

    return result;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerApplication::getLine(const QString & string, int at)
{
    int index = string.indexOf('\n', at);

    if (index == -1) return string.mid(at);
    else             return string.mid(at, index - at);
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ void WControllerApplication::skipCharacters(QString     * string,
                                                                     const QChar & character)
{
    int count = 0;

    while (count < string->length())
    {
        if (string->at(count) != character) break;

        count++;
    }

    string->remove(0, count);
}

/* Q_INVOKABLE static */ void WControllerApplication::skipSpaces(QString * string)
{
    skipCharacters(string, ' ');
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */
bool WControllerApplication::checkEscaped(const QString & string, int from)
{
    int count = 0;

    for (int i = from - 1; i != -1; i--)
    {
        if (string.at(i) != '\\') break;

        count++;
    }

    return (count % 2);
}

//---------------------------------------------------------------------------------------------
// Regular expression

/* Q_INVOKABLE static */ QString WControllerApplication::regExpCap(const QString & string,
                                                                   const WRegExp & regExp,
                                                                   int cap, int from)
{
    regExp.indexIn(string, from);

    return regExp.cap(cap);
}

/* Q_INVOKABLE static */
int WControllerApplication::regExpCapture(QStringList * captured,
                                          const QString & string,
                                          const WRegExp & regExp, int from)
{
    from = regExp.indexIn(string, from);

    if (from == -1) return -1;

    *captured = regExp.capturedTexts();

    return from;
}

//-------------------------------------------------------------------------------------------------
// Read

/* Q_INVOKABLE static */ QString WControllerApplication::readAscii(const QByteArray & array)
{
    return QString::fromLatin1(array, array.size());
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerApplication::readCodec(const QByteArray & array,
                                                                   const QString    & codec)
{
    QTextCodec * textCodec = QTextCodec::codecForName(codec.C_STR);

    if (textCodec == NULL)
    {
        return array;
    }

    const char * data = array.constData();

    int size = array.size();

    QTextCodec::ConverterState state;

    QString content = textCodec->toUnicode(data, size, &state);

    if (state.invalidChars)
    {
         return QString::fromLatin1(data, size);
    }
    else return content;
}

/* Q_INVOKABLE static */ QString WControllerApplication::readUtf8(const QByteArray & array)
{
    return readCodec(array, "utf-8");
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerApplication::latinToUtf8(const QString & string)
{
    return QString::fromUtf8(string.toLatin1());
}

/* Q_INVOKABLE static */ QString WControllerApplication::unicodeToUtf8(const QString & string)
{
    QString result = string;

    WRegExp regExp("(\\\\u[0-9a-fA-F]{4})");

    int index = regExp.indexIn(result);

    while (index != -1)
    {
        result.replace(index, 6, QChar(regExp.cap(1).right(4).toUShort(0, 16)));

        index = regExp.indexIn(result, index + 1);
    }

    return result;
}

//-------------------------------------------------------------------------------------------------

#ifdef SK_CHARSET

/* Q_INVOKABLE static */ QString WControllerApplication::detectCodec(const QByteArray & array)
{
    QString codec = csd_codec(array, array.length());

    return codec.toLower();
}

#else

/* Q_INVOKABLE static */ QString WControllerApplication::detectCodec(const QByteArray &)
{
    return QString();
}

#endif

//-------------------------------------------------------------------------------------------------
// Generate

/* Q_INVOKABLE static */ int WControllerApplication::randomInt()
{
#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
    return qrand();
#else
    // NOTE: We want to emulate the qrand behavior and never return negative values.
    return QRandomGenerator::global()->bounded(RAND_MAX);
#endif
}

/* Q_INVOKABLE static */ QByteArray WControllerApplication::randomString(int length)
{
    QByteArray string;

    for (int i = 0; i < length; i++)
    {
        int index = randomInt() % 62;

        string.append(CONTROLLERAPPLICATION_CHARACTERS.at(index).toLatin1());
    }

    return string;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */
QByteArray WControllerApplication::generateHmacSha1(const QByteArray & bytes,
                                                    const QByteArray & key)
{
    int blockSize = 64;

    QByteArray keyHashed;

    if (key.length() > blockSize)
    {
         keyHashed = QCryptographicHash::hash(key, QCryptographicHash::Sha1);
    }
    else keyHashed = key;

    QByteArray innerPadding(blockSize, char(0x36));
    QByteArray outerPadding(blockSize, char(0x5c));

    for (int i = 0; i < keyHashed.length(); i++)
    {
        innerPadding[i] = innerPadding[i] ^ keyHashed.at(i);
        outerPadding[i] = outerPadding[i] ^ keyHashed.at(i);
    }

    QByteArray part  = innerPadding;
    QByteArray total = outerPadding;

    part.append(bytes);

    total.append(QCryptographicHash::hash(part, QCryptographicHash::Sha1));

    QByteArray hashed = QCryptographicHash::hash(total, QCryptographicHash::Sha1);

    return hashed.toBase64();
}

//-------------------------------------------------------------------------------------------------
// Text

#ifndef SK_CONSOLE

/* Q_INVOKABLE static */ QString WControllerApplication::textElided(const QString     & text,
                                                                    const QFont       & font,
                                                                    int                 width,
                                                                    Qt::TextElideMode   mode)
{
    QFontMetrics metrics(font);

    return metrics.elidedText(text, mode, width);
}

/* Q_INVOKABLE static */ int WControllerApplication::textWidth(const QFont   & font,
                                                               const QString & text)
{
    QFontMetrics metrics(font);

#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    return metrics.horizontalAdvance(text) + 3;
#else
    return metrics.horizontalAdvance(text) + 2;
#endif
#elif defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    return metrics.width(text) + 1;
#else
    return metrics.width(text);
#endif
}

/* Q_INVOKABLE static */ int WControllerApplication::textHeight(const QFont & font)
{
    QFontMetrics metrics(font);

    return metrics.height();
}

#endif

//-------------------------------------------------------------------------------------------------
// Message

/* Q_INVOKABLE static */ QString WControllerApplication::extractMessage(const QString & message)
{
    QStringList list = splitArguments(message);

    qDebug("MESSAGE [%s]", message.C_STR);

    foreach (const QString & string, list)
    {
        qDebug("ARG [%s]", string.C_STR);
    }

    if (list.count() < 2)
    {
        return QString();
    }
    else return list.last();
}

//-------------------------------------------------------------------------------------------------

#ifdef Q_OS_ANDROID

/* Q_INVOKABLE static */ QString WControllerApplication::getIntentText()
{
    W_ANDROID_ACTIVITY(jni)

    if (jni.isValid() == false) return QString();

    return jni.callObjectMethod<jstring>("getIntentText").toString();
}

/* Q_INVOKABLE static */ void WControllerApplication::clearIntent()
{
    W_ANDROID_ACTIVITY(jni)

    if (jni.isValid() == false) return;

    jni.callMethod<void>("clearIntent");
}

#endif

//-------------------------------------------------------------------------------------------------
// Time

/* Q_INVOKABLE static */ QTime WControllerApplication::getTime()
{
    return QTime::currentTime();
}

/* Q_INVOKABLE static */ QDateTime WControllerApplication::getDate()
{
    return QDateTime::currentDateTime();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QDateTime WControllerApplication::dateFromSecs(qint64 seconds)
{
    if (seconds)
    {
#ifdef QT_4
        return QDateTime::fromMSecsSinceEpoch(seconds * 1000);
#else
        return QDateTime::fromSecsSinceEpoch(seconds);
#endif
    }
    else return QDateTime();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ int WControllerApplication::getMsecs(const QTime & time)
{
    return time.hour() * 3600000 + time.minute() * 60000 + time.second() * 1000 + time.msec();
}

/* Q_INVOKABLE static */ int WControllerApplication::getMsecsWeek(const QDateTime & date)
{
    int msecs = 0;

    int day = date.date().dayOfWeek();

    if (day) msecs += (day - 1) * 86400000;

    return msecs + getMsecs(date.time());
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ int WControllerApplication::getElapsed(const QTime & time)
{
    return time.msecsTo(QTime::currentTime());
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ qint64 WControllerApplication::dateToMSecs(const QDateTime & date)
{
    return date.toMSecsSinceEpoch();
}

/* Q_INVOKABLE static */ qint64 WControllerApplication::dateToSecs(const QDateTime & date)
{
#ifdef QT_4
    return date.toMSecsSinceEpoch() / 1000;
#else
    return date.toSecsSinceEpoch();
#endif
}

/* Q_INVOKABLE static */
QDateTime WControllerApplication::currentDateUtc(const QString & timeZone)
{
#ifdef QT_4
    Q_UNUSED(timeZone);

    // FIXME Qt4: We're not supportign time zones at the moment.
    return QDateTime::currentDateTimeUtc();
#else
    if (timeZone.isEmpty() == false)
    {
        QTimeZone zone(timeZone.C_STR);

        return QDateTime::currentDateTimeUtc().toTimeZone(zone);
    }
    else return QDateTime::currentDateTimeUtc();
#endif
}

/* Q_INVOKABLE static */ qint64 WControllerApplication::currentDateToMSecs()
{
    return dateToMSecs(QDateTime::currentDateTime());
}

/* Q_INVOKABLE static */ qint64 WControllerApplication::currentDateToSecs()
{
    return dateToSecs(QDateTime::currentDateTime());
}

/* Q_INVOKABLE static */ qint64 WControllerApplication::currentDateToMSecsWeek()
{
    return getMsecsWeek(QDateTime::currentDateTimeUtc());
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerApplication::dateToString(const QDateTime & date,
                                                                      const QString   & format)
{
    return date.toString(format);
}

/* Q_INVOKABLE static */ QString WControllerApplication::dateToStringNumber(const QDateTime & date)
{
    if (date.isValid())
    {
         return QString::number(dateToSecs(date));
    }
    else return QString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerApplication::currentDateString(const QString & format)
{
    QDateTime date = QDateTime::currentDateTime();

    return date.toString(format);
}

/* Q_INVOKABLE static */ QString WControllerApplication::dateToText(const QDateTime & date)
{
    if (date.isValid() == false)
    {
        return QString();
    }

    QDateTime dateTime = QDateTime::currentDateTime();

    int days = date.daysTo(dateTime);

    if (days > 365)
    {
        int years = days / 365;

        if (years == 1)
        {
             return QString("%1 ").arg(years) + tr("year ago");
        }
        else return QString("%1 ").arg(years) + tr("years ago");
    }

    if (days > 31)
    {
        int monthA = date    .date().month();
        int monthB = dateTime.date().month();

        int months;

        if (monthA == monthB)
        {
            months = 11;
        }
        else if (monthA > monthB)
        {
            months = 12 - monthA + monthB;
        }
        else months = monthB - monthA;

        if (months == 1)
        {
             return QString("%1 ").arg(months) + tr("month ago");
        }
        else return QString("%1 ").arg(months) + tr("months ago");
    }

    if (days > 0)
    {
        if (days == 1)
        {
             return QString("%1 ").arg(days) + tr("day ago");
        }
        else return QString("%1 ").arg(days) + tr("days ago");
    }

    if (getMsecs(date.time()) == 0)
    {
        return tr("Moments ago");
    }

    int seconds = date.secsTo(dateTime);

    if (seconds >= 60)
    {
        int minutes = seconds / 60;

        if (minutes >= 60)
        {
            int hours = minutes / 60;

            if (hours == 1)
            {
                 return QString("%1 ").arg(hours) + tr("hour ago");
            }
            else return QString("%1 ").arg(hours) + tr("hours ago");
        }
        else if (minutes == 1)
        {
             return QString("%1 ").arg(minutes) + tr("minute ago");
        }
        else return QString("%1 ").arg(minutes) + tr("minutes ago");
    }
    else if (seconds == 1)
    {
         return QString("%1 ").arg(seconds) + tr("second ago");
    }
    else return QString("%1 ").arg(seconds) + tr("seconds ago");
}

/* Q_INVOKABLE static */ int WControllerApplication::extractMsecs(const QString & text,
                                                                  int             defaultValue)
{
    QStringList list = text.split(':');

    int count = list.count();

    if (count == 1)
    {
        QStringList seconds = list.last().split('.');

        int msecs = seconds.first().toInt() * 1000;

        if (seconds.count() == 2)
        {
             return msecs + seconds.last().toInt();
        }
        else return msecs;
    }
    else if (count == 2)
    {
        QStringList seconds = list.last().split('.');

        int msecs = list.at(0).toInt() * 60000 // 1 minute
                    +
                    seconds.first().toInt() * 1000;

        if (seconds.count() == 2)
        {
             return msecs + seconds.last().toInt();
        }
        else return msecs;
    }
    else if (count == 3)
    {
        QStringList seconds = list.last().split('.');

        int msecs = list.at(0).toInt() * 3600000 // 1 hour
                    +
                    list.at(1).toInt() * 60000 // 1 hour
                    +
                    seconds.first().toInt() * 1000;

        if (seconds.count() == 2)
        {
             return msecs + seconds.last().toInt();
        }
        else return msecs;
    }
    else return defaultValue;
}

//---------------------------------------------------------------------------------------------
// BML

/* Q_INVOKABLE static */ QString WControllerApplication::readBml(const QByteArray & array)
{
    QString content = readUtf8(array);

    content.remove('\r');

    content.remove("\\\n");

    content.replace('\t', ' ');

    return content;
}

/* Q_INVOKABLE static */ void WControllerApplication::bmlVersion(QString       & bml,
                                                                 const QString & name,
                                                                 const QString & version,
                                                                 const QString & append)
{
    bml.append("# " + name + ' ' + version + append);
}

/* Q_INVOKABLE static */ void WControllerApplication::bmlTag(QString       & bml,
                                                             const QString & name,
                                                             const QString & append)
{
    bml.append(name + ':' + append);
}

/* Q_INVOKABLE static */ void WControllerApplication::bmlList(QString       & bml,
                                                              const QString & name,
                                                              const QString & append)
{
    bml.append(name + ": |" + append);
}

/* Q_INVOKABLE static */ void WControllerApplication::bmlPair(QString       & bml,
                                                              const QString & key,
                                                              const QString & value,
                                                              const QString & append)
{
    if (value.isEmpty()) return;

    bml.append(key + ": " + value + append);
}

/* Q_INVOKABLE static */ void WControllerApplication::bmlValue(QString       & bml,
                                                               const QString & value,
                                                               const QString & append)
{
    if (value.isEmpty()) return;

    bml.append(value + append);
}

/* Q_INVOKABLE static */ QString WControllerApplication::bmlDate(const QDateTime & date)
{
    return date.toString(Qt::ISODate);
}

//#ifdef Q_OS_MAC

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE */ void WControllerApplication::onUrl(const QUrl & url)
{
#ifdef Q_OS_MAC
    Q_D(WControllerApplication);

    d->message = url.toString();

    emit messageChanged();
#else
    Q_UNUSED(url);
#endif
}

//#endif

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

/*!
    \property WControllerApplication::type
    \brief the type of this application

    The value is used by the WControllerApplication class when it is constructed.

    \sa WControllerApplication::Type
*/
WControllerApplication::Type WControllerApplication::type() const
{
    Q_D(const WControllerApplication); return d->type;
}

//-------------------------------------------------------------------------------------------------

/*!
    \property WControllerApplication::isSingle
    \brief whether the application is a single instance or not

    \sa isMultiple
*/
bool WControllerApplication::isSingle() const
{
    Q_D(const WControllerApplication); return (d->type == Single);
}

/*!
    \property WControllerApplication::isMultiple
    \brief whether the application supports multiple instances or not

    \sa isSingle
*/
bool WControllerApplication::isMultiple() const
{
    Q_D(const WControllerApplication); return (d->type == Multiple);
}

//-------------------------------------------------------------------------------------------------

/*!
    \property WControllerApplication::isCore
    \brief whether the application is a core application or not

    \sa isGui
*/
bool WControllerApplication::isCore() const
{
    Q_D(const WControllerApplication); return (d->gui == false);
}

/*!
    \property WControllerApplication::isGui
    \brief whether the application is a GUI application or not

    \sa isCore
*/
bool WControllerApplication::isGui() const
{
    Q_D(const WControllerApplication); return d->gui;
}

//-------------------------------------------------------------------------------------------------

bool WControllerApplication::osWin() const
{
#ifdef Q_OS_WIN
    return true;
#else
    return false;
#endif
}

bool WControllerApplication::osMac() const
{
#ifdef Q_OS_MACOS
    return true;
#else
    return false;
#endif
}

bool WControllerApplication::osLinux() const
{
#ifdef Q_OS_LINUX
    return true;
#else
    return false;
#endif
}

//-------------------------------------------------------------------------------------------------

bool WControllerApplication::isQrc() const
{
    Q_D(const WControllerApplication); return d->qrc;
}

void WControllerApplication::setQrc(bool qrc)
{
    Q_D(WControllerApplication);

    if (d->qrc == qrc) return;

    d->qrc = qrc;

    emit qrcChanged();
}

//-------------------------------------------------------------------------------------------------

QString WControllerApplication::name() const
{
    Q_D(const WControllerApplication); return d->name;
}

void WControllerApplication::setName(const QString & name)
{
    Q_D(WControllerApplication);

    if (d->name == name) return;

    d->name = name;

    QCoreApplication::setApplicationName(d->name);

    emit nameChanged();
}

QString WControllerApplication::icon() const
{
    Q_D(const WControllerApplication); return d->icon;
}

void WControllerApplication::setIcon(const QString & icon)
{
    Q_D(WControllerApplication);

    if (d->icon == icon) return;

    d->icon = icon;

    emit iconChanged();
}

//-------------------------------------------------------------------------------------------------

QString WControllerApplication::version() const
{
    Q_D(const WControllerApplication); return d->version;
}

void WControllerApplication::setVersion(const QString & version)
{
    Q_D(WControllerApplication);

    if (d->version == version) return;

    d->version = version;

    emit versionChanged();
}

//-------------------------------------------------------------------------------------------------

/* static */ QString WControllerApplication::versionSky()
{
    return "3.0.0-1";
}

/* static */ QString WControllerApplication::versionQt()
{
    return QT_VERSION_STR;
}

//-------------------------------------------------------------------------------------------------

QString WControllerApplication::applicationUrl() const
{
    Q_D(const WControllerApplication); return d->applicationUrl;
}

void WControllerApplication::setApplicationUrl(const QString & url)
{
    Q_D(WControllerApplication);

    if (d->applicationUrl == url) return;

    d->applicationUrl = url;

    emit applicationUrlChanged();
}

//-------------------------------------------------------------------------------------------------

#if defined(Q_OS_MACOS) || defined(SK_MOBILE)

QString WControllerApplication::message() const
{
#ifdef Q_OS_MAC
    Q_D(const WControllerApplication);

    return d->message;
#elif defined(Q_OS_ANDROID)
    return getIntentText();
#else
    return QString();
#endif
}

/* Q_INVOKABLE */ void WControllerApplication::clearMessage()
{
#ifdef Q_OS_MAC
    Q_D(WControllerApplication);

    d->message = QString();
#elif defined(Q_OS_ANDROID)
    clearIntent();
#endif
}

#endif

//-------------------------------------------------------------------------------------------------

#ifdef SK_DESKTOP

bool WControllerApplication::runOnStartup() const
{
#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                       QSettings::NativeFormat);

    return settings.contains(WControllerFile::applicationName());
#else
    return false;
#endif
}

void WControllerApplication::setRunOnStartup(bool enabled)
{
#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                       QSettings::NativeFormat);

    QString fileName = QCoreApplication::applicationFilePath();

    QString name = WControllerFile::fileBaseName(fileName);

    if (settings.contains(name) == enabled) return;

    if (enabled)
    {
        QString value = quote(QDir::toNativeSeparators(fileName));

        qDebug("SETTING %s %s", name.C_STR, value.C_STR);

        settings.setValue(name, value);
    }
    else settings.remove(name);

    emit runOnStartupChanged();
#else
    Q_UNUSED(enabled);
#endif
}

#endif

#ifndef SK_CONSOLE

//-------------------------------------------------------------------------------------------------

WControllerApplication::Mode WControllerApplication::defaultMode() const
{
    Q_D(const WControllerApplication); return d->defaultMode;
}

void WControllerApplication::setDefaultMode(Mode mode)
{
    Q_D(WControllerApplication);

    if (d->defaultMode == mode) return;

    d->defaultMode = mode;

    emit defaultModeChanged();
}

//-------------------------------------------------------------------------------------------------

int WControllerApplication::defaultScreen() const
{
    Q_D(const WControllerApplication); return d->defaultScreen;
}

void WControllerApplication::setDefaultScreen(int screen)
{
    Q_D(WControllerApplication);

    if (d->defaultScreen == screen) return;

    d->defaultScreen = screen;

    emit defaultScreenChanged();
}

//-------------------------------------------------------------------------------------------------

int WControllerApplication::defaultWidth() const
{
    Q_D(const WControllerApplication); return d->defaultWidth;
}

void WControllerApplication::setDefaultWidth(int width)
{
    Q_D(WControllerApplication);

    if (d->defaultWidth == width) return;

    d->defaultWidth = width;

    emit defaultWidthChanged();
}

int WControllerApplication::defaultHeight() const
{
    Q_D(const WControllerApplication); return d->defaultHeight;
}

void WControllerApplication::setDefaultHeight(int height)
{
    Q_D(WControllerApplication);

    if (d->defaultHeight == height) return;

    d->defaultHeight = height;

    emit defaultHeightChanged();
}

//-------------------------------------------------------------------------------------------------

int WControllerApplication::defaultMargins() const
{
    Q_D(const WControllerApplication); return d->defaultMargins;
}

void WControllerApplication::setDefaultMargins(int ratio)
{
    Q_D(WControllerApplication);

    if (d->defaultMargins == ratio) return;

    d->defaultMargins = ratio;

    emit defaultMarginsChanged();
}

//-------------------------------------------------------------------------------------------------

bool WControllerApplication::screenDimEnabled() const
{
    Q_D(const WControllerApplication); return d->screenDimEnabled;
}

void WControllerApplication::setScreenDimEnabled(bool enabled)
{
    Q_D(WControllerApplication);

    if (d->screenDimEnabled == enabled) return;

    d->screenDimEnabled = enabled;

#ifdef Q_OS_WIN
    if (enabled == false)
    {
         SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED);
    }
    else SetThreadExecutionState(ES_CONTINUOUS);
#endif

    emit screenDimEnabledChanged();
}

//-------------------------------------------------------------------------------------------------

bool WControllerApplication::screenSaverEnabled() const
{
    Q_D(const WControllerApplication); return d->screenSaverEnabled;
}

void WControllerApplication::setScreenSaverEnabled(bool enabled)
{
    Q_D(WControllerApplication);

    if (d->screenSaverEnabled == enabled) return;

    d->screenSaverEnabled = enabled;

#ifdef Q_OS_WIN
    if (enabled)
    {
        SystemParametersInfo(SPI_GETLOWPOWERTIMEOUT,   0, &d->timeoutLowPower,   0);
        SystemParametersInfo(SPI_GETPOWEROFFTIMEOUT,   0, &d->timeoutPowerOff,   0);
        SystemParametersInfo(SPI_GETSCREENSAVETIMEOUT, 0, &d->timeoutScreenSave, 0);

        SystemParametersInfo(SPI_SETLOWPOWERTIMEOUT,   0, NULL, 0);
        SystemParametersInfo(SPI_SETPOWEROFFTIMEOUT,   0, NULL, 0);
        SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, 0, NULL, 0);
    }
    else
    {
        SystemParametersInfo(SPI_SETLOWPOWERTIMEOUT,   d->timeoutLowPower,   NULL, 0);
        SystemParametersInfo(SPI_SETPOWEROFFTIMEOUT,   d->timeoutPowerOff,   NULL, 0);
        SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, d->timeoutScreenSave, NULL, 0);
    }
#elif defined(Q_OS_MACOS)
    if (enabled == false)
    {
        CFStringRef name = CFStringCreateWithCString(NULL, d->name.C_STR, kCFStringEncodingASCII);

        IOPMAssertionCreateWithName(kIOPMAssertionTypeNoDisplaySleep,
                                    kIOPMAssertionLevelOn, name, &(d->assertion));

        CFRelease(name);
    }
    else IOPMAssertionRelease(d->assertion);
#elif defined(Q_OS_IOS)
    d->setScreenSaverEnabled(enabled);
#elif defined(Q_OS_LINUX) && defined(Q_OS_ANDROID) == false && defined(QT_NEW)
    // NOTE linux: We're not supporting D-Bus on Qt4.
    QDBusConnection bus = QDBusConnection::sessionBus();

    if (bus.isConnected())
    {
        QStringList services;
        QStringList paths;

        services.append("org.freedesktop.ScreenSaver");
        services.append("org.gnome.SessionManager");

        paths.append("/org/freedesktop/ScreenSaver");
        paths.append("/org/gnome/SessionManager");

        for (int i = 0; i < 2; i++)
        {
            QString service = services.at(i);

            QDBusInterface interface(service, paths.at(i), service, bus, this);

            if (interface.isValid() == false) continue;

            if (enabled)
            {
                 interface.call("UnInhibit");
            }
            else interface.call("Inhibit", d->name.C_STR, "reason");
        }
    }
#elif defined(Q_OS_ANDROID)
    // NOTE android: We run this on the GUI thread otherwise we get an exception.
#ifdef QT_5
    QtAndroid::runOnAndroidThread([enabled]
#else
    QNativeInterface::QAndroidApplication::runOnAndroidMainThread([enabled]
#endif
    {
        W_ANDROID_ACTIVITY(jni)

        if (jni.isValid())
        {
            jni = jni.callObjectMethod("getWindow", "()Landroid/view/Window;");

            if (jni.isValid())
            {
                // NOTE: 128 is the FLAG_KEEP_SCREEN_ON define.
                if (enabled)
                {
                     jni.callMethod<void>("clearFlags", "(I)V", 128);
                }
                else jni.callMethod<void>("addFlags", "(I)V", 128);
            }
        }

#ifdef QT_5
        QAndroidJniEnvironment env;
#else
        QJniEnvironment env;
#endif

        // NOTE android: It seems we need to check exceptions.
        if (env->ExceptionCheck())
        {
            env->ExceptionDescribe();

            env->ExceptionClear();
        }
    });
#endif

    emit screenSaverEnabledChanged();
}

//-------------------------------------------------------------------------------------------------

bool WControllerApplication::cursorVisible() const
{
    Q_D(const WControllerApplication); return d->cursorVisible;
}

void WControllerApplication::setCursorVisible(bool visible)
{
    Q_D(WControllerApplication);

    if (d->cursorVisible == visible) return;

    d->cursorVisible = visible;

    if (visible == false)
    {
#ifdef QT_4
        QApplication::setOverrideCursor(Qt::BlankCursor);
#else
        QGuiApplication::setOverrideCursor(Qt::BlankCursor);
#endif
    }
#ifdef QT_4
    else QApplication::restoreOverrideCursor();
#else
    else QGuiApplication::restoreOverrideCursor();
#endif

    emit cursorVisibleChanged();
}

#endif

#ifdef Q_OS_ANDROID

//---------------------------------------------------------------------------------------------
// Android JNI
//---------------------------------------------------------------------------------------------
// NOTE: These functions must be exported as C functions.

#ifdef __cplusplus
extern "C"
{
#endif

JNIEXPORT void JNICALL
Java_gg_omega_WActivity_updateIntent(JNIEnv *, jobject)
{
    emit sk->messageChanged();
}

JNIEXPORT void JNICALL
Java_gg_omega_WActivity_imageSelected(JNIEnv *, jobject, jstring fileName)
{
    emit sk->imageSelected(QJniObject(fileName).toString());
}

JNIEXPORT void JNICALL
Java_gg_omega_WActivity_shareFinished(JNIEnv *, jobject, jboolean ok)
{
    emit sk->shareFinished(ok);
}

#ifdef __cplusplus
}
#endif

#endif // Q_OS_ANDROID

#endif // SK_NO_CONTROLLERAPPLICATION
