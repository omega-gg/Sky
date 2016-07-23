//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#include "WControllerXml.h"

#ifndef SK_NO_CONTROLLERXML

// Qt includes
#include <QFile>
#include <QThread>
#include <QMutex>
#include <QXmlQuery>
#include <QXmlStreamReader>
#include <QDateTime>

// Sk incudes
#include <WControllerScript>

W_INIT_CONTROLLER(WControllerXml)

//=================================================================================================
// WControllerXmlQuery
//=================================================================================================

struct XmlQueryJob
{
    int  id;
    QUrl query;
};

class WControllerXmlQuery : public QObject
{
    Q_OBJECT

public:
    explicit WControllerXmlQuery(QObject * parent = NULL);

    /* virtual */ ~WControllerXmlQuery();

public: // Functions
    int doQuery(const QUrl & query);

    void abort(int id);

signals:
    void queryCompleted(int id, const QString & result);

private slots:
    void processQuery(int id);

private: // Functions
    QString doQueryJob(XmlQueryJob * currentJob);

private:
    QMutex mutex;

    QThread * thread;

    QHash<int, XmlQueryJob> jobs;

    int queryId;
};

//-------------------------------------------------------------------------------------------------

/* explicit */ WControllerXmlQuery::WControllerXmlQuery(QObject * parent) : QObject(parent)
{
    thread = new QThread(this);

    queryId = 1;

    moveToThread(thread);

    thread->start(QThread::IdlePriority);
}

/* virtual */ WControllerXmlQuery::~WControllerXmlQuery()
{
    thread->quit();
    thread->wait();
}

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

int WControllerXmlQuery::doQuery(const QUrl & query)
{
    XmlQueryJob job;

    job.id    = queryId;
    job.query = query;

    QMutexLocker locker(&mutex);

    jobs.insert(queryId, job);

    queryId++;

    if (queryId <= 0) queryId = 1;

    QMetaObject::invokeMethod(this, "processQuery", Qt::QueuedConnection, Q_ARG(int, job.id));

    return job.id;
}

void WControllerXmlQuery::abort(int id)
{
    QMutexLocker locker(&mutex);

    if (id != -1) jobs.remove(id);
}

//-------------------------------------------------------------------------------------------------
// Private slots
//-------------------------------------------------------------------------------------------------

void WControllerXmlQuery::processQuery(int id)
{
    XmlQueryJob job;

    QMutexLocker locker(&mutex);

    if (jobs.contains(id) == false) return;

    job = jobs.take(id);

    QString result = doQueryJob(&job);

    emit queryCompleted(job.id, result);
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

QString WControllerXmlQuery::doQueryJob(XmlQueryJob * currentJob)
{
    Q_ASSERT(currentJob->id != -1);

    QString result;

    QXmlQuery query;

    query.setQuery(currentJob->query);

    query.evaluateTo(&result);

    return result;
}

//=================================================================================================
// WControllerXmlPrivate
//=================================================================================================

#include <private/WController_p>

class WControllerXmlPrivate : public WControllerPrivate
{
public:
    WControllerXmlPrivate(WControllerXml * p);

    /* virtual */ ~WControllerXmlPrivate();

    void init();

public: // Variables
    WControllerXmlQuery * xmlQuery;

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

void WControllerXmlPrivate::init()
{
    xmlQuery = NULL;
}

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
// Interface
//-------------------------------------------------------------------------------------------------

void WControllerXml::createQueryThread()
{
    Q_D(WControllerXml);

    if (d->xmlQuery) return;

    d->xmlQuery = new WControllerXmlQuery(this);

    QObject::connect(d->xmlQuery, SIGNAL(queryCompleted(int, QString)),
                     this,        SIGNAL(queryCompleted(int, QString)));
}

int WControllerXml::doQuery(const QUrl & query)
{
    Q_D(WControllerXml);

    createQueryThread();

    return d->xmlQuery->doQuery(query);
}

//-------------------------------------------------------------------------------------------------
// Static interface
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

    if (nodes.isEmpty()) return QDomElement();

    return nodes.at(0).toElement();
}

/* static */ QString WControllerXml::firstElementValue(const QDomDocument & document,
                                                       const QString      & name)
{
    QDomElement node = firstElementFromName(document, name);

    if (node.isNull()) return QString();

    return node.text();
}

/* static */ QDomNamedNodeMap WControllerXml::firstElementAttributes(const QDomDocument & document,
                                                                     const QString      & name)
{
    QDomNode node = firstElementFromName(document, name);

    if (node.isNull())                 return QDomNamedNodeMap();
    if (node.hasAttributes() == false) return QDomNamedNodeMap();

    return node.attributes();
}

/* static */ QString WControllerXml::firstElementAttribute(const QDomDocument & document,
                                                           const QString      & name,
                                                           const QString      & attribute)
{
    QDomNamedNodeMap attributes = firstElementAttributes(document, name);

    if (attributes.contains(attribute) == false)
    {
        return QString();
    }
    else return attributes.namedItem(attribute).nodeValue();
}

//-------------------------------------------------------------------------------------------------

/* static */ QDomElement WControllerXml::firstElementFromName(const QDomElement & element,
                                                              const QString     & name)
{
    QDomNodeList nodes = element.elementsByTagName(name);

    if (nodes.isEmpty()) return QDomElement();

    return nodes.at(0).toElement();
}

//-------------------------------------------------------------------------------------------------

/* static */ QString WControllerXml::firstElementValue(const QDomElement & element,
                                                       const QString     & name)
{
    QDomElement node = firstElementFromName(element, name);

    if (node.isNull()) return QString();

    return node.text();
}

/* static */ QDomNamedNodeMap WControllerXml::firstElementAttributes(const QDomElement & element,
                                                                     const QString     & name)
{
    QDomNode node = firstElementFromName(element, name);

    if (node.isNull())                 return QDomNamedNodeMap();
    if (node.hasAttributes() == false) return QDomNamedNodeMap();

    return node.attributes();
}

/* static */ QString WControllerXml::firstElementAttribute(const QDomElement & element,
                                                           const QString     & name,
                                                           const QString     & attribute)
{
    QDomNamedNodeMap attributes = firstElementAttributes(element, name);

    if (attributes.contains(attribute) == false)
    {
        return QString();
    }
    else return attributes.namedItem(attribute).nodeValue();
}

//---------------------------------------------------------------------------------------------
// QXmlStreamReader

bool WControllerXml::readNextStartElement(QXmlStreamReader * stream, const QString & name)
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

/* static */ QDateTime WControllerXml::readNextDate(QXmlStreamReader * stream)
{
    uint time = readNextUInt(stream);

    if (time)
    {
         return QDateTime::fromTime_t(time);
    }
    else return QDateTime();
}

#endif // SK_NO_CONTROLLERXML

#include "WControllerXml.moc"
