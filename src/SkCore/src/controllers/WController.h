//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WCONTROLLER_H
#define WCONTROLLER_H

// Qt includes
#include <QObject>

// Sk includes
#include <Sk>

// Forward declarations
class WControllerPrivate;

//-------------------------------------------------------------------------------------------------
// Defines

#define W_DECLARE_CONTROLLER(Class)   \
                                      \
public:                               \
static Class * instance()             \
{                                     \
    if (Class::_instance == NULL)     \
    {                                 \
        Class::_instance = new Class; \
    }                                 \
                                      \
    return Class::_instance;          \
}                                     \
                                      \
static Class * instancePointer()      \
{                                     \
    return Class::_instance;          \
}                                     \
                                      \
static void clearInstance()           \
{                                     \
    Class::_instance = NULL;          \
}                                     \
                                      \
private:                              \
static Class * _instance;             \

#define W_INIT_CONTROLLER(Class)     \
                                     \
    Class * Class::_instance = NULL; \

//-------------------------------------------------------------------------------------------------

#define W_CREATE_CONTROLLER(Class)       \
                                         \
    Class::instance()->initController(); \

#define W_CLEAR_CONTROLLER(Class) \
                                  \
    Class::clearInstance();       \

//-------------------------------------------------------------------------------------------------

#define W_GET_CONTROLLER(Class, Name)        \
                                             \
    Class * Name = Class::instancePointer(); \

#define W_CONTROLLER(Class, Name)     \
                                      \
    Class * Name = Class::instance(); \
                                      \
    Q_ASSERT(Name);                   \

//-------------------------------------------------------------------------------------------------

class SK_CORE_EXPORT WController : public QObject, public WPrivatable
{
    Q_OBJECT

protected:
    WController();
    WController(WControllerPrivate * p);

public: // Interface
    void initController();

protected: // Functions
    virtual void init();

private:
    W_DECLARE_PRIVATE(WController)
};

#endif // WCONTROLLER_H
