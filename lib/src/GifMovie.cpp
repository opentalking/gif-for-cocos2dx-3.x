#include "cocos2d.h"
#include "GIFMovie.h"

GIFMovie::GIFMovie()
    : m_GIF(nullptr)
    , m_CurrentIndex(-1)
    , m_LastDrawIndex(-1)
{

}

bool GIFMovie::init(const char *fileName)
{
    FILE *file = GifUtils::openFile(fileName);
    return init(file);
}

bool GIFMovie::init(FILE *file)
{
    if (file == nullptr) {
        return false;
    }

    int error = 0;
    m_GIF = DGifOpen(file, &decodeCallBackProc, &error);

    if (nullptr == m_GIF || DGifSlurp(m_GIF) != GIF_OK) {
        GifUtils::closeFile(file);
        DGifCloseFile(m_GIF);
        m_GIF = nullptr;
        return false;
    }

    GifUtils::closeFile(file);
    return true;
}

GIFMovie::~GIFMovie()
{
    if (m_GIF) {
        DGifCloseFile(m_GIF);
    }
}

uint32_t savedimage_duration(const SavedImage *image)
{
    uint32_t duration = 0;
    for (int j = 0; j < image->ExtensionBlockCount; j++) {
        if (image->ExtensionBlocks[j].Function == GRAPHICS_EXT_FUNC_CODE) {
            int size = image->ExtensionBlocks[j].ByteCount;
            //assert(size >= 4);
            if (size < 4) break;
            const uint8_t *b = (const uint8_t *)image->ExtensionBlocks[j].Bytes;
            duration = ((b[2] << 8) | b[1]) * 10;
            break;
        }
    }

    duration = duration <= 50 ? 50 : duration;
    return duration;
}

GifFrame GIFMovie::getGifFrameByIndex(unsigned int frameIndex)
{
    GifFrame gif;

    if (m_GIF == nullptr || frameIndex > getGifCount()) {
        return gif;
    }

    const int duration = savedimage_duration(&m_GIF->SavedImages[frameIndex]);
    m_CurrentIndex = frameIndex;
    onGetBitmap(&m_Bitmap);

    gif.frameData.duration = duration;
    gif.frameData.index = frameIndex;
    gif.bitmap = &m_Bitmap;

    return gif;
}

bool GIFMovie::onGetInfo(Info *info)
{
    if (nullptr == m_GIF)
        return false;

    uint32_t dur = 0;
    for (int i = 0; i < m_GIF->ImageCount; i++) {
        dur += savedimage_duration(&m_GIF->SavedImages[i]);
    }

    info->duration = dur;
    info->width = m_GIF->SWidth;
    info->height = m_GIF->SHeight;
    // FIXME: How to compute?
    info->isOpaque = false;
    return true;
}

bool GIFMovie::onSetTime(uint32_t time)
{
    if (nullptr == m_GIF) {
        return false;
    }

    uint32_t dur = 0;
    for (int i = 0; i < m_GIF->ImageCount; i++) {
        dur += savedimage_duration(&m_GIF->SavedImages[i]);
        if (dur >= time) {
            m_CurrentIndex = i;
            return m_LastDrawIndex != m_CurrentIndex;
        }
    }

    m_CurrentIndex = m_GIF->ImageCount - 1;
    return true;
}

