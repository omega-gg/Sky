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

#ifndef WLOCALOBJECT_H
#define WLOCALOBJECT_H

// Qt includes
#include <QObject>

// Sk includes
#include <WAbstractThreadReply>

#ifndef SK_NO_LOCALOBJECT

// Forward declarations
class WLocalObjectPrivate;
class WAbstractThreadAction;

//=================================================================================================
// WLocalObject
//=================================================================================================

class SK_CORE_EXPORT WLocalObject : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_ENUMS(State)

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)

    Q_PROPERTY(bool isProcessing READ isProcessing NOTIFY processingChanged)

    Q_PROPERTY(bool isSaving READ isSaving NOTIFY savingChanged)

    Q_PROPERTY(State state READ state NOTIFY stateChanged)

    Q_PROPERTY(bool isDefault READ isDefault NOTIFY stateChanged)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY stateChanged)
    Q_PROPERTY(bool isLoaded  READ isLoaded  NOTIFY stateChanged)

    Q_PROPERTY(bool isLocked READ isLocked NOTIFY lockedChanged)

    Q_PROPERTY(bool saveEnabled READ saveEnabled WRITE setSaveEnabled NOTIFY saveEnabledChanged)

    Q_PROPERTY(int lockCount READ lockCount NOTIFY lockCountChanged)

public: // Enums
    enum State { Default, Loading, Loaded };

public:
    explicit WLocalObject(QObject * parent = NULL);
protected:
    WLocalObject(WLocalObjectPrivate * p, QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE void createPath();

    Q_INVOKABLE bool saveNow();

    Q_INVOKABLE void abortSave();
    Q_INVOKABLE void abortLoad();

    Q_INVOKABLE void abortAll();

    Q_INVOKABLE void deleteFile  (bool instant = false);
    Q_INVOKABLE void deleteFolder(bool instant = false);

    Q_INVOKABLE void deleteFileAndFolder(bool instant = false);

    Q_INVOKABLE void addDeleteLock  ();
    Q_INVOKABLE void clearDeleteLock();

    Q_INVOKABLE QString pathFileFromId  (int id) const;
    Q_INVOKABLE QString pathFolderFromId(int id) const;

    Q_INVOKABLE QString pathChildFile  (int id) const;
    Q_INVOKABLE QString pathChildFolder(int id) const;

    Q_INVOKABLE void abortAndDelete();

public: // Virtual Interface
    Q_INVOKABLE virtual bool save(bool instant = false, int duration = 1000);
    Q_INVOKABLE virtual bool load(bool instant = false);

    Q_INVOKABLE virtual bool tryDelete();

    Q_INVOKABLE virtual QString getParentPath() const;

    Q_INVOKABLE virtual QString getFilePath  () const;
    Q_INVOKABLE virtual QString getFolderPath() const;

protected: // Functions
    void setBlocked(bool block);

    bool isCacheLoad() const;

protected: // Virtual functions
    virtual void applyId(int id);

    virtual void setSaved (bool ok);
    virtual void setLoaded(bool ok);

    virtual WAbstractThreadAction * onSave(const QString & path); // {}
    virtual WAbstractThreadAction * onLoad(const QString & path); // {}

    virtual void onStateChanged      (State state);  // {}
    virtual void onLockChanged       (bool locked);  // {}
    virtual void onSaveEnabledChanged(bool enabled); // {}

    virtual void onFileDeleted  (); // {}
    virtual void onFolderDeleted(); // {}

    virtual bool hasFolder() const; // {}

signals:
    void saved ();
    void loaded();

    void idChanged();

    void processingChanged();

    void stateChanged();

    void savingChanged();

    void saveEnabledChanged();

    void lockedChanged   ();
    void lockCountChanged();

public: // Properties
    int  id() const;
    void setId(int id);

    bool isProcessing() const;

    bool isSaving() const;

    State state() const;

    bool isDefault() const;
    bool isLoading() const;
    bool isLoaded () const;
    bool isInvalid() const;

    bool isLocked() const;

    bool saveEnabled() const;
    void setSaveEnabled(bool enabled);

    int lockCount() const;

private:
    W_DECLARE_PRIVATE(WLocalObject)

    Q_PRIVATE_SLOT(d_func(), void onSaveTimeout())

    friend class WLocalObjectReplySave;
    friend class WLocalObjectReplyLoad;
};

#include <private/WLocalObject_p>

//=================================================================================================
// WLocalObjectReplySave
//=================================================================================================

class SK_CORE_EXPORT WLocalObjectReplySave : public WAbstractThreadReply
{
    Q_OBJECT

public:
    WLocalObjectReplySave(WLocalObject * object);

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

public: // Variables
    WLocalObject * object;

private:
    Q_DISABLE_COPY(WLocalObjectReplySave)
};

//=================================================================================================
// WLocalObjectReplyLoad
//=================================================================================================

class SK_CORE_EXPORT WLocalObjectReplyLoad : public WAbstractThreadReply
{
    Q_OBJECT

public:
    WLocalObjectReplyLoad(WLocalObject * object);

protected: // WAbstractThreadReply reimplementation
    /* virtual */ void onCompleted(bool ok);

public: // Variables
    WLocalObject * object;

private:
    Q_DISABLE_COPY(WLocalObjectReplyLoad)
};

#endif // SK_NO_LOCALOBJECT
#endif // WLOCALOBJECT_H
