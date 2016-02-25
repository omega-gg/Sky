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

#ifndef WPLUGIN_H
#define WPLUGIN_H

// Qt includes
#include <QObject>

// Sk includes
#include <WPluginInterface.h>

// Forward declarations
class QDomDocument;
class WPluginPrivate;

class SK_CORE_EXPORT WPlugin : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(QString fileName READ fileName)

    Q_PROPERTY(bool loaded READ isLoaded)

    Q_PROPERTY(QObject * instance READ instance)

public:
    WPlugin(const QString & fileName, QObject * parent = NULL);

    explicit WPlugin(QObject * parent = NULL);

public: // Interface
    bool load  (const QString & fileName);
    bool unload();

    bool isLoaded() const;

    QString errorString () const;

    template <class T> T interface() const;

public: // Static functions
    static QString specFileName(const QString & pluginFileName);

    static QString nameFromPlugin  (const QString & fileName);
    static QString nameFromSpecFile(const QString & fileName);

protected: // Functions
    virtual bool loadSpecs(QDomDocument * specifications);

protected: // Events
    virtual void loadEvent  ();
    virtual void unloadEvent();

public: // Properties
    QString fileName() const;

    QObject * instance() const;

    QString name() const;

    QString version      () const;
    QString compatVersion() const;

    QString description() const;
    QString url        () const;

    QString license() const;

private:
    W_DECLARE_PRIVATE(WPlugin)
};

template <class T> T WPlugin::interface() const
{
    return qobject_cast<T> (instance());
}

#endif // WPLUGIN_H
