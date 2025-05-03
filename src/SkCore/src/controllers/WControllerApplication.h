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

#ifndef WCONTROLLERAPPLICATION_H
#define WCONTROLLERAPPLICATION_H

// Qt includes
#include <QEventLoop>
#include <QVariant>
#include <QTime>
#include <QStringList>
#ifdef Q_OS_IOS
#include <QMargins>
#endif

// Sk includes
#include <WController>

#ifndef SK_NO_CONTROLLERAPPLICATION

// Forward declarations
class QDateTime;
class QMimeData;
class WControllerApplicationPrivate;
class WRegExp;

// Defines
#define sk WControllerApplication::instance()
#define Sk WControllerApplication

class SK_CORE_EXPORT WControllerApplication : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_ENUMS(Type)
#ifndef SK_CONSOLE
    Q_ENUMS(Mode)
#endif
    Q_ENUMS(Playback)

    //---------------------------------------------------------------------------------------------
    // QML

    Q_ENUMS(Direction)
    Q_ENUMS(Corner)
    Q_ENUMS(IconStyle)

    //---------------------------------------------------------------------------------------------

    Q_PROPERTY(Type type READ type CONSTANT)

    Q_PROPERTY(bool isSingle   READ isSingle   CONSTANT)
    Q_PROPERTY(bool isMultiple READ isMultiple CONSTANT)

    Q_PROPERTY(bool isCore READ isCore CONSTANT)
    Q_PROPERTY(bool isGui  READ isGui  CONSTANT)

#ifdef Q_OS_WIN
    Q_PROPERTY(bool isUwp READ isUwp CONSTANT)
#endif

    Q_PROPERTY(bool qrc READ isQrc WRITE setQrc NOTIFY qrcChanged)

    Q_PROPERTY(bool osWin   READ osWin   CONSTANT)
    Q_PROPERTY(bool osMac   READ osMac   CONSTANT)
    Q_PROPERTY(bool osLinux READ osLinux CONSTANT)

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)

    Q_PROPERTY(QString version READ version WRITE setVersion NOTIFY versionChanged)

    Q_PROPERTY(QString versionSky READ versionSky CONSTANT)
    Q_PROPERTY(QString versionQt  READ versionQt  CONSTANT)

    Q_PROPERTY(QString applicationUrl READ applicationUrl WRITE setApplicationUrl
               NOTIFY applicationUrlChanged)

#if defined(Q_OS_MACOS) || defined(SK_MOBILE)
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)
#endif

#ifdef SK_DESKTOP
    Q_PROPERTY(bool runOnStartup READ runOnStartup WRITE setRunOnStartup
               NOTIFY runOnStartupChanged)
#endif

#ifndef SK_CONSOLE
    Q_PROPERTY(Mode defaultMode READ defaultMode WRITE setDefaultMode NOTIFY defaultModeChanged)

    Q_PROPERTY(int defaultScreen READ defaultScreen WRITE setDefaultScreen
               NOTIFY defaultScreenChanged)

    Q_PROPERTY(int defaultWidth READ defaultWidth WRITE setDefaultWidth
               NOTIFY defaultWidthChanged)

    Q_PROPERTY(int defaultHeight READ defaultHeight WRITE setDefaultHeight
               NOTIFY defaultHeightChanged)

    Q_PROPERTY(int defaultMargins READ defaultMargins WRITE setDefaultMargins
               NOTIFY defaultMarginsChanged)

#ifdef Q_OS_IOS
    Q_PROPERTY(QMargins safeMargins READ safeMargins WRITE setSafeMargins
               NOTIFY safeMarginsChanged)
#endif

    Q_PROPERTY(bool screenDimEnabled READ screenDimEnabled WRITE setScreenDimEnabled
               NOTIFY screenDimEnabledChanged)

    Q_PROPERTY(bool screenSaverEnabled READ screenSaverEnabled WRITE setScreenSaverEnabled
               NOTIFY screenSaverEnabledChanged)

    Q_PROPERTY(bool cursorVisible READ cursorVisible WRITE setCursorVisible
               NOTIFY cursorVisibleChanged)
#endif

public: // Enums
    enum Type { Single, Multiple };

#ifndef SK_CONSOLE
    enum Mode { Normal, Maximized, FullScreen, FullScreenMaximized };
