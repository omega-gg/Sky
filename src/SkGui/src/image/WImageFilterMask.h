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

#ifndef WIMAGEFILTERMASK_H
#define WIMAGEFILTERMASK_H

// Sk includes
#include <WImageFilter>

#ifndef SK_NO_IMAGEFILTERMASK

// Forward declarations
class WImageFilterMaskPrivate;

class SK_GUI_EXPORT WImageFilterMask : public WImageFilter
{
    Q_OBJECT

    Q_PROPERTY(int width  READ width  WRITE setWidth  NOTIFY widthChanged)
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)

    Q_PROPERTY(int radius READ radius WRITE setRadius NOTIFY radiusChanged)

    // NOTE: Whether we want an exact match between the image and the filter sizes to apply the
    //       mask. False by default.
    Q_PROPERTY(bool exactMatch READ exactMatch WRITE setExactMatch NOTIFY exactMatchChanged)

    Q_PROPERTY(int maxCache READ maxCache WRITE setMaxCache NOTIFY maxCacheChanged)

public:
    explicit WImageFilterMask(QObject * parent = NULL);

protected: // WImageFilter implementation
    /* virtual */ bool filter(QImage * image, qreal ratio);

signals:
    void widthChanged ();
    void heightChanged();

    void radiusChanged();

    void exactMatchChanged();

    void maxCacheChanged();

public: // Properties
    int  width() const;
    void setWidth(int width);

    int  height() const;
    void setHeight(int height);

    int  radius() const;
    void setRadius(int radius);

    bool exactMatch() const;
    void setExactMatch(bool exact);

    int  maxCache() const;
    void setMaxCache(int max);

private:
    W_DECLARE_PRIVATE(WImageFilterMask)
};

#endif // SK_NO_IMAGEFILTERMASK
#endif // WIMAGEFILTERMASK_H
