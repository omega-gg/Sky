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

#ifndef WCHAPTER_H
#define WCHAPTER_H

// Qt includes
#include <QVariant>

// Sk includes
#include <Sk>

#ifndef SK_NO_CHAPTER

class WChapterPrivate;

class SK_GUI_EXPORT WChapter : public WPrivatable
{
public:
    explicit WChapter(int time = -1);

public: // Interface
    bool isValid() const;

public: // Virtual interface
    virtual QVariantMap toMap() const;

public: // Operators
    WChapter(const WChapter & other);

    bool operator==(const WChapter & other) const;

    WChapter & operator=(const WChapter & other);

public: // Properties
    int  time() const;
    void setTime(int msec);

    QString title() const;
    void    setTitle(const QString & title);

    QString cover() const;
    void    setCover(const QString & cover);

private:
    W_DECLARE_PRIVATE_COPY(WChapter)
};

#endif // SK_NO_CHAPTER
#endif // WCHAPTER_H
