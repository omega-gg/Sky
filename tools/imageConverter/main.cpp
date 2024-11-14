//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the tools for Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

// Qt includes
#include <QCoreApplication>
#include <QPainter>
#include <QImage>

//-------------------------------------------------------------------------------------------------
// Global variables

QString input;
QString output;

QString format = "png";

int quality = -1;

int width  = -1;
int height = -1;

int margin = 0;

QString effect = "none";

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

QImage desaturate(const QImage & image)
{
    QImage result(image.width(), image.height(), image.format());

    for (int y = 0; y < image.height(); y++)
    {
        const QRgb * lineA = (QRgb *) image.scanLine(y);

        QRgb * lineB = (QRgb *) result.scanLine(y);

        for (int x = 0; x < image.width(); x++)
        {
            const QRgb & color = *lineA;

            int average = (qRed(color) + qGreen(color) + qBlue(color)) / 3;

            *lineB = qRgba(average, average, average, qAlpha(color));

            lineA++;
            lineB++;
        }
    }

    return result;
}

//-------------------------------------------------------------------------------------------------

void scale(QImage * image, int margins)
{
    if (width != -1)
    {
        if (height == -1)
        {
            int size = width - margins;

            *image = image->scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        else *image = image->scaled(width - margins, height - margins, Qt::KeepAspectRatio,
                                                                       Qt::SmoothTransformation);
    }
    else if (height != -1)
    {
        if (width == -1)
        {
            int size = height - margins;

            *image = image->scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        else *image = image->scaled(width - margins, height - margins, Qt::KeepAspectRatio,
                                                                       Qt::SmoothTransformation);
    }
}

//-------------------------------------------------------------------------------------------------

void convert()
{
    qDebug("Converting: %s", qPrintable(input));

    QImage image(input);

    if (image.isNull())
    {
        qWarning("convert: Invalid image.");

        return;
    }

    if (margin)
    {
        int margins = margin * 2;

        scale(&image, margins);

        QImage content(image.width () + margins,
                       image.height() + margins, QImage::Format_ARGB32);

        content.fill(Qt::transparent);

        QPainter painter(&content);

        painter.drawImage(margin, margin, image);

        image = content;
    }
    else scale(&image, 0);

    if (effect == "desaturate")
    {
        image = desaturate(image);
    }

    if (image.save(output, qPrintable(format), quality) == false)
    {
        qWarning("convert: Failed to save image.");
    }
}

//-------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);

    if (argc > 2 && argc < 10)
    {
        input  = argv[1];
        output = argv[2];

        if (argc > 3) format = argv[3];

        if (argc > 4) quality = QString(argv[4]).toInt();

        if (argc > 5) width  = QString(argv[5]).toInt();
        if (argc > 6) height = QString(argv[6]).toInt();

        if (argc > 7) margin = QString(argv[7]).toInt();

        if (argc > 8) effect = argv[8];
    }
    else
    {
        qDebug("Usage: imageConverter <path input> <path output> [format (png)] [quality (-1)] "
               "[width (-1)] [height (-1)] [margin (0)] [effect (none)]");

        return 0;
    }

    qDebug("Sky image converter");

    qDebug("Input: %s", qPrintable(input));
    qDebug("Ouput: %s", qPrintable(output));

    qDebug("Format: %s", qPrintable(format));

    qDebug("Quality: %d", quality);

    qDebug("Width: %d",  width);
    qDebug("Height: %d", height);

    qDebug("Margin: %d", margin);

    qDebug("Effect: %s", qPrintable(effect));

    qDebug("\nCONVERTING");

    convert();

    qDebug("DONE");

    return 0;
}