bool GIFMovie::onGetBitmap(Bitmap *bm)
{
    const GifFileType *gif = m_GIF;
    if (nullptr == gif)
        return false;

    if (gif->ImageCount < 1) {
        return false;
    }

    bm->setWidth(gif->SWidth);
    bm->setHeight(gif->SHeight);

    const int width = gif->SWidth;
    const int height = gif->SHeight;

    if (width <= 0 || height <= 0) {
        return false;
    }

    if (m_LastDrawIndex >= 0 && m_LastDrawIndex == m_CurrentIndex) {
        return true;
    }

    int startIndex = m_LastDrawIndex + 1;
    if (m_LastDrawIndex < 0 || !bm->isValid()) {
        // first time
        m_BackupBitmap.setWidth(width);
        m_BackupBitmap.setHeight(height);
        bm->allocateBitmap();
        m_BackupBitmap.allocateBitmap();
        startIndex = 0;
    }
    else if (startIndex > m_CurrentIndex) {
        // rewind to 1st frame for repeat
        startIndex = 0;
    }

    int lastIndex = m_CurrentIndex;
    if (lastIndex < 0) {
        // first time
        lastIndex = 0;
    }
    else if (lastIndex > m_GIF->ImageCount - 1) {
        // this block must not be reached.
        lastIndex = m_GIF->ImageCount - 1;
    }

    Rgba bgColor;
    if (gif->SColorMap != nullptr) {
        const GifColorType &col = gif->SColorMap->Colors[m_GIF->SBackGroundColor];
        bgColor.setColor(0xFF, col.Red, col.Green, col.Blue);
    }

    Rgba paintingColor;
    // draw each frames - not intelligent way
    for (int i = startIndex; i <= lastIndex; i++) {
        const SavedImage *cur = &m_GIF->SavedImages[i];
        if (i == 0) {
            bool trans;
            int disposal;
            getTransparencyAndDisposalMethod(cur, &trans, &disposal);
            if (!trans && gif->SColorMap != nullptr) {
                paintingColor = bgColor;
            }

            bm->eraseColor(paintingColor);
            m_BackupBitmap.eraseColor(paintingColor);
        }
        else {
            // Dispose previous frame before move to next frame.
            const SavedImage *prev = &m_GIF->SavedImages[i - 1];
            disposeFrameIfNeeded(bm, prev, cur, &m_BackupBitmap, paintingColor);
        }

        // Draw frame
        // We can skip this process if this index is not last and disposal
        // method == 2 or method == 3
        if (i == lastIndex || !checkIfWillBeCleared(cur)) {
            drawFrame(bm, cur, gif->SColorMap);
        }
    }

    // save index
    m_LastDrawIndex = lastIndex;

    return true;
}

void GIFMovie::packARGB32(Rgba *pixel, GifByteType alpha, GifByteType red, GifByteType green, GifByteType blue)
{
    pixel->alpha = alpha;
    pixel->red = red;
    pixel->green = green;
    pixel->blue = blue;
}

int GIFMovie::decodeCallBackProc(GifFileType *gif, GifByteType *bytes, int size)
{
    FILE *file = (FILE *) gif->UserData;
    return fread(bytes, 1, size, file);
}

void GIFMovie::getColorFromTable(int idx, Rgba *dst, const ColorMapObject *cmap)
{
    char colIdx = idx >= cmap->ColorCount ? 0 : idx;
    GifColorType *col = &cmap->Colors[colIdx];
    packARGB32(dst, 0xFF, col->Red, col->Green, col->Blue);
}

void GIFMovie::copyLine(Rgba *dst, const unsigned char *src, const ColorMapObject *cmap, int transparent, int width)
{
    for (; width > 0; width--, src++, dst++) {
        if (*src != transparent) {
            const GifColorType &col = cmap->Colors[*src];
            *dst = Rgba(0xFF, col.Red, col.Green, col.Blue);
        }
    }
}

void GIFMovie::blitNormal(Bitmap *bm, const SavedImage *frame, const ColorMapObject *cmap, int transparent)
{
    GifWord width = bm->getWidth();
    GifWord height = bm->getHeight();
    const unsigned char *src = (unsigned char *)frame->RasterBits;
    Rgba *dst = bm->getAddr(frame->ImageDesc.Left, frame->ImageDesc.Top);

    GifWord copyWidth = frame->ImageDesc.Width;
    if (frame->ImageDesc.Left + copyWidth > width) {
        copyWidth = width - frame->ImageDesc.Left;
    }

    GifWord copyHeight = frame->ImageDesc.Height;
    if (frame->ImageDesc.Top + copyHeight > height) {
        copyHeight = height - frame->ImageDesc.Top;
    }

    for (; copyHeight > 0; copyHeight--) {
        copyLine(dst, src, cmap, transparent, copyWidth);
        src += frame->ImageDesc.Width;
        dst += width;
    }
}

void GIFMovie::fillRect(Bitmap *bm, GifWord left, GifWord top, GifWord width, GifWord height,
                        Rgba col)
{
    int bmWidth = bm->getWidth();
    int bmHeight = bm->getHeight();
    Rgba *dst = bm->getAddr(left, top);
    GifWord copyWidth = width;

    if (left + copyWidth > bmWidth) {
        copyWidth = bmWidth - left;
    }

    GifWord copyHeight = height;
    if (top + copyHeight > bmHeight) {
        copyHeight = bmHeight - top;
    }

    for (; copyHeight > 0; copyHeight--) {
        for (int wIndex = 0; wIndex < copyWidth; wIndex++, dst++) {
            *dst = col;
        }
    }
}