#endif

    enum Permission { Denied, Granted, Requested, RequestedGranted };

    enum Playback { Play, Pause, Stop };

    //---------------------------------------------------------------------------------------------
    // QML

    enum Direction { Left, Right, Up, Down };

    enum Corner { TopLeft, TopRight, BottomLeft, BottomRight,
                  TopLeftCorner, TopRightCorner, BottomLeftCorner, BottomRightCorner };

    enum IconStyle
    {
        IconNormal,
        IconRaised,
        IconSunken
    };

private:
    WControllerApplication();

    void initController();

public: // Interface
#if defined(SK_CONSOLE) == false && defined(SK_NO_QML) == false
    Q_INVOKABLE void startScript  ();
    Q_INVOKABLE void restartScript();

    Q_INVOKABLE void clearComponentCache() const;
#endif

    // NOTE: The following id(s) are supported:
    //       - camera
    //       - vibrate
    Q_INVOKABLE Permission checkPermission(const QString & id);

#ifdef Q_OS_IOS
    void applyUrlHandler(const QString & scheme, bool enabled = true);
#endif

    Q_INVOKABLE void quit();

    /*Q_INVOKABLE QPoint cursorPosition() const;
    Q_INVOKABLE void   setCursorPosition(const QPoint & position);
    Q_INVOKABLE void   clearCursorPosition();*/

#ifndef SK_CONSOLE
    Q_INVOKABLE QString clipboardText() const;
    Q_INVOKABLE void    setClipboardText(const QString & text);
#endif

public: // Static functions
#ifdef Q_OS_WIN
    Q_INVOKABLE static bool isUwp();
#endif

    Q_INVOKABLE static bool hasRotateLock();

    Q_INVOKABLE static int orientation();

    // NOTE: This is a convenient function designed to retrieve camera's orientation based on the
    //       device orientation. That's currently useful for iOS.
    Q_INVOKABLE static int orientationCamera(int orientation, const QString & id);

    // NOTE: This is a convenience function that requests landscape orientation by taking the OS
    //       preferences into account. For instance, there's no way to check if the orientation
    //       lock is active on iOS, so we attempt to rotate to landscape without disabling physical
    //       rotation.
    Q_INVOKABLE static void requestLandscape(bool enabled);

    Q_INVOKABLE static void forceLandscape(bool enabled);

    Q_INVOKABLE static QString extractParameter(const QString & argument);

    Q_INVOKABLE static bool fuzzyCompare(qreal valueA, qreal valueB);

    Q_INVOKABLE static QList<int> variantsToInts(const QVariantList & variants);

    Q_INVOKABLE static QMimeData * duplicateMime(const QMimeData * source);

#ifdef QT_4
    Q_INVOKABLE static int keypad(int modifiers);
#else
    Q_INVOKABLE static Qt::KeyboardModifiers keypad(Qt::KeyboardModifiers flags);
#endif

    Q_INVOKABLE static void showPlayback(const QString & title,
                                         const QString & author = QString());

    Q_INVOKABLE static void hidePlayback();

    // NOTE iOS: This function is defined in WControllerApplication.mm
    Q_INVOKABLE static void vibrate(int msec);

    Q_INVOKABLE static void shutdown();

#ifdef SK_MOBILE
    // NOTE iOS: This function is defined in WControllerApplication.mm
    Q_INVOKABLE static void openGallery();

    Q_INVOKABLE static void share(const QString & title,
                                  const QString & text,
                                  const QString & fileName = QString(),
                                  const QString & type     = "*/*");
#endif

// FIXME Qt5.12.2: Without SK_OS_IOS, we can't access these from the QML.
#if defined(Q_OS_IOS) || defined(SK_OS_IOS)
    Q_INVOKABLE static QString deviceName   ();
    Q_INVOKABLE static QString deviceVersion();

    // NOTE iOS: These functions are defined in WControllerApplication.mm
    Q_INVOKABLE static void shareText(const QString & text);
    Q_INVOKABLE static void shareFile(const QString & fileName);

    // NOTE iOS: This function is defined in WControllerApplication.mm
    Q_INVOKABLE static void triggerLocal();
#endif

#ifdef Q_OS_ANDROID
    Q_INVOKABLE static bool saveMedia(const QString & name,
                                      const QString & type,
                                      const QString & mime,
                                      const QString & path, const QByteArray & data);

    Q_INVOKABLE static void scanFile(const QString & fileName);

    // NOTE android: This is required to avoid the notch in full screen.
    Q_INVOKABLE static void prepareFullScreen(bool enabled);

    Q_INVOKABLE static void goBack();
