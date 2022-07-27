//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors. <http://omega.gg/Sky>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of SkBarcode.

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

#ifndef WBARCODEWRITER_H
#define WBARCODEWRITER_H

// Qt includes
#include <QObject>
#include <QImage>

// Sk includes
#include <Sk>

// Forward declarations
class WAbstractThreadAction;
class WBarcodeWriterPrivate;

class SK_BARCODE_EXPORT WBarcodeWriter : public QObject, public WPrivatable
{
    Q_OBJECT

    Q_ENUMS(Type)

public: // Enums
    enum Type { Text, Vbml };

public:
    explicit WBarcodeWriter(QObject * parent = NULL);

public: // Static functions
    Q_INVOKABLE static QString encode(const QString & text,
                                      Type type = Text, const QString & prefix = QString());

    Q_INVOKABLE static QImage write(const QString & text,
                                    Type type = Text, const QString & prefix = QString());

    // NOTE: When providing a cover the generated tag is twice the size.
    Q_INVOKABLE static QImage writeTag(const QString & text,
                                       const QString & background,
                                       const QString & cover  = QString(),
                                       const QString & prefix = QString(),
                                       int             size   = 512,
                                       int             margin = 28);

    // NOTE: When providing a cover the generated tag is twice the size.
    Q_INVOKABLE static bool writeTagFile(const QString & fileName,
                                         const QString & text,
                                         const QString & background,
                                         const QString & cover  = QString(),
                                         const QString & prefix = QString(),
                                         int             size   = 512,
                                         int             margin = 28,
                                         const QString & format = "png");


    // NOTE: The 'method' format is complete(const QString &).
    Q_INVOKABLE static WAbstractThreadAction * startEncode(const QString & text,
                                                           QObject       * receiver,
                                                           const char    * method = NULL,
                                                           Type            type   = Text,
                                                           const QString & prefix = QString());

    // NOTE: The 'method' format is complete(const QImage &).
    Q_INVOKABLE static WAbstractThreadAction * startWrite(const QString & text,
                                                          QObject       * receiver,
                                                          const char    * method = NULL,
                                                          Type            type   = Text,
                                                          const QString & prefix = QString());

    // NOTE: The 'method' format is complete(const QImage &).
    Q_INVOKABLE static WAbstractThreadAction * startWriteTag(const QString & text,
                                                             const QString & background,
                                                             QObject       * receiver,
                                                             const char    * method = NULL,
                                                             const QString & cover  = QString(),
                                                             const QString & prefix = QString(),
                                                             int             size   = 512,
                                                             int             margin = 28);

    // NOTE: The 'method' format is complete(bool).
    Q_INVOKABLE static WAbstractThreadAction * startWriteTagFile(const QString & fileName,
                                                                 const QString & text,
                                                                 const QString & background,
                                                                 QObject       * receiver,
                                                                 const char    * method = NULL,
                                                                 const QString & cover  = QString(),
                                                                 const QString & prefix = QString(),
                                                                 int             size   = 512,
                                                                 int             margin = 28,
                                                                 const QString & format = "png");

    // NOTE: Returns a standardized VideoTag fileName.
    Q_INVOKABLE static QString getTagName(const QString & title,
                                          const QString & prefix = "VideoTag-");

    //---------------------------------------------------------------------------------------------
    // NOTE: These are useful to get rid of the black bars.

    Q_INVOKABLE static QImage generateCover(const QImage & image);

    Q_INVOKABLE static int getBarTop   (const QImage & image);
    Q_INVOKABLE static int getBarBottom(const QImage & image);

private:
    W_DECLARE_PRIVATE(WBarcodeWriter)
};

#endif // WBARCODEWRITER_H
