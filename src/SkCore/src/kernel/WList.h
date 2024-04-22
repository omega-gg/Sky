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

#ifndef WLIST_H
#define WLIST_H

// C++ includes
#include <list>

// Sk includes
#include <Sk>

#ifndef SK_NO_LIST

// NOTE Qt6: A WList is useful when we want to ensure that data pointers are updated on a move
//           operation. A QList will move the data between pointers (std::rotate). When using a
//           WList we're losing the QList implicit sharing. So it should be used when we have no
//           other choice.

// NOTE: You should use W_FOREACH instead of foreach to avoid copying the elements while looping.

template <typename T>
class WList : public std::list<T>
{
public: // Interface
    void append (const T & value);
    void prepend(const T & value);

    void insert(int index, const T & value);

    void move(int from, int to);

    void removeAt(int index);

    void removeFirst();
    void removeLast ();

    const T & at(int index) const;

    const T & first() const;
    const T & last () const;

    bool contains(const T & value) const;

    bool isEmpty() const;

    int count() const;

public: // Operators
    T & operator[](int index);
};

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

template<typename T>
void WList<T>::append(const T & value)
{
    this->push_back(value);
}

template<typename T>
void WList<T>::prepend(const T & value)
{
    insert(0, value);
}

template<typename T>
void WList<T>::insert(int index, const T & value)
{
    Q_ASSERT(index >= 0 && index <= (int) this->size());

    std::list<T>::insert(std::next(this->begin(), index), value);
}

template<typename T>
void WList<T>::move(int from, int to)
{
    Q_ASSERT(from >= 0 && from < (int) this->size()
             &&
             to >= 0 && to < (int) this->size());

    typename WList<T>::iterator itA = std::next(this->begin(), from);
    typename WList<T>::iterator itB = std::next(this->begin(), to);

    // NOTE: We are using splice instead of rotate because we want to move the actual pointer, not
    //       just the data.
    this->splice(itB, *this, itA);
}

template<typename T>
void WList<T>::removeAt(int index)
{
    Q_ASSERT(index >= 0 && index <= (int) this->size());

    this->erase(std::next(this->begin(), index));
}

template<typename T>
void WList<T>::removeFirst()
{
    removeAt(0);
}

template<typename T>
void WList<T>::removeLast()
{
    removeAt(this->size() - 1);
}

//-------------------------------------------------------------------------------------------------

template<typename T>
const T & WList<T>::at(int index) const
{
    Q_ASSERT(index >= 0 && index < (int) this->size());

    return *(std::next(this->begin(), index));
}

template<typename T>
const T & WList<T>::first() const
{
    return at(0);
}

template<typename T>
const T & WList<T>::last() const
{
    return at(this->size() - 1);
}

template<typename T>
bool WList<T>::contains(const T & value) const
{
    return (std::find(this->begin(), this->end(), value) != this->end());
}

template<typename T>
bool WList<T>::isEmpty() const
{
    return (this->size() == 0);
}

template<typename T>
int WList<T>::count() const
{
    return this->size();
}

//-------------------------------------------------------------------------------------------------
// Operators
//-------------------------------------------------------------------------------------------------

template<typename T>
T & WList<T>::operator[](int index)
{
    Q_ASSERT(index >= 0 && index < (int) this->size());

    return *(std::next(this->begin(), index));
}

#endif // SK_NO_LIST
#endif // WLIST_H
