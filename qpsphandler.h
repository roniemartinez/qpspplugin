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

private:
    /* NOTE: All "enums" from "PSP File Format Specification 8" are preserved, both
     * usable and unusable
     */

    /* Block identifiers.
     */
    enum PSPBlockID
    {
        PSP_IMAGE_BLOCK = 0, // General Image Attributes Block (main)
        PSP_CREATOR_BLOCK, // Creator Data Block (main)
        PSP_COLOR_BLOCK, // Color Palette Block (main and sub)
        PSP_LAYER_START_BLOCK, // Layer Bank Block (main)
        PSP_LAYER_BLOCK, // Layer Block (sub)
        PSP_CHANNEL_BLOCK, // Channel Block (sub)
        PSP_SELECTION_BLOCK, // Selection Block (main)
        PSP_ALPHA_BANK_BLOCK, // Alpha Bank Block (main)
        PSP_ALPHA_CHANNEL_BLOCK,// Alpha Channel Block (sub)
        PSP_COMPOSITE_IMAGE_BLOCK, // Composite Image Block (sub)
        PSP_EXTENDED_DATA_BLOCK,// Extended Data Block (main)
        PSP_TUBE_BLOCK, // Picture Tube Data Block (main)
        PSP_ADJUSTMENT_EXTENSION_BLOCK, // Adjustment Layer Block (sub)
        PSP_VECTOR_EXTENSION_BLOCK, // Vector Layer Block (sub)
        PSP_SHAPE_BLOCK, // Vector Shape Block (sub)
        PSP_PAINTSTYLE_BLOCK, // Paint Style Block (sub)
        PSP_COMPOSITE_IMAGE_BANK_BLOCK, // Composite Image Bank (main)
        PSP_COMPOSITE_ATTRIBUTES_BLOCK, // Composite Image Attr. (sub)
        PSP_JPEG_BLOCK, // JPEG Image Block (sub)
        PSP_LINESTYLE_BLOCK, // Line Style Block (sub)
        PSP_TABLE_BANK_BLOCK, // Table Bank Block (main)
        PSP_TABLE_BLOCK, // Table Block (sub)
        PSP_PAPER_BLOCK, // Vector Table Paper Block (sub)
        PSP_PATTERN_BLOCK, // Vector Table Pattern Block (sub)
        PSP_GRADIENT_BLOCK, // Vector Table Gradient Block (not used)
        PSP_GROUP_EXTENSION_BLOCK, // Group Layer Block (sub)
        PSP_MASK_EXTENSION_BLOCK, // Mask Layer Block (sub)
        PSP_BRUSH_BLOCK, // Brush Data Block (main)
        PSP_ART_MEDIA_BLOCK, // Art Media Layer Block (main)
        PSP_ART_MEDIA_MAP_BLOCK, // Art Media Layer map data Block (main)
        PSP_ART_MEDIA_TILE_BLOCK, // Art Media Layer map tile Block(main)
        PSP_ART_MEDIA_TEXTURE_BLOCK,// AM Layer map texture Block (main)
        PSP_COLORPROFILE_BLOCK, // ICC Color profile block
    };
    /* Truth values.
    */
    /*
    enum PSP_BOOLEAN
    {
        FALSE = 0,
        TRUE,
    };*/
    /* Possible metrics used to measure resolution.
    */
    enum PSP_METRIC
    {
    PSP_METRIC_UNDEFINED = 0, // Metric unknown
    PSP_METRIC_INCH, // Resolution is in inches
    PSP_METRIC_CM, // Resolution is in centimeters
    };
    /* Creator application identifiers.
    */
    enum PSPCreatorAppID
    {
    PSP_CREATOR_APP_UNKNOWN = 0, // Creator application unknown
    PSP_CREATOR_APP_PAINT_SHOP_PRO, // Creator is Paint Shop Pro
    };
    /* Creator field types.
    */
    enum PSPCreatorFieldID
    {
    PSP_CRTR_FLD_TITLE = 0, // Image document title field
    PSP_CRTR_FLD_CRT_DATE, // Creation date field
    PSP_CRTR_FLD_MOD_DATE, // Modification date field
    PSP_CRTR_FLD_ARTIST, // Artist name field
    PSP_CRTR_FLD_CPYRGHT, // Copyright holder name field
    PSP_CRTR_FLD_DESC, // Image document description field
    PSP_CRTR_FLD_APP_ID, // Creating app id field
    PSP_CRTR_FLD_APP_VER, // Creating app version field
    };
    /* Extended data field types.
    */
    enum PSPExtendedDataID
    {
    PSP_XDATA_TRNS_INDEX = 0, // Transparency index field
    PSP_XDATA_GRID, // Image grid information
    PSP_XDATA_GUIDE, // Image guide information
    PSP_XDATA_EXIF, // Image EXIF information
    PSP_XDATA_IPTC, // Image IPTC information
    };
    /* Grid units type.
    */
    enum PSPGridUnitsType
    {
        keGridUnitsPixels = 0, // Grid units is pixels
        keGridUnitsInches, // Grid units is inches
        keGridUnitsCentimeters // Grid units is centimeters
    };
    /* Guide orientation type.
    */
    enum PSPGuideOrientationType
    {
        keHorizontalGuide = 0, // Horizontal guide direction
        keVerticalGuide // Vertical guide direction
    };
    /* Bitmap types.
    */
    enum PSPDIBType
    {
        PSP_DIB_IMAGE = 0, // Layer color bitmap
        PSP_DIB_TRANS_MASK, // Layer transparency mask bitmap
        PSP_DIB_USER_MASK, // Layer user mask bitmap
        PSP_DIB_SELECTION, // Selection mask bitmap
        PSP_DIB_ALPHA_MASK, // Alpha channel mask bitmap
        PSP_DIB_THUMBNAIL, // Thumbnail bitmap
        PSP_DIB_THUMBNAIL_TRANS_MASK, // Thumbnail transparency mask
        PSP_DIB_ADJUSTMENT_LAYER, // Adjustment layer bitmap
        PSP_DIB_COMPOSITE, // Composite image bitmap
        PSP_DIB_COMPOSITE_TRANS_MASK, // Composite image transparency
        PSP_DIB_PAPER, // Paper bitmap
        PSP_DIB_PATTERN, // Pattern bitmap
        PSP_DIB_PATTERN_TRANS_MASK, // Pattern transparency mask
    };
    /* Type of image in the composite image bank block.
    */
    enum PSPCompositeImageType
    {
        PSP_IMAGE_COMPOSITE = 0, // Composite Image
        PSP_IMAGE_THUMBNAIL, // Thumbnail Image
    };
    /* Channel types.
    */
    enum PSPChannelType
    {
        PSP_CHANNEL_COMPOSITE = 0, // Channel of single channel bitmap
        PSP_CHANNEL_RED, // Red channel of 8, 16 bpc bitmap
        PSP_CHANNEL_GREEN, // Green channel of 8,16 bpc bitmap
        PSP_CHANNEL_BLUE, // Blue channel of 8, 16 bpc bitmap
    };
    /* Possible types of compression.
    */
    enum PSPCompression
    {
        PSP_COMP_NONE = 0, // No compression
        PSP_COMP_RLE, // RLE compression
        PSP_COMP_LZ77, // LZ77 compression
        PSP_COMP_JPEG // JPEG compression (only used by
        // thumbnail and composite image)
    };
    /* Layer types.
    */
    enum PSPLayerType
    {
        keGLTUndefined = 0, // Undefined layer type
        keGLTRaster, // Standard raster layer
        keGLTFloatingRasterSelection, // Floating selection (raster)
        keGLTVector, // Vector layer
        keGLTAdjustment, // Adjustment layer
        keGLTGroup, // Group layer
        keGLTMask, // Mask layer
        keGLTArtMedia // Art media layer
    };
    /* Layer flags.
    */
    enum PSPLayerProperties
    {
        keVisibleFlag = 0x00000001, // Layer is visible
        keMaskPresenceFlag = 0x00000002, // Layer has a mask
    };
    /* Blend modes.
    */
    enum PSPBlendModes
    {
        LAYER_BLEND_NORMAL,
        LAYER_BLEND_DARKEN,
        LAYER_BLEND_LIGHTEN,
        LAYER_BLEND_LEGACY_HUE,
        LAYER_BLEND_LEGACY_SATURATION,
        LAYER_BLEND_LEGACY_COLOR,
        LAYER_BLEND_LEGACY_LUMINOSITY,
        LAYER_BLEND_MULTIPLY,
        LAYER_BLEND_SCREEN,
        LAYER_BLEND_DISSOLVE,
        LAYER_BLEND_OVERLAY,
        LAYER_BLEND_HARD_LIGHT,
        LAYER_BLEND_SOFT_LIGHT,
        LAYER_BLEND_DIFFERENCE,
        LAYER_BLEND_DODGE,
        LAYER_BLEND_BURN,
        LAYER_BLEND_EXCLUSION,
        LAYER_BLEND_TRUE_HUE,
        LAYER_BLEND_TRUE_SATURATION,
        LAYER_BLEND_TRUE_COLOR,
        LAYER_BLEND_TRUE_LIGHTNESS,
        LAYER_BLEND_ADJUST = 255,
    };
    /* Adjustment layer types.
    */
    enum PSPAdjustmentLayerType
    {
        keAdjNone = 0, // Undefined adjustment layer type
        keAdjLevel, // Level adjustment
        keAdjCurve, // Curve adjustment
        keAdjBrightContrast, // Brightness-contrast adjustment
        keAdjColorBal, // Color balance adjustment
        keAdjHSL, // HSL adjustment
        keAdjChannelMixer, // Channel mixer adjustment
        keAdjInvert, // Invert adjustment
        keAdjThreshold, // Threshold adjustment
        keAdjPoster // Posterize adjustment
    };
    /* Art media layer map types
    */
    enum PSPArtMediaMapType
    {
        keArtMediaColorMap = 0,
        keArtMediaBumpMap,
        keArtMediaShininessMap,
        keArtMediaReflectivityMap,
        keArtMediaDrynessMap
    };
    /* Vector shape types.
    */
    enum PSPVectorShapeType
    {
        keVSTUnknown = 0, // Undefined vector type
        keVSTText, // Shape represents lines of text
        keVSTPolyline, // Shape represents a multiple segment line
        keVSTEllipse, // Shape represents an ellipse (or circle)
        keVSTPolygon, // Shape represents a closed polygon
        keVSTGroup, // Shape represents a group shape
    };
    /* Shape property flags
    */
    enum PSPShapeProperties
    {
        keShapeAntiAliased = 0x00000001, // Shape is anti-aliased
        keShapeSelected = 0x00000002, // Shape is selected
        keShapeVisible = 0x00000004, // Shape is visible
    };
    /* Polyline node type flags.
    */
    enum PSPPolylineNodeTypes
    {
        keNodeUnconstrained = 0x0000, // Default node type
        keNodeSmooth = 0x0001, // Node is smooth
        keNodeSymmetric = 0x0002, // Node is symmetric
        keNodeAligned = 0x0004, // Node is aligned
        keNodeActive = 0x0008, // Node is active
        keNodeLocked = 0x0010, // Node is locked
        keNodeSelected = 0x0020, // Node is selected
        keNodeVisible = 0x0040, // Node is visible
        keNodeClosed = 0x0080, // Node is closed
    };
    /* Paint style types.
    */
    enum PSPPaintStyleType
    {
        keStyleNone = 0x0000, // No paint style info applies
        keStyleColor = 0x0001, // Color paint style info
        keStyleGradient = 0x0002, // Gradient paint style info
        keStylePattern = 0x0004, // Pattern paint style info
        keStylePaper = 0x0008, // Paper paint style info
        keStylePen = 0x0010, // Organic pen paint style info
    };
    /* Gradient type.
    */
    enum PSPStyleGradientType
    {
        keSGTLinear = 0, // Linera gradient type
        keSGTRadial, // Radial gradient type
        keSGTRectangular, // Rectangulat gradient type
        keSGTSunburst // Sunburst gradient type
    };
    /* Paint Style Cap Type (Start & End).
    */
    enum PSPStyleCapType
    {
        keSCTCapFlat = 0, // Flat cap type (was round in psp6)
        keSCTCapRound, // Round cap type (was square in psp6)
        keSCTCapSquare, // Square cap type (was flat in psp6)
        keSCTCapArrow, // Arrow cap type
        keSCTCapCadArrow, // Cad arrow cap type
        keSCTCapCurvedTipArrow, // Curved tip arrow cap type
        keSCTCapRingBaseArrow, // Ring base arrow cap type
        keSCTCapFluerDelis, // Fluer deLis cap type
        keSCTCapFootball, // Football cap type
        keSCTCapXr71Arrow, // Xr71 arrow cap type
        keSCTCapLilly, // Lilly cap type
        keSCTCapPinapple, // Pinapple cap type
        keSCTCapBall, // Ball cap type
        keSCTCapTulip // Tulip cap type
    };
    /* Paint Style Join Type.
    */
    enum PSPStyleJoinType
    {
        keSJTJoinMiter = 0, // Miter join type
        keSJTJoinRound, // Round join type
        keSJTJoinBevel // Bevel join type
    };
    /* Organic pen type.
    */
    enum PSPStylePenType
    {
        keSPTOrganicPenNone = 0, // Undefined pen type
        keSPTOrganicPenMesh, // Mesh pen type
        keSPTOrganicPenSand, // Sand pen type
        keSPTOrganicPenCurlicues, // Curlicues pen type
        keSPTOrganicPenRays, // Rays pen type
        keSPTOrganicPenRipple, // Ripple pen type
        keSPTOrganicPenWave, // Wave pen type
        keSPTOrganicPen // Generic pen type
    };
    /* Text element types.
    */
    enum PSPTextElementType
    {
        keTextElemUnknown = 0, // Undefined text element type
        keTextElemChar, // A single character code
        keTextElemCharStyle, // A character style change
        keTextElemLineStyle // A line style change
    };
    /* Text alignment types.
    */
    enum PSPTextAlignment
    {
        keTextAlignmentLeft = 0, // Left text alignment
        keTextAlignmentCenter, // Center text alignment
        keTextAlignmentRight // Right text alignment
    };
    /* Text antialias modes.
    */
    enum PSPAntialiasMode
    {
        keNoAntialias = 0, // Antialias off
        keSharpAntialias, // Sharp
        keSmoothAntialias // Smooth
    };
    /* Text flow types
    */
    enum PSPTextFlow
    {
        keTFHorizontalDown = 0, // Horizontal then down
        keTFVerticalLeft, // Vertical then left
        keTFVerticalRight, // Vertical then right
        keTFHorizontalUp // Horizontal then up
    };
    /* Character style flags.
    */
    enum PSPCharacterProperties
    {
        keStyleItalic = 0x00000001, // Italic property bit
        keStyleStruck = 0x00000002, // Strike-out property bit
        keStyleUnderlined = 0x00000004, // Underlined property bit
        keStyleWarped = 0x00000008, // Warped property bit
        keStyleAntiAliased = 0x00000010, // Anti-aliased property bit
    };
    /* Table type.
    */
    enum PSPTableType
    {
        keTTUndefined = 0, // Undefined table type
        keTTGradientTable, // Gradient table type
        keTTPaperTable, // Paper table type
        keTTPatternTable // Pattern table type
    };
    /* Picture tube placement mode.
    */
    enum TubePlacementMode
    {
        tpmRandom, // Place tube images in random intervals
        tpmConstant, // Place tube images in constant intervals
    };
    /* Picture tube selection mode.
    */
    enum TubeSelectionMode
    {
        tsmRandom, // Randomly select the next image in
        // tube to display
        tsmIncremental, // Select each tube image in turn
        tsmAngular, // Select image based on cursor direction
        tsmPressure, // Select image based on pressure
        // (from pressure-sensitive pad)
        tsmVelocity, // Select image based on cursor speed
    };
    /* Graphic contents flags.
    */
    enum PSPGraphicContents
    {
        // Layer types
        keGCRasterLayers = 0x00000001, // At least one raster layer
        keGCVectorLayers = 0x00000002, // At least one vector layer
        keGCAdjustmentLayers = 0x00000004, // At least one adjust. layer
        keGCGroupLayers = 0x00000008, // at least one group layer
        keGCMaskLayers = 0x00000010, // at least one mask layer
        keGCArtMediaLayers = 0x00000020, // at least one art media layer
        // Additional attributes
        keGCMergedCache = 0x00800000, // merged cache (composite image)
        keGCThumbnail = 0x01000000, // Has a thumbnail
        keGCThumbnailTransparency = 0x02000000, // Thumbnail transp.
        keGCComposite = 0x04000000, // Has a composite image
        keGCCompositeTransparency = 0x08000000, // Composite transp.
        keGCFlatImage = 0x10000000, // Just a background
        keGCSelection = 0x20000000, // Has a selection
        keGCFloatingSelectionLayer = 0x40000000, // Has float. selection
        keGCAlphaChannels = 0x80000000, // Has alpha channel(s)
    };
};

#endif // QPSPHANDLER_H
