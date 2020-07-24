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

#ifndef WIMAGEFILTER_H
#define WIMAGEFILTER_H

// Qt includes
#include <QObject>
#include <QImage>

// Sk includes
#include <Sk>

#ifndef SK_NO_IMAGEFILTER

// Forward declarations
class WImageFilterPrivate;

class SK_GUI_EXPORT WImageFilter : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(bool filterDelay READ filterDelay WRITE setFilterDelay NOTIFY filterDelayChanged)

public:
    explicit WImageFilter(QObject * parent = NULL);
protected:
    WImageFilter(WImageFilterPrivate * p, QObject * parent = NULL);

public: // Interface
    bool applyFilter(QImage  * image);
    bool applyFilter(QPixmap * pixmap);

protected: // Abstract functions
    virtual bool filter(QImage * image) = 0;

protected slots:
    void refreshFilter();

signals:
    void filterUpdated();

    void filterDelayChanged();

public: // Properties
    int  filterDelay() const;
    void setFilterDelay(int delay);

private:
    W_DECLARE_PRIVATE(WImageFilter)
};

#endif // SK_NO_IMAGEFILTER
#endif // WIMAGEFILTER_H
