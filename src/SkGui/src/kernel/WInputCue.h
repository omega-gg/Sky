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

#ifndef WINPUTCUE_H
#define WINPUTCUE_H

// Qt includes
#include <QObject>

// Sk includes
#include <Sk>

#ifndef SK_NO_INPUTCUE

// Forward declarations
class WInputCuePrivate;
class WView;

//-------------------------------------------------------------------------------------------------
// WInputCueAction
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WInputCueAction
{
public:
    WInputCueAction();
protected:
    virtual ~WInputCueAction();

public: // Abstract interface
    virtual void run() = 0;

public: // Variables
    int delay;

private:
    friend class WInputCue;
    friend class WInputCuePrivate;
};

//-------------------------------------------------------------------------------------------------
// WInputCue
//-------------------------------------------------------------------------------------------------

class SK_GUI_EXPORT WInputCue : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(WView * view READ view WRITE setView NOTIFY viewChanged)

public:
    explicit WInputCue(QObject * parent = NULL);

public: // Interface
    Q_INVOKABLE void push  (WInputCueAction * action);
    Q_INVOKABLE void remove(WInputCueAction * action);

    Q_INVOKABLE void pushId(int id, int delay);

    Q_INVOKABLE void wait(int delay);

    Q_INVOKABLE void clear();

    //---------------------------------------------------------------------------------------------
    // View

    Q_INVOKABLE void mouseMove(int delay, int x, int y, Qt::MouseButton button = Qt::NoButton);

    Q_INVOKABLE void mousePress  (int delay, Qt::MouseButton button = Qt::LeftButton);
    Q_INVOKABLE void mouseRelease(int delay, Qt::MouseButton button = Qt::LeftButton);

    Q_INVOKABLE void mouseClick(int delay, Qt::MouseButton button = Qt::LeftButton,
                                           int             msec   = 100);

    Q_INVOKABLE void wheel(int delay, Qt::Orientation orientation = Qt::Vertical,
                                      int             delta       = -120);

    Q_INVOKABLE void wheelUp  (int delay, int delta =  120);
    Q_INVOKABLE void wheelDown(int delay, int delta = -120);

    Q_INVOKABLE void keyPress(int delay, int                   key,
                                         Qt::KeyboardModifiers modifiers = Qt::NoModifier);

    Q_INVOKABLE void keyRelease(int delay, int                   key,
                                           Qt::KeyboardModifiers modifiers = Qt::NoModifier);

    Q_INVOKABLE void keyClick(int delay, int                   key,
                                         Qt::KeyboardModifiers modifiers = Qt::NoModifier,
                                         int                   msec      = 100);

    //---------------------------------------------------------------------------------------------
    // QML

    Q_INVOKABLE void mouseMove(int delay, int x, int y, int button);

    Q_INVOKABLE void mousePress  (int delay, int button);
    Q_INVOKABLE void mouseRelease(int delay, int button);

    Q_INVOKABLE void mouseClick(int delay, int button, int msec = 100);

    Q_INVOKABLE void wheel(int delay, int orientation, int delta = -120);

    Q_INVOKABLE void keyPress  (int delay, int key, int modifiers);
    Q_INVOKABLE void keyRelease(int delay, int key, int modifiers);

    Q_INVOKABLE void keyClick(int delay, int key, int modifiers, int msec = 100);

signals:
    void run(int id);

    void viewChanged();

public: // Properties
    WView * view() const;
    void    setView(WView * view);

private:
    W_DECLARE_PRIVATE(WInputCue)

    Q_PRIVATE_SLOT(d_func(), void onFinished())

    friend class WInputCueId;
};

#include <private/WInputCue_p>

#endif // SK_NO_INPUTCUE
#endif // WINPUTCUE_H