#endif

    //---------------------------------------------------------------------------------------------
    // Version

    // NOTE: Returns 0 when equal, -1 when lower, and 1 when higher.
    Q_INVOKABLE static int versionCheck(const QString & versionA,
                                        const QString & versionB);

    Q_INVOKABLE static bool versionIsLower(const QString & versionA,
                                           const QString & versionB);

    Q_INVOKABLE static bool versionIsHigher(const QString & versionA,
                                            const QString & versionB);

    Q_INVOKABLE static QString getVersionLite(const QString & version);

    //---------------------------------------------------------------------------------------------
    // File

    Q_INVOKABLE static void setCurrentPath(const QString & path);

    Q_INVOKABLE static bool fileExists(const QString & fileName);

    //---------------------------------------------------------------------------------------------
    // Process

    Q_INVOKABLE static bool runUpdate();

    Q_INVOKABLE static bool runAdmin(const QString & fileName, const QString & parameters);

    Q_INVOKABLE static void wait(int msec, QEventLoop::ProcessEventsFlags flags
                                           = QEventLoop::AllEvents);

    Q_INVOKABLE static void processEvents(QEventLoop::ProcessEventsFlags flags
                                          = QEventLoop::AllEvents);

    Q_INVOKABLE static void processEvents(QEventLoop::ProcessEventsFlags flags, int maxtime);

    //---------------------------------------------------------------------------------------------
    // String

    Q_INVOKABLE static int indexBefore(const QString & string, const QChar & match, int from = -1);

    Q_INVOKABLE static int indexSkip(const QString & string, const QString & match, int skip);

    Q_INVOKABLE static QString trim      (const QString & string);
    Q_INVOKABLE static QString simplify  (const QString & string);
    Q_INVOKABLE static QString reverse   (const QString & string);
    Q_INVOKABLE static QString capitalize(const QString & string);
    Q_INVOKABLE static QString quote     (const QString & string);

    Q_INVOKABLE static QString tabs(int count, int size = 4);

    Q_INVOKABLE static void replaceFirst(QString * string, const QString & before,
                                                           const QString & after,
                                                           int             from = 0);

    // NOTE: This version of split skips the empty parts by default.
    Q_INVOKABLE static QStringList split(const QString & string, const QString & separator);
    Q_INVOKABLE static QStringList split(const QString & string, const QChar   & separator);

    Q_INVOKABLE static QStringList splitArguments(const QString & arguments);

    //---------------------------------------------------------------------------------------------
    // NOTE: Insert a line on the next line(s) after the matched 'pattern'.

    Q_INVOKABLE static void insertLine(QString * string,
                                       const QString & line, const QString & pattern,
                                       int from = 0);

    Q_INVOKABLE static void insertLines(QString * string,
                                        const QString & line, const QString & pattern,
                                        int from = 0, int to = -1);

    //---------------------------------------------------------------------------------------------
    // NOTE: Remove the line(s) containing the matched 'pattern'.

    Q_INVOKABLE static void removeLine(QString * string, const QString & pattern,
                                       int from = 0);

    Q_INVOKABLE static void removeLines(QString * string, const QString & pattern,
                                        int from = 0, int to = -1);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static QString slice(const QString & string, int start, int end);

    Q_INVOKABLE static QString slice(const QString & string, const QString & start,
                                                             const QString & end,
                                                             int             from = 0);

    Q_INVOKABLE static QString slice(const QString & string, const WRegExp & start,
                                                             const WRegExp & end,
                                                             int             from = 0);

    Q_INVOKABLE static QString sliceIn(const QString & string, const QString & start,
                                                               const QString & end,
                                                               int             from = 0);

    Q_INVOKABLE static QString sliceIn(const QString & string, const WRegExp & start,
                                                               const WRegExp & end,
                                                               int             from = 0);

    Q_INVOKABLE static QStringList slices(const QString & string, const QString & start,
                                                                  const QString & end,
                                                                  int             from = 0);

    Q_INVOKABLE static QStringList slices(const QString & string, const WRegExp & start,
                                                                  const WRegExp & end,
                                                                  int             from = 0);

    Q_INVOKABLE static QStringList slicesIn(const QString & string, const QString & start,
                                                                    const QString & end,
                                                                    int             from = 0);

    Q_INVOKABLE static QStringList slicesIn(const QString & string, const WRegExp & start,
                                                                    const WRegExp & end,
                                                                    int             from = 0);

    Q_INVOKABLE static QString extractText(QString       * string,
                                           const QString & pattern, int from = 0);

    Q_INVOKABLE static QString extractText(QString       * string,
                                           const WRegExp & regExp, int from = 0);

    Q_INVOKABLE static QString extractLine(QString * string, int from = 0);

    Q_INVOKABLE static QString getLine(const QString & string, int at = 0);

    Q_INVOKABLE static void skipCharacters(QString * string, const QChar & character);

    Q_INVOKABLE static void skipSpaces(QString * string);

    Q_INVOKABLE static bool checkEscaped(const QString & string, int from);

    //---------------------------------------------------------------------------------------------
    // Regular expression

    Q_INVOKABLE static QString regExpCap(const QString & string,
                                         const WRegExp & regExp, int cap, int from = 0);

    Q_INVOKABLE static int regExpCapture(QStringList * captured,
                                         const QString & string,
                                         const WRegExp & regExp, int from);

    //---------------------------------------------------------------------------------------------
    // Read

    Q_INVOKABLE static QString readAscii(const QByteArray & array);

    Q_INVOKABLE static QString readCodec(const QByteArray & array, const QString & codec);
    Q_INVOKABLE static QString readUtf8 (const QByteArray & array);

    Q_INVOKABLE static QString latinToUtf8(const QString & string);

    Q_INVOKABLE static QString unicodeToUtf8(const QString & string);

    Q_INVOKABLE static QString detectCodec(const QByteArray & array);

    //---------------------------------------------------------------------------------------------
    // Generate

    Q_INVOKABLE static int randomInt();

    Q_INVOKABLE static QByteArray randomString(int length);

    Q_INVOKABLE static QByteArray generateHmacSha1(const QByteArray & bytes,
                                                   const QByteArray & key);

    //---------------------------------------------------------------------------------------------
    // Text

