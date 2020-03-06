//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkCore module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WCRYPTER_H
#define WCRYPTER_H

// Sk includes
#include <WControllerApplication>

class WCrypterPrivate;

class SK_CORE_EXPORT WCrypter : public WPrivatable
{
public: // Enums
    enum CompressionMode
    {
        CompressionAuto,
        CompressionAlways,
        CompressionNever
    };

    enum ProtectionMode
    {
        ProtectionNone,
        ProtectionChecksum,
        ProtectionHash
    };

    enum CrypterFlag
    {
        CrypterNone        = 0,
        CrypterCompression = 0x1,
        CrypterChecksum    = 0x2,
        CrypterHash        = 0x4
    };
    Q_DECLARE_FLAGS(CrypterFlags, CrypterFlag)

public:
    explicit WCrypter(quint64 key = 0);

public: // Interface
    QString encryptToString(const QString    & string) const;
    QString encryptToString(const QByteArray & bytes)  const;

    QByteArray encryptToByteArray(const QString    & string) const;
    QByteArray encryptToByteArray(const QByteArray & bytes)  const;

    //---------------------------------------------------------------------------------------------

    QString decryptToString(const QString    & string) const;
    QString decryptToString(const QByteArray & bytes)  const;

    QByteArray decryptToByteArray(const QString    & string) const;
    QByteArray decryptToByteArray(const QByteArray & bytes)  const;

public: // Properties
    quint64 key() const;
    void    setKey(quint64 key);

    CompressionMode compressionMode() const;
    void            setCompressionMode(CompressionMode compression);

    ProtectionMode protectionMode() const;
    void           setProtectionMode(ProtectionMode protection);

private:
    W_DECLARE_PRIVATE(WCrypter)
};

#endif // WCRYPTER_H
