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

#ifndef WCONTROLLERNETWORK_H
#define WCONTROLLERNETWORK_H

// Qt includes
#include <QUrl>
#include <QStringList>

// Sk includes
#include <WController>

#ifndef SK_NO_CONTROLLERNETWORK

// Forward declarations
class WControllerNetworkPrivate;
class WPage;

// Defines
#define wControllerNetwork WControllerNetwork::instance()

class SK_CORE_EXPORT WControllerNetwork : public WController
{
    Q_OBJECT

    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)

    Q_PROPERTY(bool checkConnected READ isCheckingConnected WRITE setCheckConnected
               NOTIFY checkConnectedChanged)

private:
    WControllerNetwork();

protected: // Initialize
    /* virtual */ void init();

public: // Static functions
    Q_INVOKABLE static bool textIsUrl(const QString & text);
    Q_INVOKABLE static bool textIsIp (const QString & text);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static bool urlIsFile(const QUrl    & url);
    Q_INVOKABLE static bool urlIsFile(const QString & string);

    Q_INVOKABLE static bool urlIsHttp(const QUrl    & url);
    Q_INVOKABLE static bool urlIsHttp(const QString & string);

    Q_INVOKABLE static QString urlName (const QUrl & url);
    Q_INVOKABLE static QString urlTitle(const QUrl & url);

    Q_INVOKABLE static QString urlScheme(const QUrl    & url);
    Q_INVOKABLE static QString urlScheme(const QString & string);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static QString generateUrl(const QString & string,
                                           const QString & baseUrl = QString());

    Q_INVOKABLE static QString resolveUrl(const QString & string, const QString & baseUrl);

    Q_INVOKABLE static QString encodeUrl(const QUrl    & url);
    Q_INVOKABLE static QString encodeUrl(const QString & string);

    Q_INVOKABLE static QUrl encodedUrl(const QUrl    & url);
    Q_INVOKABLE static QUrl encodedUrl(const QString & string);

    Q_INVOKABLE static QString decodeUrl(const QUrl    & url);
    Q_INVOKABLE static QString decodeUrl(const QString & string);

    Q_INVOKABLE static QString extractBaseUrl(const QUrl    & url);
    Q_INVOKABLE static QString extractBaseUrl(const QString & string);

    Q_INVOKABLE static QString extractUrlHost(const QUrl    & url);
    Q_INVOKABLE static QString extractUrlHost(const QString & string);

    Q_INVOKABLE static QString extractUrlPath(const QUrl    & url);
    Q_INVOKABLE static QString extractUrlPath(const QString & string);

    Q_INVOKABLE static QString extractUrlValue(const QUrl & url, const QString & key);

    Q_INVOKABLE static int indexUrlElement(const QString & string, int from = 0);

    Q_INVOKABLE static QString extractUrlElement(const QUrl    & url,    int from = 0);
    Q_INVOKABLE static QString extractUrlElement(const QString & string, int from = 0);

    Q_INVOKABLE static int indexUrlElement(const QString & string, int index, int from);

    Q_INVOKABLE static QString extractUrlElement(const QUrl    & url,    int index, int from);
    Q_INVOKABLE static QString extractUrlElement(const QString & string, int index, int from);

    Q_INVOKABLE static QString extractUrlElements(const QUrl    & url,    int count, int from = 0);
    Q_INVOKABLE static QString extractUrlElements(const QString & string, int count, int from = 0);

    Q_INVOKABLE static QString extractUrlFileName(const QUrl    & url);
    Q_INVOKABLE static QString extractUrlFileName(const QString & string);

    Q_INVOKABLE static QString extractUrlExtension(const QUrl    & url);
    Q_INVOKABLE static QString extractUrlExtension(const QString & string);

    Q_INVOKABLE static QString removeUrlPrefix(const QUrl    & url);
    Q_INVOKABLE static QString removeUrlPrefix(const QString & string);

    Q_INVOKABLE static QString removeUrlExtension(const QUrl    & url);
    Q_INVOKABLE static QString removeUrlExtension(const QString & string);

    Q_INVOKABLE static QString removeUrlFragment(const QUrl    & url);
    Q_INVOKABLE static QString removeUrlFragment(const QString & string);

    Q_INVOKABLE static QString removeFileExtension(const QUrl    & url);
    Q_INVOKABLE static QString removeFileExtension(const QString & string);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static QString extractIpBase(const QString & string);
    Q_INVOKABLE static QString extractIpPort(const QString & string);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static void appendUrls(QStringList   * list,
                                       const QString & text,
                                       const QString & tag,
                                       const QString & attribute,
                                       const QString & baseUrl,
                                       const QString & pattern = QString());

    Q_INVOKABLE static QStringList extractUrls(const QString & text,
                                               const QString & tag,
                                               const QString & attribute,
                                               const QString & baseUrl,
                                               const QString & pattern = QString());

    Q_INVOKABLE static QString extractUrlAt(const QString & text,
                                            const QString & baseUrl, int index);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static QString getUrlRedirect(const QUrl & origin, const QUrl & redirect);

    Q_INVOKABLE static QUrl getUrlOAuth(const QUrl       & url,
                                        const QByteArray & httpMethod,
                                        const QByteArray & key,
                                        const QByteArray & secret,
                                        const QByteArray & token = QByteArray());

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static QString htmlToUtf8(const QString & string);

    Q_INVOKABLE static QString & fixHtml(QString & text);

    Q_INVOKABLE static QString extractHtml(const QString & text);

    Q_INVOKABLE static QString extractHead(const QString & html);

    Q_INVOKABLE static QString extractCharset(const QString & html);

    Q_INVOKABLE static QString extractTitle(const QString & head);
    Q_INVOKABLE static QString extractImage(const QString & head);

    Q_INVOKABLE static QString extractEntityAt(const QString & text, int   at);
    Q_INVOKABLE static QString extractEntityAt(const QString & text, int * index);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static int indexNode(const QString & text, const QString & pattern, int from = 0);

    Q_INVOKABLE static int indexNodeBegin(const QString & text, int at);

    Q_INVOKABLE static QString extractNodeAt(const QString & text,
                                             const QString & pattern, int at);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static QString extractNodeAttribute(const QString & text,
                                                    const QString & pattern,
                                                    const QString & attribute);

    Q_INVOKABLE static QString extractNodeAttributeAt(const QString & text,
                                                      const QString & attribute, int at);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static int indexValue(const QString & text, int from = 0);

    Q_INVOKABLE static QString extractValue(const QString & text, int from = 0);

    Q_INVOKABLE static QString extractValue(const QString & text,
                                            const QString & name, int from = 0);

    Q_INVOKABLE static QString extractValueAt(const QString & text, int at);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static QString extractValueUtf8(const QString & text, int from = 0);

    Q_INVOKABLE static QString extractValueUtf8(const QString & text,
                                                const QString & name, int from = 0);

    Q_INVOKABLE static QString extractValueUtf8At(const QString & text, int at);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static QString & fixAttributes(QString & text);

    Q_INVOKABLE static int indexAttribute(const QString & text,
                                          const QString & attribute, int from = 0);

    Q_INVOKABLE static QString extractAttribute(const QString & text,
                                                const QString & attribute, int from = 0);

    Q_INVOKABLE static QString extractAttributeAt(const QString & text, int at);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static QString extractAttributeUtf8(const QString & text,
                                                    const QString & attribute, int from = 0);

    Q_INVOKABLE static QString extractAttributeUtf8At(const QString & text, int at);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static int indexJson(const QString & text, const QString & attribute,
                                     int             from = 0);

    Q_INVOKABLE static int indexJsonEnd(const QString & text, int at);

    Q_INVOKABLE static QString extractJson(const QString & text);

    Q_INVOKABLE static QString extractJson(const QString & text,
                                           const QString & attribute, int from = 0);

    Q_INVOKABLE static QString extractJsonAt(const QString & text, int at);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static QString extractJsonUtf8(const QString & text);

    Q_INVOKABLE static QString extractJsonUtf8(const QString & text,
                                               const QString & attribute, int from = 0);

    Q_INVOKABLE static QString extractJsonUtf8At(const QString & text, int at);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static QString extractJsonHtml(const QString & text);

    Q_INVOKABLE static QString extractJsonHtml(const QString & text,
                                               const QString & attribute, int from = 0);

    Q_INVOKABLE static QString extractJsonHtmlAt(const QString & text, int at);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static QStringList splitJson(const QString & text, int from = 0);

    Q_INVOKABLE static QString stripJson(const QString & text,
                                         const QString & attribute, int from = 0);

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE static QString extractScript(const QString & text,
                                             const QString & name, int from = 0);

    Q_INVOKABLE static QString extractScriptHtml(const QString & text,
                                                 const QString & name, int from = 0);

signals:
    void connectedChanged(bool connected);

    void checkConnectedChanged(bool check);

public: // Properties
    bool isConnected() const;

    bool isCheckingConnected() const;
    void setCheckConnected(bool check);

private:
    W_DECLARE_PRIVATE   (WControllerNetwork)
    W_DECLARE_CONTROLLER(WControllerNetwork)

    Q_PRIVATE_SLOT(d_func(), void onStateChanged(QAbstractSocket::SocketState))

    Q_PRIVATE_SLOT(d_func(), void onSocketTimeout())
    Q_PRIVATE_SLOT(d_func(), void onRetryTimeout ())

    friend class WPagePrivate;
};

#include <private/WControllerNetwork_p>

#endif // SK_NO_CONTROLLERNETWORK
#endif // WCONTROLLERNETWORK_H
