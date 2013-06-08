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


#ifndef QPSPHANDLER_H
#define QPSPHANDLER_H

#include <QImageIOHandler>
#include <QVariant>
#include <QImage>
#include <qmath.h>


class QPspHandler : public QImageIOHandler
{
public:
    QPspHandler();
    ~QPspHandler();

    bool canRead() const;
    bool read(QImage *image);
    //bool write(const QImage &image);

    static bool canRead(QIODevice *device);

    QVariant option(ImageOption option) const;
    //void setOption(ImageOption option, const QVariant &value);
    bool supportsOption(ImageOption option) const;

    enum pspBlockID
    {
        pspImageBlock = 0,              // General Image Attributes Block (main)
        pspCreatorBlock,                // Creator Data Block (main)
        pspColorBlock,                  // Color Palette Block (main and sub)
        pspLayerStartBlock,             // Layer Bank Block (main)
        pspLayerBlock,                  // Layer Block (sub)
        pspChannelBlock,                // Channel Block (sub)
        pspSelectionBlock,              // Selection Block (main)
        pspAlphaBankBlock,              // Alpha Bank Block (main)
        pspAlphaChannelBlock,           // Alpha Channel Block (sub)
        pspCompositeImageBlock,         // Composite Image Block (sub)
        pspExtendedDataBlock,           // Extended Data Block (main)
        pspTubeBlock,                   // Picture Tube Data Block (main)
        pspAdjustmentExtensionBlock,    // Adjustment Layer Block (sub)
        pspVectorExtensionBlock,        // Vector Layer Block (sub)
        pspShapeBlock,                  // Vector Shape Block (sub)
        pspPaintStyleBlock,             // Paint Style Block (sub)
        pspCompositeImageBankBlock,     // Composite Image Bank (main)
        pspCompositeAttributesBlock,    // Composite Image Attr. (sub)
        pspJpegBlock,                   // JPEG Image Block (sub)
        pspLineStyleBlock,              // Line Style Block (sub)
        pspTableBankBlock,              // Table Bank Block (main)
        pspTableBlock,                  // Table Block (sub)
        pspPaperBlock,                  // Vector Table Paper Block (sub)
        pspPatternBlock,                // Vector Table Pattern Block (sub)
        pspGradientBlock,               // Vector Table Gradient Block (not used)
        pspGroupExtensionBlock,         // Group Layer Block (sub)
        pspMaskExtensionBlock,          // Mask Layer Block (sub)
        pspBrushBlock,                  // Brush Data Block (main)
        pspArtMediaBlock,               // Art Media Layer Block (main)
        pspArtMediaMapBlock,            // Art Media Layer map data Block (main)
        pspArtMediaTileBlock,           // Art Media Layer map tile Block(main)
        pspArtMediaTextureBlock,        // AM Layer map texture Block (main)
        pspColorProfileBlock            // ICC Color Profile block
    };

    enum pspMetric
    {
        pspMetricUndefind = 0,  // Metric unknown
        pspMetricInch,          // Resolution is in inches
        pspMetricCm            // Resolution is in centimeters
    };

    enum pspCompression
    {
        pspCompNone = 0,    // No compression
        pspCompRle,         // RLE compression
        pspCompLz77,        // LZ77 compression
        pspCompJpeg         // JPEG compression (only used by thumbnail and composite image)
    };

    enum pspGraphicContents
    {
        // Layer types
        keGCRasterLayers = 0x00000001,              // At least one raster layer
        keGCVectorLayers = 0x00000002,              // At least one vector layer
        keGCAdjustmentLayers = 0x00000004,          // At least one adjust. layer
        keGCGroupLayers = 0x00000008,               // at least one group layer
        keGCMaskLayers = 0x00000010,                // at least one mask layer
        keGCArtMediaLayers = 0x00000020,            // at least one art media layer
        // Additional attributes
        keGCMergedCache = 0x00800000,               // merged cache (composite image)
        keGCThumbnail = 0x01000000,                 // Has a thumbnail
        keGCThumbnailTransparency = 0x02000000,     // Thumbnail transp.
        keGCComposite = 0x04000000,                 // Has a composite image
        keGCCompositeTransparency = 0x08000000,     // Composite transp.
        keGCFlatImage = 0x10000000,                 // Just a background
        keGCSelection = 0x20000000,                 // Has a selection
        keGCFloatingSelectionLayer = 0x40000000,    // Has float. selection
        keGCAlphaChannels = 0x80000000              // Has alpha channel(s)
    };

    enum pspCompositeImageType
    {
        pspImageComposite = 0,    // Composite Image
        pspImageThumbnail        // Thumbnail Image
    };

    enum pspDibType
    {
        pspDibImage = 0,            // Layer color bitmap
        pspDibTransMask,            // Layer transparency mask bitmap
        pspDibUserMask,             // Layer user mask bitmap
        pspDibSelection,            // Selection mask bitmap
        pspDibAlphaMask,            // Alpha channel mask bitmap
        pspDibThumbnail,            // Thumbnail bitmap
        pspDibThumbnailTransMask,   // Thumbnail transparency mask
        pspDibAdjustmentLayer,      // Adjustment layer bitmap
        pspDibComposite,            // Composite image bitmap
        pspDibCompositeTransMask,   // Composite image transparency
        pspDibPaper,                // Paper bitmap
        pspDibPattern,              // Pattern bitmap
        pspDibPatternTransMask      // Pattern transparency mask
    };
};

#endif // QPSPHANDLER_H
