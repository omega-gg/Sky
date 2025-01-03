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

#ifndef WSUBTITLE_H
#define WSUBTITLE_H

// Qt includes
#include <QVariant>

// Sk includes
#include <Sk>

#ifndef SK_NO_SUBTITLE

class WSubtitlePrivate;

class SK_GUI_EXPORT WSubtitle : public WPrivatable
{
public:
    explicit WSubtitle(const QString & source = QString(),
                       const QString & title  = QString());

public: // Interface
    bool isValid() const;

public: // Virtual interface
    virtual QVariantMap toMap() const;

public: // Operators
    WSubtitle(const WSubtitle & other);

    bool operator==(const WSubtitle & other) const;

    WSubtitle & operator=(const WSubtitle & other);

public: // Properties
    QString source() const;
    void    setSource(const QString & source);

    QString title() const;
    void    setTitle(const QString & title);

private:
    W_DECLARE_PRIVATE_COPY(WSubtitle)
};

#endif // SK_NO_SUBTITLE
#endif // WSUBTITLE_H
