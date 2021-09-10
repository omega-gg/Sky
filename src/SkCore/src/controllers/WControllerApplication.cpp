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
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#include <QRandomGenerator>
#endif
#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QAndroidJniEnvironment>
#endif

// Sk incudes
#include <WControllerFile>
#include <WControllerNetwork>
#include <WControllerDownload>
#include <WControllerXml>
#ifndef SK_NO_QML
#include <WControllerDeclarative>
#endif

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
#ifdef Q_OS_MAC
#include <CoreServices/CoreServices.h>
#endif

W_INIT_CONTROLLER(WControllerApplication)

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

QHash<QString, QString> WControllerApplicationPrivate::extractArguments(int & argc, char ** argv)
{
    QHash<QString, QString> arguments;

    int count = 0;

    for (int i = 1; i < argc; i++)
    {
        QString arg(argv[i]);

        arg = arg.trimmed();

        if (arg.startsWith("--"))
        {
            int split = arg.indexOf('=');

            if (split > 0)
            {
                QString key = arg.mid(2).left(split - 2);

                QString value = arg.mid(split + 1).trimmed();

                arguments.insert(key, value);
            }
            else arguments.insert(arg.mid(2), QString());
        }
        else if (arg.startsWith('-'))
        {
            arguments.insert(arg.mid(1), QString());
        }
        else
        {
            count++;

            arguments.insert(QString::number(count), arg);
        }
    }

    return arguments;
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
// Private slots
//-------------------------------------------------------------------------------------------------

void WControllerApplicationPrivate::onAboutToQuit()
{
    Q_Q(WControllerApplication);

    emit q->aboutToQuit();

    wControllerFile->waitActions();

    wControllerFile->d_func()->clearMessageHandler();

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

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 1)
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
        component.loadUrl(QUrl("qrc:/qrc/Main.qml"));
    }
#ifdef Q_OS_MAC
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
         component.loadUrl(QUrl("qrc:/qrc/Main.qml"));
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

//-------------------------------------------------------------------------------------------------

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

#ifdef Q_OS_MAC
    // NOTE: On macOS an arrow key is considered part of the keypad.
    return (flags | Qt::KeypadModifier);
#else
    return flags;
#endif
}

//-------------------------------------------------------------------------------------------------
// Version

/* Q_INVOKABLE static */ int WControllerApplication::versionCheck(const QString & versionA,
                                                                  const QString & versionB)
{
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
    if (versionCheck(versionA, versionB) == -1)
    {
         return true;
    }
    else return false;
}

