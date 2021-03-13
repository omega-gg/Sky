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

#ifndef WMODELCOMPLETIONGOOGLE_H
#define WMODELCOMPLETIONGOOGLE_H

// Qt includes
#include <QAbstractListModel>

// Sk includes
#include <Sk>

#ifndef SK_NO_MODELCOMPLETIONGOOGLE

// Forward declarations
class WModelCompletionGooglePrivate;
class WRemoteData;

class SK_GUI_EXPORT WModelCompletionGoogle : public QAbstractListModel, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(QUrl url READ url NOTIFY urlChanged)

    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)

    Q_PROPERTY(bool isLoading READ isLoading NOTIFY loadingChanged)

public: // Enums
    enum CompletionRoles
    {
        RoleTitle = Qt::UserRole + 1,
    };

public:
    explicit WModelCompletionGoogle(QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE bool runQuery();

    Q_INVOKABLE void resetQuery();

    Q_INVOKABLE void addQueryItem(const QString & key, const QString & value);

    Q_INVOKABLE QString getCompletionAt(int index) const;

public: // QAbstractItemModel implementation
    /* virtual */ int rowCount(const QModelIndex & parent = QModelIndex()) const;

    /* virtual */ QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

public: // QAbstractItemModel reimplementation
    /* virtual */ QHash<int, QByteArray> roleNames() const;

signals:
    void urlChanged();

    void queryChanged();

    void loadingChanged();

    void queryCompleted();

public: // Properties
    QUrl url() const;

    QString query() const;
    void    setQuery(const QString & query);

    bool isLoading() const;

private:
    W_DECLARE_PRIVATE(WModelCompletionGoogle)

    Q_PRIVATE_SLOT(d_func(), void onLoaded(WRemoteData *))
};

#include <private/WModelCompletionGoogle_p>

#endif // SK_NO_MODELCOMPLETIONGOOGLE
#endif // WMODELCOMPLETIONGOOGLE_H