#ifndef SK_CONSOLE
    Q_INVOKABLE static QString textElided(const QString     & text,
                                          const QFont       & font,
                                          int                 width,
                                          Qt::TextElideMode   mode = Qt::ElideRight);

    Q_INVOKABLE static int textWidth (const QFont & font, const QString & text);
    Q_INVOKABLE static int textHeight(const QFont & font);
#endif

    //---------------------------------------------------------------------------------------------
    // Message

    // NOTE: This is useful to extract the argument from QtSingleApplication::messageReceived().
    Q_INVOKABLE static QString extractMessage(const QString & message);

#ifdef Q_OS_ANDROID
    Q_INVOKABLE static QString getIntentText();

    Q_INVOKABLE static void clearIntent();
#endif

    //---------------------------------------------------------------------------------------------
    // Time

    Q_INVOKABLE static QTime     getTime();
    Q_INVOKABLE static QDateTime getDate();

    Q_INVOKABLE static QDateTime dateFromSecs(qint64 seconds);

    Q_INVOKABLE static int getMsecs(const QTime & time);

    Q_INVOKABLE static int getMsecsWeek(const QDateTime & date);

    Q_INVOKABLE static int getElapsed(const QTime & time);

    Q_INVOKABLE static qint64 dateToMSecs(const QDateTime & date);
    Q_INVOKABLE static qint64 dateToSecs (const QDateTime & date);

    Q_INVOKABLE static QDateTime currentDateUtc(const QString & timeZone);

    Q_INVOKABLE static qint64 currentDateToMSecs();
    Q_INVOKABLE static qint64 currentDateToSecs ();

    Q_INVOKABLE static qint64 currentDateToMSecsWeek();

    Q_INVOKABLE static QString dateToString      (const QDateTime & date, const QString & format);
    Q_INVOKABLE static QString dateToStringNumber(const QDateTime & date);

    Q_INVOKABLE static QString currentDateString(const QString & format
                                                 = "yyyy-MM-dd_hh-mm-ss-zzz");

    Q_INVOKABLE static QString dateToText(const QDateTime & date);

    // NOTE: This extracts msecs as a raw value or mm:ss.zzz and hh:mm:ss.zzz format with optional
    //       zzz for msecs.
    Q_INVOKABLE static int extractMsecs(const QString & text, int defaultValue = 0);

    //---------------------------------------------------------------------------------------------
    // BML

    // NOTE: This reads UTF-8 and removes invalid characters.
    Q_INVOKABLE static QString readBml(const QByteArray & array);

    Q_INVOKABLE static void bmlVersion(QString       & bml,
                                       const QString & name,
                                       const QString & version,
                                       const QString & append = QString('\n'));

    Q_INVOKABLE static void bmlTag(QString       & bml,
                                   const QString & name,
                                   const QString & append = QString('\n'));

    Q_INVOKABLE static void bmlList(QString       & bml,
                                    const QString & name,
                                    const QString & append = QString('\n'));

    Q_INVOKABLE static void bmlPair(QString       & bml,
                                    const QString & key,
                                    const QString & value,
                                    const QString & append = QString('\n'));

    Q_INVOKABLE static void bmlValue(QString       & bml,
                                     const QString & value,
                                     const QString & append = QString('\n'));

    Q_INVOKABLE static QString bmlDate(const QDateTime & date);

    // FIXME iOS: Having an ifdef prevents the function to be added to the moc.