void GIFMovie::drawFrame(Bitmap *bm, const SavedImage *frame, const ColorMapObject *cmap)
{
    int transparent = -1;

    for (int i = 0; i < frame->ExtensionBlockCount; ++i) {
        ExtensionBlock *eb = frame->ExtensionBlocks + i;
        if (eb->Function == GRAPHICS_EXT_FUNC_CODE &&
            eb->ByteCount == 4) {
            bool has_transparency = ((eb->Bytes[0] & 1) == 1);
            if (has_transparency) {
                transparent = (unsigned char)eb->Bytes[3];
            }
        }
    }

    if (frame->ImageDesc.ColorMap != nullptr) {
        // Use local color table
        cmap = frame->ImageDesc.ColorMap;
    }

    if (cmap == nullptr || cmap->ColorCount != (1 << cmap->BitsPerPixel)) {

        return;
    }

    blitNormal(bm, frame, cmap, transparent);
}

bool GIFMovie::checkIfWillBeCleared(const SavedImage *frame)
{
    for (int i = 0; i < frame->ExtensionBlockCount; ++i) {
        ExtensionBlock *eb = frame->ExtensionBlocks + i;
        if (eb->Function == GRAPHICS_EXT_FUNC_CODE &&
            eb->ByteCount == 4) {
            // Check disposal method
            int disposal = ((eb->Bytes[0] >> 2) & 7);
            if (disposal == 2 || disposal == 3) {
                return true;
            }
        }
    }
    return false;
}

void GIFMovie::getTransparencyAndDisposalMethod(const SavedImage *frame, bool *trans, int *disposal)
{
    *trans = false;
    *disposal = 0;
    for (int i = 0; i < frame->ExtensionBlockCount; ++i) {
        ExtensionBlock *eb = frame->ExtensionBlocks + i;
        if (eb->Function == GRAPHICS_EXT_FUNC_CODE &&
            eb->ByteCount == 4) {
            *trans = ((eb->Bytes[0] & 1) == 1);
            *disposal = ((eb->Bytes[0] >> 2) & 7);
        }
    }
}

bool GIFMovie::checkIfCover(const SavedImage *target, const SavedImage *covered)
{
    if (target->ImageDesc.Left <= covered->ImageDesc.Left
        && covered->ImageDesc.Left + covered->ImageDesc.Width <= target->ImageDesc.Left + target->ImageDesc.Width
        && target->ImageDesc.Top <= covered->ImageDesc.Top
        && covered->ImageDesc.Top + covered->ImageDesc.Height <= target->ImageDesc.Top + target->ImageDesc.Height) {
        return true;
    }
    return false;
}

void GIFMovie::disposeFrameIfNeeded(Bitmap *bm, const SavedImage *cur, const SavedImage *next, Bitmap *backup, Rgba color)
{
    // We can skip disposal process if next frame is not transparent
    // and completely covers current area
    bool curTrans = false;
    int curDisposal = 0;
    getTransparencyAndDisposalMethod(cur, &curTrans, &curDisposal);

    bool nextTrans = false;
    int nextDisposal = 0;
    getTransparencyAndDisposalMethod(next, &nextTrans, &nextDisposal);


    // #define DISPOSAL_UNSPECIFIED      0       /* No disposal specified. */
    // #define DISPOSE_DO_NOT            1       /* Leave image in place */
    // #define DISPOSE_BACKGROUND        2       /* Set area too background color */
    // #define DISPOSE_PREVIOUS          3       /* Restore to previous content */

    if ((curDisposal == DISPOSE_BACKGROUND || curDisposal == DISPOSE_PREVIOUS) && (nextTrans || !checkIfCover(next, cur))) {
        switch (curDisposal) {
        // restore to background color
        // -> 'background' means background under this image.
        case DISPOSE_BACKGROUND:
            fillRect(bm, cur->ImageDesc.Left, cur->ImageDesc.Top, cur->ImageDesc.Width, cur->ImageDesc.Height, color);
            break;

        // restore to previous
        case DISPOSE_PREVIOUS:
            bm->swap(backup);
            break;
        }
    }

    // Save current image if next frame's disposal method == 3
    if (nextDisposal == DISPOSE_PREVIOUS) {
        memcpy(backup->getAddr(0, 0), bm->getAddr(0, 0), bm->getPixelLenth() * sizeof(Rgba));
    }
}