/* Q_INVOKABLE static */ bool WControllerApplication::versionIsHigher(const QString & versionA,
                                                                      const QString & versionB)
{
    if (versionCheck(versionA, versionB) == 1)
    {
         return true;
    }
    else return false;
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
    Q_UNUSED(fileName);
    Q_UNUSED(parameters);

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

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerApplication::tabs(int count, int size)
{
    QString result;

    QString tabs;

    tabs.fill(' ', size);

    for (int i = 0; i < count; i++)
    {
        result.append(tabs);
    }

    return result;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ void WControllerApplication::replaceFirst(QString       * string,
                                                                   const QString & before,
                                                                   const QString & after,
                                                                   int             from)
{
    string->replace(string->indexOf(before, from), before.length(), after);
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
                                                               const QRegExp & start,
                                                               const QRegExp & end,
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
                                                                 const QRegExp & start,
                                                                 const QRegExp & end,
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

    int indexA = string.indexOf(start, from);

    if (indexA == -1) return list;

    int length = start.length();

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
                                                                    const QRegExp & start,
                                                                    const QRegExp & end,
                                                                    int             from)
{
    QStringList list;

    int indexA = start.indexIn(string, from);

    if (indexA == -1) return list;

    int length = start.matchedLength();

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
                                                                      const QRegExp & start,
                                                                      const QRegExp & end,
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
                                                                     const QString & pattern)
{
    QString result;

    int index = string->indexOf(pattern, 1);

    if (index == -1)
    {
        result = *string;

        string->clear();
    }
    else
    {
        result = string->mid(0, index);

        string->remove(0, index);
    }

    return result;
}

/* Q_INVOKABLE static */ QString WControllerApplication::extractText(QString       * string,
                                                                     const QRegExp & regExp)
{
    QString result;

    int index = regExp.indexIn(*string, 1);

    if (index == -1)
    {
        result = *string;

        string->clear();
    }
    else
    {
        result = string->mid(0, index);

        string->remove(0, index);
    }

    return result;
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerApplication::extractLine(QString * string)
{
    QString result;

    int index = string->indexOf('\n');

    if (index == -1)
    {
        result = *string;

        string->clear();
    }
    else
    {
        result = string->mid(0, index);

        string->remove(0, index + 1);
    }

    return result;
}

/* Q_INVOKABLE static */ QString WControllerApplication::extractWord(QString * string)
{
    return extractText(string, " ");
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerApplication::regExpCap(const QString & string,
                                                                   const QString & pattern,
                                                                   int cap, int from)
{
    QRegExp regExp(pattern);

    string.indexOf(regExp, from);

    return regExp.cap(cap);
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

    QRegExp regExp("(\\\\u[0-9a-fA-F]{4})");

    int index = result.indexOf(regExp);

    while (index != -1)
    {
        result.replace(index, 6, QChar(regExp.cap(1).right(4).toUShort(0, 16)));

        index = result.indexOf(regExp, index);
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    // NOTE: We want to emulate the qrand behavior and never return negative values.
    return QRandomGenerator::global()->bounded(RAND_MAX);
#else
    return qrand();
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
    return metrics.horizontalAdvance(text) + 2;
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

/* Q_INVOKABLE static */ QString WControllerApplication::extractArgument(const QString & message)
{
    int indexA = message.indexOf(' ');

    if (indexA == -1) return QString();

    indexA++;

    int indexB = message.indexOf(' ', indexA);

    if (indexB == -1)
    {
         return message.mid(indexA, indexB);
    }
    else return message.mid(indexA, indexB - indexA);
}

/* Q_INVOKABLE static */ QString WControllerApplication::getMessage()
{
#ifdef Q_OS_ANDROID
    return getIntentText();
#else
    return QString();
#endif
}

//-------------------------------------------------------------------------------------------------

#ifdef Q_OS_ANDROID

/* Q_INVOKABLE static */ QString WControllerApplication::getIntentText()
{
    QAndroidJniObject object = QtAndroid::androidActivity();

    if (object.isValid() == false) return QString();

    object = object.callObjectMethod("getIntent", "()Landroid/content/Intent;");

    if (object.isValid() == false) return QString();

    object = object.callObjectMethod("getExtras", "()Landroid/os/Bundle;");

    if (object.isValid() == false) return QString();

    QAndroidJniObject key = QAndroidJniObject::fromString("android.intent.extra.TEXT");

    return object.callObjectMethod("getCharSequence",
                                   "(Ljava/lang/String;)Ljava/lang/CharSequence;",
                                   key.object<jstring>()).toString();
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
         return QString::number(date.toTime_t());
    }
    else return QString();
}

//-------------------------------------------------------------------------------------------------

/* Q_INVOKABLE static */ QString WControllerApplication::currentDateString(const QString & format)
{
    QDateTime date = QDateTime::currentDateTime();

    return date.toString(format);
}

//-------------------------------------------------------------------------------------------------

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
#ifdef Q_OS_MAC
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
    return "1.7.0-4";
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
#elif defined(Q_OS_MAC)
    if (enabled == false)
    {
        CFStringRef name = CFStringCreateWithCString(NULL, d->name.C_STR, kCFStringEncodingASCII);

        IOPMAssertionCreateWithName(kIOPMAssertionTypeNoDisplaySleep,
                                    kIOPMAssertionLevelOn, name, &(d->assertion));

        CFRelease(name);
    }
    else IOPMAssertionRelease(d->assertion);
    // NOTE linux: Currently we're only supporting D-Bus on Qt5.
#elif defined(Q_OS_LINUX) && defined(Q_OS_ANDROID) == false && defined(QT_LATEST)
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
    QtAndroid::runOnAndroidThread([enabled]
    {
        QAndroidJniObject jni = QtAndroid::androidActivity();

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

        QAndroidJniEnvironment env;

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
    emit sk->messageUpdated();
}

#ifdef __cplusplus
}
#endif

#endif // Q_OS_ANDROID

#endif // SK_NO_CONTROLLERAPPLICATION
