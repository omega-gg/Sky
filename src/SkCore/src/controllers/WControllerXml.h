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

#ifndef WCONTROLLERXML_H
#define WCONTROLLERXML_H

// Qt includes
#include <QDomDocument>
#include <QUrl>

// Sk includes
#include <WController>

#ifndef SK_NO_CONTROLLERXML

// Forward declarations
class QXmlStreamReader;
class QDateTime;
class WControllerXmlPrivate;

// Defines
#define wControllerXml WControllerXml::instance()

class SK_CORE_EXPORT WControllerXml : public WController
{
    Q_OBJECT

private:
    WControllerXml();

protected: // Initialize
    /* virtual */ void init();

public: // Interface
    void createQueryThread();

    int doQuery(const QString & query);

public: // Static Interface
    static bool loadFromFile(QDomDocument * document,
                             const QString & fileName, QString * errorMsg = 0,
                             int * errorLine = 0, int * errorColumn = 0);

    //---------------------------------------------------------------------------------------------
    // Getting content

    // Element from name
    static QDomElement firstElementFromName(const QDomDocument & document,
                                            const QString      & name);

    // Element value
    static QString firstElementValue(const QDomDocument & document,
                                     const QString      & name);

    // Attributes
    static QDomNamedNodeMap firstElementAttributes(const QDomDocument & document,
                                                   const QString      & name);

    static QString firstElementAttribute(const QDomDocument & document,
                                         const QString      & name,
                                         const QString      & attribute);

    //---------------------------------------------------------------------------------------------

    // Element from name
    static QDomElement firstElementFromName(const QDomElement & element,
                                            const QString     & name);

    // Element value
    static QString firstElementValue(const QDomElement & element,
                                     const QString     & name);

    // Attributes
    static QDomNamedNodeMap firstElementAttributes(const QDomElement & element,
                                                   const QString     & name);

    static QString firstElementAttribute(const QDomElement & element,
                                         const QString     & name,
                                         const QString     & attribute);

    //---------------------------------------------------------------------------------------------
    // QXmlStreamReader

    static bool readNextStartElement(QXmlStreamReader * stream, const QString & name);

    static int    readNextInt  (QXmlStreamReader * stream);
    static uint   readNextUInt (QXmlStreamReader * stream);
    static qint64 readNextInt64(QXmlStreamReader * stream);

    static float  readNextFloat (QXmlStreamReader * stream);
    static double readNextDouble(QXmlStreamReader * stream);

    static QString readNextString(QXmlStreamReader * stream);

    static QUrl readNextUrl(QXmlStreamReader * stream);

    static QDateTime readNextDate(QXmlStreamReader * stream);

signals:
    void queryCompleted(int id, const QString & result);

private:
    W_DECLARE_PRIVATE   (WControllerXml)
    W_DECLARE_CONTROLLER(WControllerXml)
};

#endif // SK_NO_CONTROLLERXML
#endif // WCONTROLLERXML_H
