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
        if (blockIdentifier != pspImageBlock)
            return false;
        input >> totalBlockLength >> chunkSize;
        quint32 tempLength;
        if (majorVersion < 12)
            tempLength = 0;
        else
            tempLength = 4;

        if (totalBlockLength != chunkSize)
            return false;
        if (tempLength < chunkSize) {//check if tempLength == chunksize
            input >> imageWidth;
            tempLength += 4;
        }
        if (tempLength < chunkSize) {//check if tempLength == chunksize
            input >> imageHeight;
            tempLength += 4;
        }
        if (tempLength < chunkSize) {//check if tempLength == chunksize
            input >> resolutionValue;
            tempLength += 8;
        }
        if (tempLength < chunkSize) {//check if tempLength == chunksize
            input >> resolutionMetric;
            if (resolutionMetric > pspMetricCm)
                return false;
            tempLength += 1;
        }
        if (tempLength < chunkSize) {//check if tempLength == chunksize
            input >> compressionType;
            if (compressionType >= pspCompJpeg)
                return false;
            tempLength += 2;
        }
        if (tempLength < chunkSize) {//check if tempLength == chunksize
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
            tempLength += 2;
        }
        if (tempLength < chunkSize) {//check if tempLength == chunksize
            input >> planeCount;
            if (planeCount != 1)
                return false;
            tempLength += 2;
        }
        if (tempLength < chunkSize) {//check if tempLength == chunksize
            input >> colorCount;
            if (colorCount != qPow(2, bitDepth))
                return false;
            tempLength += 4;
        }
        if (tempLength < chunkSize) {//check if tempLength == chunksize
            input >> greyscaleFlag;
            if (greyscaleFlag != 0)
                if (greyscaleFlag != 1)
                    return false;
            tempLength += 1;
        }
        if (tempLength < chunkSize) {//check if tempLength == chunksize
            input >> totalImageSize;
            tempLength += 4;
        }
        if (tempLength < chunkSize) {//check if tempLength == chunksize
            input >> activeLayer;
            tempLength += 4;
        }
        if (tempLength < chunkSize) {//check if tempLength == chunksize
            input >> layerCount;
            tempLength += 2;
        }
        if (tempLength < chunkSize) {//check if tempLength == chunksize
            input >> graphicContents;
            tempLength += 4;
        }
        /* Expansion field
         * This field is currently non-existent, but robust readers should assume that
         * data may be added here in the future, and silently ignore it.
         */
        while (tempLength < chunkSize) {
            quint8 temp;
            input >> temp;
            tempLength += 1;
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
                 << "layer count:" << layerCount << endl
                 << "graphic contents:" << graphicContents << endl;
    }

    /* skip all optional blocks */
    bool end = false;
    while (!end) {
        input.readRawData(headerIdentifier.data(), 4);
        if (headerIdentifier != QByteArray::fromHex("7e424b00")) {
            qDebug() << "invalid header";
            return false;
        }
        input >> blockIdentifier;
        if (blockIdentifier != pspCompositeImageBankBlock) {
            //qDebug() << "block identifier" << blockIdentifier;
            input >> totalBlockLength;
            input.skipRawData(totalBlockLength);
        } else {
            end = true;
        }
    }

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
        if (blockIdentifier != pspCompositeAttributesBlock)
            return false;
        input >> totalBlockLength;
        length += 10 + totalBlockLength;
        if (i != activeLayer)
            input.skipRawData(totalBlockLength);
        else {
            input >> chunkSize;
            if (totalBlockLength != chunkSize)
                return false;
            input >> compositeWidth >> compositeHeight;
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
            if (compositeCompressionType > pspCompJpeg)
                return false;
            input >> compositePlaneCount;
            if (compositePlaneCount != 1)
                return false;
            input >> compositeColorCount;
            if (compositeColorCount != qPow(2, compositeBitDepth))
                return false;
            input >> compositeImageType;
            qDebug() << "composite width:" << compositeWidth << endl
                     << "composite height:" << compositeHeight << endl
                     << "composite bit depth:" << compositeBitDepth << endl
                     << "composite compression type:" << compositeCompressionType << endl;
        }
    }

    /* Composite Image Sub-Block */
    for (quint32 i = 0; i < compositeImageCount; ++i) {
        input.readRawData(headerIdentifier.data(), 4);
        if (headerIdentifier != QByteArray::fromHex("7e424b00"))
            return false;
        input >> blockIdentifier;
        if (compositeCompressionType == pspCompJpeg) {
            if (blockIdentifier != pspJpegBlock)
                return false;
        } else {
            if (blockIdentifier != pspCompositeImageBlock)
                return false;
        }

        input >> totalBlockLength;
        length += 10 + totalBlockLength;
        if (i != activeLayer)
            input.skipRawData(totalBlockLength);
        else {
            quint32 compressedImageSize, uncompressedImageSize;
            quint16 imageType;
            input >> chunkSize >> compressedImageSize >> uncompressedImageSize >> imageType;
            compositeData.resize(compressedImageSize);
            input.readRawData(compositeData.data(), compressedImageSize);
            qDebug() << "data size:" << compositeData.size() << endl
                     << "compressed image size:" << compressedImageSize << endl
                     << "uncompressed image size:" << uncompressedImageSize << endl
                     << "image type:" << imageType << endl;
            *image = QImage::fromData(compositeData);
            if (!image->isNull()) {
                return true;
            } else {
                qDebug() << "null";
                return false;
            }

        }
    }

    if (compositeImageBankBlockLength != length)
        return false;

    if (input.status() != QDataStream::Ok)
        return false;
    return true;
}

//not yet implemented
bool QPspHandler::supportsOption(ImageOption option) const
{
    Q_UNUSED(option);
    return false;
}

QVariant QPspHandler::option(ImageOption option) const
{
    Q_UNUSED(option);
    return QVariant();
}
