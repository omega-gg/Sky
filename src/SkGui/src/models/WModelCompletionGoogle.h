//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
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

public: // QAbstractItemModel reimplementation
    /* virtual */ QHash<int, QByteArray> roleNames() const;

    /* virtual */ int rowCount(const QModelIndex & parent = QModelIndex()) const;

    /* virtual */ QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

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
