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

#include "WCrypter.h"

#ifndef SK_NO_CRYPTER

// Qt includes
#include <QCryptographicHash>
#include <QDataStream>
#include <QVector>

//-------------------------------------------------------------------------------------------------
// Private
//-------------------------------------------------------------------------------------------------

#include <private/Sk_p>

class SK_CORE_EXPORT WCrypterPrivate : public WPrivate
{
public:
    WCrypterPrivate(WCrypter * p);

    void init(quint64 key);

public: // Functions
    void splitKey();

public: // Variables
    quint64 key;

    WCrypter::CompressionMode compression;
    WCrypter::ProtectionMode  protection;

    QVector<char> keyParts;

protected:
    W_DECLARE_PUBLIC(WCrypter)
};

//-------------------------------------------------------------------------------------------------

WCrypterPrivate::WCrypterPrivate(WCrypter * p) : WPrivate(p) {}

void WCrypterPrivate::init(quint64 key)
{
    this->key = key;

    compression = WCrypter::CompressionAuto;
    protection  = WCrypter::ProtectionChecksum;

    if (key) splitKey();
}

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------

void WCrypterPrivate::splitKey()
{
    keyParts.clear();

    keyParts.resize(8);

    for (int i = 0; i < 8; i++)
    {
        quint64 part = key;

        for (int j = i; j > 0; j--)
        {
            part = part >> 8;
        }

        part = part & 0xff;

        keyParts[i] = static_cast<char> (part);
    }
}

//-------------------------------------------------------------------------------------------------
// Ctor / dtor
//-------------------------------------------------------------------------------------------------

/* explicit */ WCrypter::WCrypter(quint64 key) : WPrivatable(new WCrypterPrivate(this))
{
    Q_D(WCrypter); d->init(key);
}

//-------------------------------------------------------------------------------------------------
// Interface
//-------------------------------------------------------------------------------------------------

QString WCrypter::encryptToString(const QString & string) const
{
    return encryptToString(string.toUtf8());
}

QString WCrypter::encryptToString(const QByteArray & bytes) const
{
    QByteArray cypher = encryptToByteArray(bytes);

    return QString::fromLatin1(cypher.toBase64());
}

//-------------------------------------------------------------------------------------------------

QByteArray WCrypter::encryptToByteArray(const QString & string) const
{
    return encryptToByteArray(string.toUtf8());
}

//-------------------------------------------------------------------------------------------------

QByteArray WCrypter::encryptToByteArray(const QByteArray & bytes) const
{
    Q_D(const WCrypter);

    if (d->keyParts.isEmpty())
    {
        qWarning("WCrypter::encryptToByteArray: No key.");

        return QByteArray();
    }

    QByteArray array = bytes;

    CrypterFlags flags = CrypterNone;

    if (d->compression == CompressionAlways)
    {
        array = qCompress(array, 9);

        flags |= CrypterCompression;
    }
    else if (d->compression == CompressionAuto)
    {
        QByteArray compressed = qCompress(array, 9);

        if (compressed.count() < array.count())
        {
            array = compressed;

            flags |= CrypterCompression;
        }
    }

    QByteArray protection;

    if (d->protection == ProtectionChecksum)
    {
        flags |= CrypterChecksum;

        QDataStream s(&protection, QIODevice::WriteOnly);

        s << qChecksum(array, array.size());
    }
    else if (d->protection == ProtectionHash)
    {
        flags |= CrypterHash;

        QCryptographicHash hash(QCryptographicHash::Sha1);

        hash.addData(array);

        protection.append(hash.result());
    }

    array = char(qrand() & 0xFF) + protection + array;

    char lastChar = 0;

    for (int i = 0; i < array.count(); i++)
    {
        array[i] = array.at(i) ^ d->keyParts.at(i % 8) ^ lastChar;

        lastChar = array.at(i);
    }

    array.prepend(char(flags));
    array.prepend(char(3));

    return array;
}

//-------------------------------------------------------------------------------------------------

QString WCrypter::decryptToString(const QString & string) const
{
    QByteArray bytes = QByteArray::fromBase64(string.toLatin1());

    return decryptToString(bytes);
}

QString WCrypter::decryptToString(const QByteArray & bytes) const
{
    QByteArray array = decryptToByteArray(bytes);

    return QString::fromUtf8(array, array.size());
}

//-------------------------------------------------------------------------------------------------

QByteArray WCrypter::decryptToByteArray(const QString & string) const
{
    QByteArray bytes = QByteArray::fromBase64(string.toLatin1());

    return decryptToByteArray(bytes);
}

QByteArray WCrypter::decryptToByteArray(const QByteArray & bytes) const
{
    Q_D(const WCrypter);

    if (d->keyParts.isEmpty())
    {
        qWarning("WCrypter::decryptToByteArray: No key.");

        return QByteArray();
    }

    QByteArray array = bytes;

    char version = array.at(0);

    if (version != 3)
    {
        qWarning("WCrypter::decryptToByteArray: Invalid cypher version.");

        return QByteArray();
    }

    CrypterFlags flags = CrypterFlags(array.at(1));

    array = array.mid(2);

    char lastChar = 0;

    for (int i = 0; i < array.count(); i++)
    {
        char currentChar = array[i];

        array[i] = array.at(i) ^ lastChar ^ d->keyParts.at(i % 8);

        lastChar = currentChar;
    }

    array = array.mid(1);

    bool integrity = true;

    if (flags.testFlag(CrypterChecksum))
    {
        if (array.length() < 2) return QByteArray();

        quint16 storedChecksum;

        QDataStream s(&array, QIODevice::ReadOnly);

        s >> storedChecksum;

        array = array.mid(2);

        quint16 checksum = qChecksum(array, array.size());

        integrity = (checksum == storedChecksum);
    }
    else if (flags.testFlag(CrypterHash))
    {
        if (array.length() < 20) return QByteArray();

        QByteArray storedHash = array.left(20);

        array = array.mid(20);

        QCryptographicHash hash(QCryptographicHash::Sha1);

        hash.addData(array);

        integrity = (hash.result() == storedHash);
    }

    if (integrity == false) return QByteArray();

    if (flags.testFlag(CrypterCompression))
    {
        array = qUncompress(array);
    }

    return array;
}

//-------------------------------------------------------------------------------------------------
// Properties
//-------------------------------------------------------------------------------------------------

quint64 WCrypter::key() const
{
    Q_D(const WCrypter); return d->key;
}

void WCrypter::setKey(quint64 key)
{
    Q_D(WCrypter);

    if (d->key == key) return;

    d->key = key;

    d->splitKey();
}

//-------------------------------------------------------------------------------------------------

WCrypter::CompressionMode WCrypter::compressionMode() const
{
    Q_D(const WCrypter); return d->compression;
}

void WCrypter::setCompressionMode(CompressionMode compression)
{
    Q_D(WCrypter); d->compression = compression;
}

//-------------------------------------------------------------------------------------------------

WCrypter::ProtectionMode WCrypter::protectionMode() const
{
    Q_D(const WCrypter); return d->protection;
}

void WCrypter::setProtectionMode(ProtectionMode protection)
{
    Q_D(WCrypter); d->protection = protection;
}

#endif // SK_NO_CRYPTER
