//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkGui module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WDECLARATIVECONTEXTUALPAGE_H
#define WDECLARATIVECONTEXTUALPAGE_H

// Qt includes
#include <QObject>
#include <QSizeF>
#ifdef QT_4
#include <QDeclarativeListProperty>
#else
#include <QQmlListProperty>
#endif

// Sk includes
#include <Sk>

#ifndef SK_NO_DECLARATIVECONTEXTUALPAGE

// Forward declarations
class WDeclarativeContextualPagePrivate;
class WDeclarativeContextualItem;

//-------------------------------------------------------------------------------------------------
// WContextualPageWatcher
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WContextualPageWatcher
{
protected:
    virtual void beginItemsInsert(int first, int last); // {}
    virtual void endItemsInsert  (); // {}

    virtual void itemUpdated(int index); // {}

    virtual void beginItemsClear(); // {}
    virtual void endItemsClear  (); // {}

    virtual void currentIndexChanged(int index); // {}

    virtual void contextualPageDestroyed(); // {}

private:
    friend class WDeclarativeContextualPage;
    friend class WDeclarativeContextualPagePrivate;
};

class SK_GUI_EXPORT WDeclarativeContextualPage : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_ENUMS(ItemType)

    Q_PROPERTY(QVariantList values READ values WRITE setValues NOTIFY valuesChanged)

    Q_PROPERTY(int currentId READ currentId WRITE setCurrentId NOTIFY currentIdChanged)

    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIdChanged)

public: // Enums
    enum ItemType
    {
        Item,
        ItemCover,
        ItemConfirm,
        Category
    };

public:
    explicit WDeclarativeContextualPage(QObject * parent = NULL);

public: // Interface
    void registerWatcher  (WContextualPageWatcher * watcher);
    void unregisterWatcher(WContextualPageWatcher * watcher);

    //---------------------------------------------------------------------------------------------
    // Items

    Q_INVOKABLE void addItem(ItemType        type,
                             int             id,
                             const QString & title,
                             const QString & icon     = QString(),
                             const QSizeF  & iconSize = QSizeF(),
                             bool            visible  = true,
                             bool            enabled  = true);

    Q_INVOKABLE void insertItem(int             index,
                                ItemType        type,
                                int             id,
                                const QString & title,
                                const QString & icon     = QString(),
                                const QSizeF  & iconSize = QSizeF(),
                                bool            visible  = true,
                                bool            enabled  = true);

    Q_INVOKABLE void set(int index, const QVariant & variant);

    Q_INVOKABLE void setItemVisible(int index, bool visible);
    Q_INVOKABLE void setItemEnabled(int index, bool enabled);

    Q_INVOKABLE void clearItems();

    Q_INVOKABLE int count() const;

    Q_INVOKABLE void selectPrevious();
    Q_INVOKABLE void selectNext    ();

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE int idAt(int index) const;

    Q_INVOKABLE int indexOf(const WDeclarativeContextualItem * item) const;

    Q_INVOKABLE int indexFromId(int id) const;

    //---------------------------------------------------------------------------------------------

    Q_INVOKABLE WDeclarativeContextualItem getItemAt    (int index) const;
    Q_INVOKABLE WDeclarativeContextualItem getItemFromId(int id)    const;

    Q_INVOKABLE const WDeclarativeContextualItem * itemAt    (int index) const;
    Q_INVOKABLE const WDeclarativeContextualItem * itemFromId(int id)    const;

    Q_INVOKABLE const WDeclarativeContextualItem * currentItemPointer() const;

signals:
    void valuesChanged(const QVariantList & values);

    void currentIdChanged();

public: // Properties
    QVariantList values() const;
    void         setValues(const QVariantList & values);

    int  currentId() const;
    void setCurrentId(int id);

    int  currentIndex() const;
    void setCurrentIndex(int index);

private:
    W_DECLARE_PRIVATE(WDeclarativeContextualPage)
};

//-------------------------------------------------------------------------------------------------
// WDeclarativeContextualItem
//-------------------------------------------------------------------------------------------------

class WDeclarativeContextualItem
{
public:
    WDeclarativeContextualItem();

public: // Interface
    bool isValid() const;

public: // Variables
    WDeclarativeContextualPage::ItemType type;

    int id;

    QString title;

    QString icon;
    QSizeF  iconSize;

    bool visible;
    bool enabled;
};

#endif // SK_NO_DECLARATIVECONTEXTUALPAGE
#endif // WDECLARATIVECONTEXTUALPAGE_H
