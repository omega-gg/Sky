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

#ifndef WMODELRANGE_H
#define WMODELRANGE_H

// Qt includes
#include <QObject>
#include <QSizeF>

// Sk includes
#include <Sk>

#ifndef SK_NO_MODELRANGE

// Forward declarations
class WModelRangePrivate;

class SK_GUI_EXPORT WModelRange : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation
               NOTIFY orientationChanged)

    Q_PROPERTY(qreal value    READ value    WRITE setValue    NOTIFY valueChanged)
    Q_PROPERTY(qreal position READ position WRITE setPosition NOTIFY positionChanged)

    Q_PROPERTY(int intValue    READ value    WRITE setValue    NOTIFY valueChanged)
    Q_PROPERTY(int intPosition READ position WRITE setPosition NOTIFY positionChanged)

    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)

    Q_PROPERTY(qreal singleStep READ singleStep WRITE setSingleStep NOTIFY singleStepChanged)
    Q_PROPERTY(qreal pageStep   READ pageStep   WRITE setPageStep   NOTIFY pageStepChanged)

    Q_PROPERTY(QSizeF range READ range WRITE setRange NOTIFY rangeChanged)

    Q_PROPERTY(qreal minimum READ minimum WRITE setMinimum NOTIFY rangeChanged)
    Q_PROPERTY(qreal maximum READ maximum WRITE setMaximum NOTIFY rangeChanged)

    Q_PROPERTY(QSizeF handleRange READ handleRange WRITE setHandleRange NOTIFY handleRangeChanged)

    Q_PROPERTY(qreal handleMinimum READ handleMinimum WRITE setHandleMinimum
               NOTIFY handleRangeChanged)

    Q_PROPERTY(qreal handleMaximum READ handleMaximum WRITE setHandleMaximum
               NOTIFY handleRangeChanged)

    Q_PROPERTY(bool atMinimum READ atMinimum NOTIFY handleChanged)
    Q_PROPERTY(bool atMaximum READ atMaximum NOTIFY handleChanged)

public:
    explicit WModelRange(QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE void setRange      (qreal minimum, qreal maximum);
    Q_INVOKABLE void setHandleRange(qreal minimum, qreal maximum);

    Q_INVOKABLE void scroll    (qreal steps);
    Q_INVOKABLE void pageScroll(qreal pageSteps);

    Q_INVOKABLE qreal valueToPosition(qreal value)    const;
    Q_INVOKABLE qreal positionToValue(qreal position) const;

signals:
    void orientationChanged();

    void valueChanged   ();
    void positionChanged();

    void scaleChanged();

    void singleStepChanged();
    void pageStepChanged  ();

    void rangeChanged      ();
    void handleRangeChanged();

    void handleChanged();

public: // Properties
    Qt::Orientation orientation() const;
    void            setOrientation(Qt::Orientation orientation);

    qreal value() const;
    void  setValue(qreal value);

    qreal position() const;
    void  setPosition(qreal position);

    qreal scale() const;
    void  setScale(qreal scale);

    qreal singleStep() const;
    void  setSingleStep(qreal singleStep);

    qreal pageStep() const;
    void  setPageStep(qreal pageStep);

    QSizeF range() const;
    void   setRange(const QSizeF & range);

    qreal minimum() const;
    void  setMinimum(qreal minimum);

    qreal maximum() const;
    void  setMaximum(qreal maximum);

    QSizeF handleRange() const;
    void   setHandleRange(const QSizeF & range);

    qreal handleMinimum() const;
    void  setHandleMinimum(qreal minimum);

    qreal handleMaximum() const;
    void  setHandleMaximum(qreal maximum);

    bool atMinimum() const;
    bool atMaximum() const;

private:
    W_DECLARE_PRIVATE(WModelRange)
};

#endif // SK_NO_MODELRANGE
#endif // WMODELRANGE_H
