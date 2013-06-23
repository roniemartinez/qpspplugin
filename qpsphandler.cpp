/*
Copyright (c) 2013 Ronie Martinez (ronmarti18@gmail.com)
All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Lesser General Public License for more
details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301  USA
*/


#include "qpsphandler.h"
#include <QDebug>
#include <QSize>


QPspHandler::QPspHandler()
{
}

QPspHandler::~QPspHandler()
{
}

bool QPspHandler::canRead() const
{
    if (canRead(device()))
        return true;
    return false;
}

bool QPspHandler::canRead(QIODevice *device)
{
    return device->peek(32) == QByteArray::fromHex("5061696e742053686f702050726f20496d6167652046696c650a1a0000000000");
}

bool QPspHandler::read(QImage *image)
{
    QDataStream input(device());
    input.setByteOrder(QDataStream::LittleEndian);
    input.setFloatingPointPrecision(QDataStream::DoublePrecision);

    QByteArray signature, headerIdentifier, compositeData;
    quint32 totalBlockLength, chunkSize, imageWidth, imageHeight, colorCount, totalImageSize,
            activeLayer, graphicContents;
    quint16 majorVersion, minorVersion, blockIdentifier, compressionType, bitDepth,
            planeCount, layerCount;
    qreal resolutionValue;
    quint8 resolutionMetric, greyscaleFlag;

    /* PSP File Header */
    {
        signature.resize(32);
        input.readRawData(signature.data(), 32);
        if (signature != QByteArray::fromHex("5061696e742053686f702050726f20496d6167652046696c650a1a0000000000"))
            return false;
        input >> majorVersion >> minorVersion;
        qDebug() << "major version:" << majorVersion << endl
                 << "minor version:" << minorVersion << endl;
    }

    /* General Image Attributes Block */
    {
        headerIdentifier.resize(4);
        input.readRawData(headerIdentifier.data(), 4);
        if (headerIdentifier != QByteArray::fromHex("7e424b00"))
            return false;
        input >> blockIdentifier;
        if (blockIdentifier != PSP_IMAGE_BLOCK)
            return false;
        /* PSP5SPEC: Initial data chunk length, 4 bytes */
        if (majorVersion == 3) {
            input >> chunkSize >> totalBlockLength;
        } else {
            input >> totalBlockLength >> chunkSize;
        }

        if (totalBlockLength != chunkSize)
            return false;

        input >> imageWidth;
        input >> imageHeight;
        input >> resolutionValue;
        input >> resolutionMetric;
        if (resolutionMetric > PSP_METRIC_CM)
            return false;
        input >> compressionType;
        if (compressionType >= PSP_COMP_JPEG)
            return false;
        input >> bitDepth;
        switch (bitDepth) { //must be 1, 4, 8, 24, or 48
        case 1:
        case 4:
        case 8:
        case 24:
        case 48:
            break;
        default:
            return false;
            break;
        }
        input >> planeCount;
        if (planeCount != 1)
            return false;
        input >> colorCount;
        if (colorCount != qPow(2, bitDepth))
            return false;
        input >> greyscaleFlag;
        if (greyscaleFlag != 0)
            if (greyscaleFlag != 1)
                return false;
        input >> totalImageSize;
        input >> activeLayer;
        input >> layerCount;

        if (majorVersion != 3) {
            input >> graphicContents;
            quint32 tempLength = 46;

            /* Expansion field
             * This field is currently non-existent, but robust readers should assume that
             * data may be added here in the future, and silently ignore it.
             */
            while (tempLength < chunkSize) {
                quint8 temp;
                input >> temp;
                tempLength += 1;
            }
        }

        qDebug() << "chunk size:" << chunkSize << endl
                 << "image width:" << imageWidth << endl
                 << "image height:" << imageHeight << endl
                 << "resolution value:" << resolutionValue << endl
                 << "resolution metric:" << resolutionMetric << endl
                 << "compression type:" << compressionType << endl
                 << "bit depth:" << bitDepth << endl
                 << "plane count:" << planeCount << endl
                 << "color count:" << colorCount << endl
                 << "greyscale flag:" << greyscaleFlag << endl
                 << "total image size:" << totalImageSize << endl
                 << "active layer:" << activeLayer << endl
                 << "layer count:" << layerCount;
        if (majorVersion != 3)
                 qDebug() << "graphic contents:" << graphicContents;
        qDebug() << endl;
    }

    /* Look for PSP_COMPOSITE_IMAGE_BANK_BLOCK or PSP_LAYER_START_BLOCK */
    bool end = false;
    while (!end) {
        input.readRawData(headerIdentifier.data(), 4);
        if (headerIdentifier != QByteArray::fromHex("7e424b00")) {
            return false;
        }
        input >> blockIdentifier;
        switch (blockIdentifier) {
        case PSP_COMPOSITE_IMAGE_BANK_BLOCK: /* The Composite Image Bank Block */
        {
            quint32 compositeImageBankBlockLength, length, compositeImageCount;
            input >> compositeImageBankBlockLength >> chunkSize;
            length = chunkSize;
            input >> compositeImageCount;

            /* Composite Image Attributes Sub-Block */
            quint32 compositeWidth, compositeHeight, compositeColorCount;
            quint16 compositeBitDepth, compositeCompressionType, compositePlaneCount,
                    compositeImageType;
            for (quint32 i = 0; i < compositeImageCount; ++i) {
                input.readRawData(headerIdentifier.data(), 4);
                if (headerIdentifier != QByteArray::fromHex("7e424b00"))
                    return false;
                input >> blockIdentifier;
                if (blockIdentifier != PSP_COMPOSITE_ATTRIBUTES_BLOCK)
                    return false;
                if (majorVersion == 3) {
                    input >> totalBlockLength >> chunkSize;
                    length += 14 + totalBlockLength;
                } else  {
                    input >> chunkSize >> totalBlockLength;
                    length += 10 + totalBlockLength;
                }
                input >> compositeWidth;
                input >> compositeHeight;
                input >> compositeBitDepth;
                switch (compositeBitDepth) { //must be 1, 4, 8, 24, or 48
                case 1:
                case 4:
                case 8:
                case 24:
                case 48:
                    break;
                default:
                    return false;
                    break;
                }
                input >> compositeCompressionType;
                if (compositeCompressionType > PSP_COMP_JPEG)
                    return false;
                input >> compositePlaneCount;
                if (compositePlaneCount != 1)
                    return false;
                input >> compositeColorCount;
                if (compositeColorCount != qPow(2, compositeBitDepth))
                    return false;
                input >> compositeImageType;
                quint32 tempLength = 24;
                /* Expansion field
                 * This field is currently non-existent, but robust readers should assume that
                 * data may be added here in the future, and silently ignore it.
                 */
                while (tempLength < chunkSize) {
                    quint8 temp;
                    input >> temp;
                    tempLength += 1;
                }
                if (compositeImageType == PSP_IMAGE_COMPOSITE
                        || (compositeImageCount == 1 && compositeImageType == PSP_IMAGE_THUMBNAIL)) {
                    qDebug() << "composite width:" << compositeWidth << endl
                             << "composite height:" << compositeHeight << endl
                             << "composite bit depth:" << compositeBitDepth << endl
                             << "composite compression type:" << compositeCompressionType << endl
                             << "composite plane count:" << compositePlaneCount << endl
                             << "composite color count:" << compositeColorCount << endl
                             << "composite image type:" << compositeImageType << endl;
                }
            }

            /* Composite Image Sub-Block */
            for (quint32 i = 0; i < compositeImageCount; ++i) {
                input.readRawData(headerIdentifier.data(), 4);
                if (headerIdentifier != QByteArray::fromHex("7e424b00"))
                    return false;
                input >> blockIdentifier;
                if (compositeCompressionType == PSP_COMP_JPEG) {
                    if (blockIdentifier != PSP_JPEG_BLOCK)
                        return false;
                } else {
                    if (blockIdentifier != PSP_COMPOSITE_IMAGE_BLOCK)
                        return false;
                }
                input >> totalBlockLength;
                length += 10 + totalBlockLength;
                quint32 compressedImageSize, uncompressedImageSize;
                quint16 imageType;
                input >> chunkSize >> compressedImageSize >> uncompressedImageSize >> imageType;
                compositeData.resize(compressedImageSize);
                input.readRawData(compositeData.data(), compressedImageSize);
                if (imageType == PSP_DIB_COMPOSITE ||
                        (compositeImageCount == 1 && imageType == PSP_DIB_THUMBNAIL)) {
                        qDebug() << "data size:" << compositeData.size() << endl
                                 << "compressed image size:" << compressedImageSize << endl
                                 << "uncompressed image size:" << uncompressedImageSize << endl
                                 << "image type:" << imageType << endl;
                        switch (compositeCompressionType) {
                        case PSP_COMP_JPEG:
                        {
                            *image = QImage::fromData(compositeData);
                            if (!image->isNull()) {
                                return true;
                            } else {
                                qDebug() << "null";
                                return false;
                            }
                        }
                            break;
                        default:
                            qDebug() << "composite compression type" << compositeCompressionType << endl;
                            break;
                        }
                    }
            }

            if (compositeImageBankBlockLength != length)
                return false;
        }
            end = true;
            break;
        case PSP_LAYER_START_BLOCK: /* NOT YET IMPLEMENTED */
            end = true;
            break;
        default:
        {
            if (majorVersion == 3) {
                input >> chunkSize;
            }
            input >> totalBlockLength;
            input.skipRawData(totalBlockLength);
        }
            break;
        }
        if (input.atEnd())
            end = true;
    }

    if (input.status() != QDataStream::Ok)
        return false;
    return true;
}

/* NOT YET IMPLEMENTED */
bool QPspHandler::supportsOption(ImageOption option) const
{
    Q_UNUSED(option);
    return false;
}

/* NOT YET IMPLEMENTED */
QVariant QPspHandler::option(ImageOption option) const
{
    Q_UNUSED(option);
    return QVariant();
}