//#ifdef Q_OS_MAC
private: // Slots
    // FIXME iOS: It seems setUrlHandler won't let us declare this slot privately.
    Q_INVOKABLE void onUrl(const QUrl & url);
//#endif

signals:
    //void controllerCreated  (WController * controller);
    //void controllerDestroyed(WController * controller);

#ifdef SK_MOBILE
    void imageSelected(const QString & fileName);

    void shareFinished(bool ok);
#endif

    void permissionUpdated(const QString & id, WControllerApplication::Permission status);

    void playbackUpdated(Playback status);

    void aboutToQuit();

    void qrcChanged();

    void nameChanged();
    void iconChanged();

    void versionChanged();

    void applicationUrlChanged();

#if defined(Q_OS_MACOS) || defined(SK_MOBILE)
    // NOTE android: This is useful to notify that the 'intent' has changed.
    void messageChanged();
#endif

#ifdef SK_DESKTOP
    void runOnStartupChanged();
#endif

#ifndef SK_CONSOLE
    void defaultModeChanged();

    void defaultScreenChanged();

    void defaultWidthChanged ();
    void defaultHeightChanged();

    void defaultMarginsChanged();

#ifdef Q_OS_IOS
    void safeMarginsChanged();
#endif

    void screenDimEnabledChanged  ();
    void screenSaverEnabledChanged();

    void cursorVisibleChanged();
#endif

public: // Properties
    Type type() const;

    bool isSingle  () const;
    bool isMultiple() const;

    bool isCore() const;
    bool isGui () const;

    bool isQrc() const;
    void setQrc(bool qrc);

    bool osWin  () const;
    bool osMac  () const;
    bool osLinux() const;

    QString name() const;
    void    setName(const QString & name);

    QString icon() const;
    void    setIcon(const QString & icon);

    QString version() const;
    void    setVersion(const QString & version);

    static QString versionSky();
    static QString versionQt ();

    QString applicationUrl() const;
    void    setApplicationUrl(const QString & url);

#if defined(Q_OS_MACOS) || defined(SK_MOBILE)
    // NOTE android: This returns the 'intent' text.
    QString message() const;

    Q_INVOKABLE void clearMessage();
#endif

#ifdef SK_DESKTOP
    bool runOnStartup() const;
    void setRunOnStartup(bool enabled);
#endif

#ifndef SK_CONSOLE
    Mode defaultMode() const;
    void setDefaultMode(Mode mode);

    int  defaultScreen() const;
    void setDefaultScreen(int screen);

    int  defaultWidth() const;
    void setDefaultWidth(int width);

    int  defaultHeight() const;
    void setDefaultHeight(int height);

    int  defaultMargins() const;
    void setDefaultMargins(int ratio);

#ifdef Q_OS_IOS
    QMargins safeMargins() const;
    void     setSafeMargins(QMargins margins);
#endif

    bool screenDimEnabled() const;
    void setScreenDimEnabled(bool enabled);

    bool screenSaverEnabled() const;
    void setScreenSaverEnabled(bool enabled);

    bool cursorVisible() const;
    void setCursorVisible(bool visible);
#endif

private:
    W_DECLARE_PRIVATE   (WControllerApplication)
    W_DECLARE_CONTROLLER(WControllerApplication)

    Q_PRIVATE_SLOT(d_func(), void onAboutToQuit())

    friend class WController;
    friend class WControllerPrivate;
    friend class WCoreApplication;
    friend class WApplication;
#ifdef Q_OS_MACOS
    friend class WSingleApplication;
#endif
};

#include <private/WControllerApplication_p>

#endif // SK_NO_CONTROLLERAPPLICATION
#endif // WCONTROLLERAPPLICATION_H
