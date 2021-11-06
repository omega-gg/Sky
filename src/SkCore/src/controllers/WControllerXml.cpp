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

#include "WControllerXml.h"

#ifndef SK_NO_CONTROLLERXML

// Qt includes
#include <QFile>
#include <QXmlStreamReader>
#include <QDateTime>

// Sk incudes
#include <WControllerApplication>

W_INIT_CONTROLLER(WControllerXml)

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/WController_p>

class WControllerXmlPrivate : public WControllerPrivate
{
public:
    WControllerXmlPrivate(WControllerXml * p);

    /* virtual */ ~WControllerXmlPrivate();

    void init();

protected:
    W_DECLARE_PUBLIC(WControllerXml)
};

//-------------------------------------------------------------------------------------------------

WControllerXmlPrivate::WControllerXmlPrivate(WControllerXml * p) : WControllerPrivate(p) {}

/* virtual */ WControllerXmlPrivate::~WControllerXmlPrivate()
{
    W_CLEAR_CONTROLLER(WControllerXml);
}

//-------------------------------------------------------------------------------------------------

void WControllerXmlPrivate::init() {}

//=================================================================================================
// WControllerXml
//=================================================================================================
// Private

WControllerXml::WControllerXml() : WController(new WControllerXmlPrivate(this)) {}

//-------------------------------------------------------------------------------------------------
// Initialize
//-------------------------------------------------------------------------------------------------

/* virtual */ void WControllerXml::init()
{
    Q_D(WControllerXml); d->init();
}

//-------------------------------------------------------------------------------------------------
// Static functions
//-------------------------------------------------------------------------------------------------

/* static */ bool WControllerXml::loadFromFile(QDomDocument * document,
                                               const QString & fileName, QString * errorMsg,
                                               int * errorLine, int * errorColumn)
{
    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly) == false) return false;

    return document->setContent(file.readAll(), errorMsg, errorLine, errorColumn);
}

//-------------------------------------------------------------------------------------------------

/* static */ QDomElement WControllerXml::firstElementFromName(const QDomDocument & document,
                                                              const QString      & name)
{
    QDomNodeList nodes = document.elementsByTagName(name);

    if (nodes.isEmpty())
    {
         return QDomElement();
    }
    else return nodes.at(0).toElement();
}

/* static */ QString WControllerXml::firstElementValue(const QDomDocument & document,
                                                       const QString      & name)
{
    QDomElement node = firstElementFromName(document, name);

    return node.text();
}

/* static */ QDomNamedNodeMap WControllerXml::firstElementAttributes(const QDomDocument & document,
                                                                     const QString      & name)
{
    QDomNode node = firstElementFromName(document, name);

    return node.attributes();
}

/* static */ QString WControllerXml::firstElementAttribute(const QDomDocument & document,
                                                           const QString      & name,
                                                           const QString      & attribute)
{
    QDomNamedNodeMap attributes = firstElementAttributes(document, name);

    if (attributes.contains(attribute))
    {
         return attributes.namedItem(attribute).nodeValue();
    }
    else return QString();
}

//-------------------------------------------------------------------------------------------------

/* static */ QDomElement WControllerXml::firstElementFromName(const QDomElement & element,
                                                              const QString     & name)
{
    QDomNodeList nodes = element.elementsByTagName(name);

    if (nodes.isEmpty())
    {
         return QDomElement();
    }
    else return nodes.at(0).toElement();
}

//-------------------------------------------------------------------------------------------------

/* static */ QString WControllerXml::firstElementValue(const QDomElement & element,
                                                       const QString     & name)
{
    QDomElement node = firstElementFromName(element, name);

    return node.text();
}

/* static */ QDomNamedNodeMap WControllerXml::firstElementAttributes(const QDomElement & element,
                                                                     const QString     & name)
{
    QDomNode node = firstElementFromName(element, name);

    return node.attributes();
}

/* static */ QString WControllerXml::firstElementAttribute(const QDomElement & element,
                                                           const QString     & name,
                                                           const QString     & attribute)
{
    QDomNamedNodeMap attributes = firstElementAttributes(element, name);

    if (attributes.contains(attribute))
    {
         return attributes.namedItem(attribute).nodeValue();
    }
    else return QString();
}

//---------------------------------------------------------------------------------------------
// QXmlStreamReader

/* static */ bool WControllerXml::readNextStartElement(QXmlStreamReader * stream,
                                                       const QString    & name)
{
    while (stream->atEnd() == false)
    {
        QXmlStreamReader::TokenType type = stream->readNext();

        if (type == QXmlStreamReader::StartElement && stream->name() == name)
        {
            return true;
        }
    }

    return false;
}

//---------------------------------------------------------------------------------------------

/* static */ int WControllerXml::readNextInt(QXmlStreamReader * stream)
{
    stream->readNext();

    return stream->text().toString().toInt();
}

/* static */ uint WControllerXml::readNextUInt(QXmlStreamReader * stream)
{
    stream->readNext();

    return stream->text().toString().toUInt();
}

/* static */ qint64 WControllerXml::readNextInt64(QXmlStreamReader * stream)
{
    stream->readNext();

    return stream->text().toString().toLongLong();
}

//---------------------------------------------------------------------------------------------

/* static */ float WControllerXml::readNextFloat(QXmlStreamReader * stream)
{
    stream->readNext();

    return stream->text().toString().toFloat();
}

/* static */ double WControllerXml::readNextDouble(QXmlStreamReader * stream)
{
    stream->readNext();

    return stream->text().toString().toDouble();
}

//---------------------------------------------------------------------------------------------

/* static */ QString WControllerXml::readNextString(QXmlStreamReader * stream)
{
    stream->readNext();

    return stream->text().toString();
}

//---------------------------------------------------------------------------------------------

/* static */ QUrl WControllerXml::readNextUrl(QXmlStreamReader * stream)
{
    stream->readNext();

    return QUrl::fromEncoded(stream->text().toUtf8());
}

//---------------------------------------------------------------------------------------------

/* static */ QDateTime WControllerXml::readNextDate(QXmlStreamReader * stream)
{
    qint64 seconds = readNextInt64(stream);

    return Sk::dateFromSecs(seconds);
}

#endif // SK_NO_CONTROLLERXML
