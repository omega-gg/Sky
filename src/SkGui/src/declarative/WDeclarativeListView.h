//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WDECLARATIVELISTVIEW_H
#define WDECLARATIVELISTVIEW_H

// Sk includes
#include <WDeclarativeItem>

#ifndef SK_NO_DECLARATIVELISTVIEW

// Forward declarations
class WDeclarativeListViewPrivate;
class QAbstractItemModel;

//-------------------------------------------------------------------------------------------------
// WDeclarativeListView
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeListView : public WDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(QAbstractItemModel * model READ model WRITE setModel NOTIFY modelChanged)

#ifdef QT_4
    Q_PROPERTY(QDeclarativeComponent * delegate READ delegate WRITE setDelegate
               NOTIFY delegateChanged)
#else
    Q_PROPERTY(QQmlComponent * delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
#endif

    Q_PROPERTY(int count READ count NOTIFY countChanged)

    Q_PROPERTY(qreal itemSize READ itemSize WRITE setItemSize NOTIFY itemSizeChanged)

    Q_PROPERTY(qreal start READ start WRITE setStart RESET resetStart NOTIFY startChanged)
    Q_PROPERTY(qreal end   READ end   WRITE setEnd   RESET resetEnd   NOTIFY endChanged)

    Q_PROPERTY(bool clearDelayed READ clearDelayed WRITE setClearDelayed
               NOTIFY clearDelayedChanged)

    Q_PROPERTY(int clearDelay READ clearDelay WRITE setClearDelay NOTIFY clearDelayChanged)

protected:
#ifdef QT_4
    WDeclarativeListView(Qt::Orientation orientation, QDeclarativeItem * parent = NULL);
#else
    WDeclarativeListView(Qt::Orientation orientation, QQuickItem * parent = NULL);
#endif

public: // Interface
    Q_INVOKABLE void setRange(qreal start, qreal end);

#ifdef QT_4
    Q_INVOKABLE QDeclarativeItem * itemAt(int index) const;
#else
    Q_INVOKABLE QQuickItem * itemAt(int index) const;
#endif

    Q_INVOKABLE qreal itemY(int index) const;

signals:
    void modelChanged   ();
    void delegateChanged();

    void orientationChanged();

    void countChanged();

    void sizeChanged    ();
    void itemSizeChanged();

    void startChanged();
    void endChanged  ();

    void clearDelayedChanged();
    void clearDelayChanged  ();

public: // Properties
    QAbstractItemModel * model() const;
    void                 setModel(QAbstractItemModel * model);

#ifdef QT_4
    QDeclarativeComponent * delegate() const;
    void                    setDelegate(QDeclarativeComponent * delegate);
#else
    QQmlComponent * delegate() const;
    void            setDelegate(QQmlComponent * delegate);
#endif

    int count() const;

    qreal itemSize() const;
    void  setItemSize(qreal size);

    qreal start() const;
    void  setStart(qreal start);
    void  resetStart();

    qreal end() const;
    void  setEnd(qreal end);
    void  resetEnd();

    bool clearDelayed() const;
    void setClearDelayed(bool delayed);

    int  clearDelay() const;
    void setClearDelay(int delay);

private:
    W_DECLARE_PRIVATE(WDeclarativeListView)

    Q_PRIVATE_SLOT(d_func(), void onReset())

    Q_PRIVATE_SLOT(d_func(), void onInsert(const QModelIndex &, int, int))
    Q_PRIVATE_SLOT(d_func(), void onRemove(const QModelIndex &, int, int))

    Q_PRIVATE_SLOT(d_func(), void onMove(const QModelIndex &, int, int, const QModelIndex &, int))

    Q_PRIVATE_SLOT(d_func(), void onDataChanged(const QModelIndex &, const QModelIndex &))

    Q_PRIVATE_SLOT(d_func(), void onLayoutChanged())

    Q_PRIVATE_SLOT(d_func(), void onSizeChanged())

    Q_PRIVATE_SLOT(d_func(), void onClearItemsOld())
};

//-------------------------------------------------------------------------------------------------
// WDeclarativeListHorizontal
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeListHorizontal : public WDeclarativeListView
{
    Q_OBJECT

public:
#ifdef QT_4
    explicit WDeclarativeListHorizontal(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeListHorizontal(QQuickItem * parent = NULL);
#endif

private:
    Q_DISABLE_COPY(WDeclarativeListHorizontal)
};

//-------------------------------------------------------------------------------------------------
// WDeclarativeListVertical
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WDeclarativeListVertical : public WDeclarativeListView
{
    Q_OBJECT

public:
#ifdef QT_4
    explicit WDeclarativeListVertical(QDeclarativeItem * parent = NULL);
#else
    explicit WDeclarativeListVertical(QQuickItem * parent = NULL);
#endif

private:
    Q_DISABLE_COPY(WDeclarativeListVertical)
};

#include <private/WDeclarativeListView_p>

#endif // SK_NO_DECLARATIVELISTVIEW
#endif // WDECLARATIVELISTVIEW_H
