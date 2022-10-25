//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkGui.

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

#ifndef WMODELOUTPUT_H
#define WMODELOUTPUT_H

// Qt includes
#include <QAbstractListModel>

// Sk includes
#include <WAbstractBackend>

#ifndef SK_NO_MODELOUTPUT

// Forward declarations
class WModelOutputPrivate;

class SK_GUI_EXPORT WModelOutput : public QAbstractListModel, public WBackendWatcher,
                                   public WPrivatable
{
    Q_OBJECT

    Q_ENUMS(OutputRoles)

    Q_PROPERTY(WAbstractBackend * backend READ backend WRITE setBackend NOTIFY backendChanged)

public: // Enums
    enum OutputRoles
    {
        RoleName = Qt::UserRole + 1,
        RoleLabel,
        RoleCurrent
    };

public:
    explicit WModelOutput(QObject * parent = NULL);

public: // QAbstractItemModel implementation
    /* virtual */ int rowCount(const QModelIndex & parent = QModelIndex()) const;

    /* virtual */ QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

public: // QAbstractItemModel reimplementation
    /* virtual */ QHash<int, QByteArray> roleNames() const;

protected: // WBackendWatcher implementation
    /* virtual */ void beginOutputInsert(int first, int last);
    /* virtual */ void endOutputInsert  ();

    /* virtual */ void beginOutputRemove(int first, int last);
    /* virtual */ void endOutputRemove  ();

    /* virtual */ void currentOutputChanged(int index);

    /* virtual */ void backendDestroyed();

signals:
    void backendChanged();

public: // Properties
    WAbstractBackend * backend() const;
    void               setBackend(WAbstractBackend * tabs);

private:
    W_DECLARE_PRIVATE(WModelOutput)
};

#endif // SK_NO_MODELOUTPUT
#endif // WMODELOUTPUT_H
