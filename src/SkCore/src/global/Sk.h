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

#ifndef SK_H_
#define SK_H_

#include <QtGlobal>

class WPrivate;

//-------------------------------------------------------------------------------------------------
// Defines

#if defined(SK_CORE_LIBRARY)
#   define SK_CORE_EXPORT Q_DECL_EXPORT
#else
#   define SK_CORE_EXPORT Q_DECL_IMPORT
#endif

#if defined(SK_GUI_LIBRARY)
#   define SK_GUI_EXPORT Q_DECL_EXPORT
#else
#   define SK_GUI_EXPORT Q_DECL_IMPORT
#endif

#if defined(SK_MEDIA_LIBRARY)
#   define SK_MEDIA_EXPORT Q_DECL_EXPORT
#else
#   define SK_MEDIA_EXPORT Q_DECL_IMPORT
#endif

#if defined(SK_BARCODE_LIBRARY)
#   define SK_BARCODE_EXPORT Q_DECL_EXPORT
#else
#   define SK_BARCODE_EXPORT Q_DECL_IMPORT
#endif

#if defined(SK_MULTIMEDIA_LIBRARY)
#   define SK_MULTIMEDIA_EXPORT Q_DECL_EXPORT
#else
#   define SK_MULTIMEDIA_EXPORT Q_DECL_IMPORT
#endif

#if defined(SK_WEB_LIBRARY)
#   define SK_WEB_EXPORT Q_DECL_EXPORT
#else
#   define SK_WEB_EXPORT Q_DECL_IMPORT
#endif

#if defined(SK_TORRENT_LIBRARY)
#   define SK_TORRENT_EXPORT Q_DECL_EXPORT
#else
#   define SK_TORRENT_EXPORT Q_DECL_IMPORT
#endif

#if defined(SK_BACKEND_LIBRARY)
#   define SK_BACKEND_EXPORT Q_DECL_EXPORT
#else
#   define SK_BACKEND_EXPORT Q_DECL_IMPORT
#endif

#if defined(SK_PLUGIN_LIBRARY)
#  define SK_PLUGIN_EXPORT Q_DECL_EXPORT
#else
#  define SK_PLUGIN_EXPORT Q_DECL_IMPORT
#endif

//-------------------------------------------------------------------------------------------------

#define W_DECLARE_PRIVATE_COPY(Class)                       \
                                                            \
inline Class##Private * d_func()                            \
{                                                           \
    return reinterpret_cast<Class##Private *> (sk_d);       \
}                                                           \
                                                            \
inline const Class##Private * d_func() const                \
{                                                           \
    return reinterpret_cast<const Class##Private *> (sk_d); \
}                                                           \
                                                            \
friend class Class##Private;                                \

#define W_DECLARE_PRIVATE(Class) \
                                 \
Q_DISABLE_COPY        (Class)    \
W_DECLARE_PRIVATE_COPY(Class)    \

//-------------------------------------------------------------------------------------------------

#define C_STR toLatin1().constData()
#define C_UTF toUtf8  ().constData()

#define C_URL toString().C_STR

#ifdef QT_OLD
#define W_FOREACH(value, list) foreach (value, list)
#else
// NOTE: C++11 is required for this.
#define W_FOREACH(value, list) for (value : list)
#endif

#ifdef QT_4
#define qInfo qWarning
#endif

//-------------------------------------------------------------------------------------------------
// Torrent

#ifdef SK_NO_TORRENT
#define SK_NO_CONTROLLERTORRENT
#define SK_NO_LOADERTORRENT
#define SK_NO_BACKENDTORRENT
#define SK_NO_HOOKTORRENT
#define SK_NO_TORRENTENGINE
#endif

//-------------------------------------------------------------------------------------------------

/**
 * \internal
 * @class WPrivatable
 */
class SK_CORE_EXPORT WPrivatable
{
protected:
    WPrivatable(WPrivate * p);

    virtual ~WPrivatable();

    WPrivate * sk_d;
};

#endif // SK_H_
