//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkBackend module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WBACKENDUNIVERSAL_P_H
#define WBACKENDUNIVERSAL_P_H

/*  W A R N I N G
    -------------

    This file is not part of the Sk API. It exists purely as an
    implementation detail. This header file may change from version to
    version without notice, or even be removed.

    We mean it.
*/

// Private includes
#include <private/WBackendNet_p>

#ifndef SK_NO_BACKENDUNIVERSAL

// Forward declarations
class WRemoteData;
class WBackendUniversalData;
class WBackendUniversalParameters;

//=================================================================================================
// WBackendUniversalData
//=================================================================================================

struct WBackendUniversalData
{
    WBackendUniversalData()
    {
        hasSearch = false;

        isSearchEngine = false;
        isSearchCover  = false;
    }

    bool hasSearch;

    bool isSearchEngine;
    bool isSearchCover;

    QString title;
    QString host;

    QList<WLibraryFolderItem> items;

    //---------------------------------------------------------------------------------------------

    QString validate;

    QString trackId;
    QString trackOutput;

    QString playlistInfo;

    QString urlTrack;
    QString urlPlaylist;

    QString querySource;
    QString queryTrack;
    QString queryPlaylist;
    QString queryFolder;
    QString queryItem;

    QString createQuery;
};

//=================================================================================================
// WBackendUniversalNode
//=================================================================================================

class SK_BACKEND_EXPORT WBackendUniversalNode
{
public: // Enums
    enum Type
    {
        Variable,
        Function
    };

public:
    WBackendUniversalNode(Type type);

public: // Interface
    QVariant run(WBackendUniversalParameters * parameters) const;

    void dump(int indent = 0) const;

    int   getInt  (WBackendUniversalParameters * parameters, int index) const;
    float getFloat(WBackendUniversalParameters * parameters, int index) const;

    QByteArray getByteArray(WBackendUniversalParameters * parameters, int index) const;
    QString    getString   (WBackendUniversalParameters * parameters, int index) const;

    QRegExp getRegExp(WBackendUniversalParameters * parameters, int index) const;

    QVariant * getValue(WBackendUniversalParameters * parameters, int index) const;

public: // Variables
    Type type;

    QString data;

    QList<WBackendUniversalNode> nodes;
};

//=================================================================================================
// WBackendUniversalScript
//=================================================================================================

class SK_BACKEND_EXPORT WBackendUniversalScript
{
public:
    WBackendUniversalScript(const QString & data);

public: // Interface
    bool isValid() const;

    QVariant run(WBackendUniversalParameters * parameters) const;

    void dump() const;

private: // Functions
    void load(const QString & data);

    bool loadParameters(WBackendUniversalNode * node,
                        QString               * string, const QRegExp & regExp) const;

    bool loadFunction(WBackendUniversalNode * node,
                      QString               * string, const QRegExp & regExp) const;

    QString extractWord  (QString * string) const;
    QString extractString(QString * string) const;

    bool skipCondition(int * index) const;

    bool getCondition(WBackendUniversalParameters * parameters,
                      const WBackendUniversalNode & node, int * index) const;

public: // Properties
    QList<WBackendUniversalNode> nodes;
};

//=================================================================================================
// WBackendUniversalParameters
//=================================================================================================

class SK_BACKEND_EXPORT WBackendUniversalParameters
{
public:
    WBackendUniversalParameters(const WBackendUniversalScript & script);

public: // Interface
    void add(const QString & name, const QVariant & value = QVariant());

    QVariant       * value     (const QString & name);
    const QVariant * valueConst(const QString & name) const;

private: // Functions
    void extract(QStringList * list, const WBackendUniversalNode & node);

public: // Variables
    QHash<QString, QVariant> parameters;
};

//=================================================================================================
// WBackendUniversalPrivate
//=================================================================================================

class SK_BACKEND_EXPORT WBackendUniversalPrivate : public WBackendNetPrivate
{
public:
    WBackendUniversalPrivate(WBackendUniversal * p);

    void init(const QString & id, const QString & source);

public: // Functions
    void load();

    void runQuery(WBackendNetQuery * query, const QString & source, const QString & url) const;

    void applyQueryParameters(WBackendUniversalParameters * parameters, const QString & url) const;

    void applyQuery(WBackendNetQuery * query, WBackendUniversalParameters * parameters) const;

public: // Events
    void onLoaded();

    void onData(const WBackendUniversalData & data);

public: // Variables
    QThread * thread;

    WBackendUniversalData data;

    WRemoteData * remote;

    QString id;
    QString source;

    QMetaMethod method;

protected:
    W_DECLARE_PUBLIC(WBackendUniversal)
};

#endif // SK_NO_BACKENDUNIVERSAL
#endif // WBACKENDUNIVERSAL_P_H
