//=================================================================================================
/*
    Copyright (C) 2015-2016 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WSCRIPT_H
#define WSCRIPT_H

// Qt includes
#include <QObject>
#include <QScriptValue>

// Sk includes
#include <Sk>

#ifndef SK_NO_SCRIPT

class WScriptPrivate;

//-------------------------------------------------------------------------------------------------
// Defines

#define W_SCRIPT_DECLARE_QMETAOBJECT_ACCESSORS(myObject)                                   \
                                                                                           \
inline QScriptValue myObject##ToScriptValue(QScriptEngine * engine, myObject * const & in) \
{                                                                                          \
    return engine->newQObject(in);                                                         \
}                                                                                          \
                                                                                           \
inline void myObject##FromScriptValue(const QScriptValue & object, myObject * & out)       \
{                                                                                          \
    out = qobject_cast<myObject *> (object.toQObject());                                   \
}                                                                                          \

//-------------------------------------------------------------------------------------------------

#define W_SCRIPT_REGISTER_QMETAOBJECT(engine, myObject)                                 \
                                                                                        \
    engine->globalObject().setProperty(#myObject,                                       \
                                       qScriptValueFromQMetaObject<myObject> (engine)); \

#define W_SCRIPT_REGISTER_QMETAOBJECT_ACCESSORS(engine, myObject)                        \
                                                                                         \
    qScriptRegisterMetaType(engine, myObject##ToScriptValue, myObject##FromScriptValue); \

#define W_SCRIPT_REGISTER_QMETAOBJECT_CTOR(engine, myObject)               \
                                                                           \
    QScriptValue myObject##Ctor = engine->newFunction(myObject##_ctor);    \
                                                                           \
    QScriptValue myObject##MetaObject                                      \
    = engine->newQMetaObject(&myObject::staticMetaObject, myObject##Ctor); \
                                                                           \
    engine->globalObject().setProperty(#myObject, myObject##MetaObject);   \

//-------------------------------------------------------------------------------------------------

#define W_SCRIPT_DECLARE_QMETAOBJECT_CTOR1(T, _Arg1)                             \
                                                                                 \
template<> inline QScriptValue                                                   \
qscriptQMetaObjectConstructor<T>(QScriptContext * ctx, QScriptEngine * eng, T *) \
{                                                                                \
    if (ctx->isCalledAsConstructor() == false)                                   \
    {                                                                            \
        return eng->undefinedValue();                                            \
    }                                                                            \
                                                                                 \
    _Arg1 arg1 = qscriptvalue_cast<_Arg1> (ctx->argument(0));                    \
                                                                                 \
    T * t = new T(arg1);                                                         \
                                                                                 \
    return eng->newQObject(t, QScriptEngine::QtOwnership);                       \
}                                                                                \

//-------------------------------------------------------------------------------------------------

#define W_DECLARE_QOBJECT(object, argument)        \
                                                   \
    W_SCRIPT_DECLARE_QMETAOBJECT_ACCESSORS(object) \

#define W_REGISTER_QOBJECT(engine, object)                  \
                                                            \
    W_SCRIPT_REGISTER_QMETAOBJECT_ACCESSORS(engine, object) \

//-------------------------------------------------------------------------------------------------

#define W_SCRIPT_CHECK_EQUALS                                                                     \
                                                                                                  \
    if (ctx->thisObject().strictlyEquals(ctx->engine()->globalObject()))                          \
    {                                                                                             \
        return ctx->throwError(QString::fromLatin1("%1: Did you forget to construct with 'new'?") \
                               .arg(ctx->toString()));                                            \
    }                                                                                             \

#define W_SCRIPT_NEW_QOBJECT(object)                         \
                                                             \
    eng->newQObject(new object, QScriptEngine::QtOwnership); \

//-------------------------------------------------------------------------------------------------

class SK_CORE_EXPORT WScript : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_PROPERTY(QString fileName READ fileName)

public:
    WScript(QScriptEngine * engine = 0, QObject * parent = NULL);
protected:
    WScript(WScriptPrivate * p, QScriptEngine * engine = 0, QObject * parent = NULL);

public: // Interface
    virtual bool load(const QString & fileName);

    virtual void setContent(const QString & content);

signals:
    void scriptUpdated();

protected: // Interface
    QScriptValue construct(const QString          & ctorName,
                           const QScriptValueList & parameters = QScriptValueList());

    QScriptValue construct(const QString & ctorName, QObject * param);

public: // Properties
    QString fileName() const;

private:
    W_DECLARE_PRIVATE(WScript)
};

#endif // SK_NO_SCRIPT
#endif // WSCRIPT_H
