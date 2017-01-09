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

    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)

public:
    explicit WImageFilter(QObject * parent = NULL);
protected:
    WImageFilter(WImageFilterPrivate * p, QObject * parent = NULL);

public: // Interface
    bool applyFilter(QImage  * image)  const;
    bool applyFilter(QPixmap * pixmap) const;

    Q_INVOKABLE void updateFilter();

protected: // Functions
    virtual bool filter(QImage * image) const = 0;

protected slots:
    void refreshFilter();

signals:
    void filterUpdated();

    void autoUpdateChanged();

public: // Properties
    bool autoUpdate() const;
    void setAutoUpdate(bool autoUpdate);

private:
    W_DECLARE_PRIVATE(WImageFilter)
};

#endif // SK_NO_IMAGEFILTER
#endif // WIMAGEFILTER_H
