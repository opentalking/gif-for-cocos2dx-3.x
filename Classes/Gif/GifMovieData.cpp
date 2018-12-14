#include "cocos2d.h"
#include "GIFMovieData.h"
USING_NS_CC;

static void packARGB32(Rgba* pixel, GifByteType alpha, GifByteType red,
	GifByteType green, GifByteType blue)
{
	pixel->alpha = alpha;
	pixel->red = red;
	pixel->green = green;
	pixel->blue = blue;
}

static int DecodeCallBackProc(GifFileType* gif, GifByteType* bytes, int size)
{
	FileData* file = (FileData*)gif->UserData;
	return file->ReadData(bytes, size);
}

static void getColorFromTable(int idx, Rgba* dst, const ColorMapObject* cmap)
{
	char colIdx = idx >= cmap->ColorCount ? 0 : idx;
	GifColorType* col = &cmap->Colors[colIdx];
	packARGB32(dst, 0xFF, col->Red, col->Green, col->Blue);
}

GIFMovieData::GIFMovieData()
{   fGIF = NULL;
    fCurrIndex = -1;
    fLastDrawIndex = -1;
}

bool GIFMovieData::init( const char* file)
{
	if(file == NULL)
		return false;

	std::string fileName = cocos2d::FileUtils::getInstance()->fullPathForFilename(file);
	Data data = FileUtils::getInstance()->getDataFromFile(fileName.c_str());
	m_FileData.SetData(data.getBytes(), data.getSize());

	int error = 0;
	fGIF = DGifOpen( &m_FileData, &DecodeCallBackProc, &error );

	if (NULL == fGIF || DGifSlurp(fGIF) != GIF_OK)
	{
		DGifCloseFile(fGIF);
		fGIF = NULL;
		return false;
	}
	return true;
}

cocos2d::Texture2D* GIFMovieData::GetTexture( int index )
{
	uint32_t dur = duration();
	int nCount = getGifCount();
	if (index < 0)
		index = 0;
	if (index > nCount)
		index = nCount;
	uint32_t oldTime = getTime();
	setTime(dur / (float)nCount * index);

	Bitmap* btmp = bitmap();
	Image* pImg = new Image;
	pImg->autorelease();
	pImg->initWithRawData((unsigned char *)btmp->getRGBA(), btmp->getPixelLenth(), btmp->m_width, btmp->m_hight, 8);
	Texture2D* texture = new Texture2D;
	texture->autorelease();
	texture->initWithImage(pImg);

	setTime(oldTime);
	return texture;
}

cocos2d::Texture2D* GIFMovieData::StaticGetTexture( const char* file, int index )
{
	GIFMovieData* movie = GIFMovieData::create(file);
	uint32_t dur = movie->duration();
	int nCount = movie->getGifCount();
	if (index < 0)
		index = 0;
	if (index > nCount)
		index = nCount;
	movie->setTime( dur / (float)nCount * index );

	Bitmap* btmp = movie->bitmap();
	Image* pImg = new Image;
	pImg->autorelease();
	pImg->initWithRawData((unsigned char *)btmp->getRGBA(), btmp->getPixelLenth(), btmp->m_width, btmp->m_hight, 8);
	Texture2D* texture = new Texture2D;
	texture->autorelease();
	texture->initWithImage(pImg);

	delete movie;
	return texture;
}

GIFMovieData::~GIFMovieData()
{
	DGifCloseFile(fGIF);
	fGIF = NULL;
}
 
static uint32_t savedimage_duration(const SavedImage* image)
{
	uint32_t duration = 0;
    for (int j = 0; j < image->ExtensionBlockCount; j++)
    {
        if (image->ExtensionBlocks[j].Function == GRAPHICS_EXT_FUNC_CODE)
        {
            int size = image->ExtensionBlocks[j].ByteCount;
            //assert(size >= 4);
            if(size < 4) break;
            const uint8_t* b = (const uint8_t*)image->ExtensionBlocks[j].Bytes;
            duration =  ((b[2] << 8) | b[1]) * 10;
			break;
        }
    }
	duration = duration <= 50 ? 50 : duration;
    return duration;
}

GifFrame GIFMovieData::getGifFrameByIndex(unsigned int frameIndex)
{
	GifFrame gif;
    
	if(fGIF == NULL || frameIndex > getGifCount())
	{
		return gif;
	}
	int duration = savedimage_duration(&fGIF->SavedImages[frameIndex]);
	fCurrIndex = frameIndex;
	onGetBitmap(&m_bitmap);
    
	gif.m_frameData.m_duration = duration;
	gif.m_frameData.m_index = frameIndex;
    gif.m_bm = &m_bitmap;

	return gif;
}

bool GIFMovieData::onGetInfo(Info* info)
{
    if (NULL == fGIF)
        return false;
 
    uint32_t dur = 0;
    for (int i = 0; i < fGIF->ImageCount; i++)
        dur += savedimage_duration(&fGIF->SavedImages[i]);
 
    info->fDuration = dur;
    info->fWidth = fGIF->SWidth;
    info->fHeight = fGIF->SHeight;
    info->fIsOpaque = false;    // how to compute?
    return true;
}
 
bool GIFMovieData::onSetTime(uint32_t time)
{
	if (NULL == fGIF)
		return false;

	uint32_t dur = 0;
	for (int i = 0; i < fGIF->ImageCount; i++)
	{
		dur += savedimage_duration(&fGIF->SavedImages[i]);
		if (dur >= time)
		{
			fCurrIndex = i;
			return fLastDrawIndex != fCurrIndex;
		}
	}
	fCurrIndex = fGIF->ImageCount - 1;
    return true;
}

static void copyLine(Rgba* dst, const unsigned char* src, const ColorMapObject* cmap,
                    int transparent, int width)
{
	for (; width > 0; width--, src++, dst++) {
		if (*src != transparent) {
			const GifColorType& col = cmap->Colors[*src];
			*dst = Rgba(0xFF, col.Red, col.Green, col.Blue);
		}
	}
}
 
static void blitNormal(Bitmap* bm, const SavedImage* frame, const ColorMapObject* cmap,
                    int transparent)
{
	GifWord width = bm->m_width;
	GifWord height = bm->m_hight;
    const unsigned char* src = (unsigned char*)frame->RasterBits;
	Rgba* dst = bm->getAddr(frame->ImageDesc.Left, frame->ImageDesc.Top);

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
 
static void fillRect(Bitmap* bm, GifWord left, GifWord top, GifWord width, GifWord height,
                    Rgba col)
{
	int bmWidth = bm->m_width;
	int bmHeight = bm->m_hight;
	Rgba* dst = bm->getAddr(left, top);
	GifWord copyWidth = width;
    if (left + copyWidth > bmWidth) {
        copyWidth = bmWidth - left;
    }
 
    GifWord copyHeight = height;
    if (top + copyHeight > bmHeight) {
        copyHeight = bmHeight - top;
    }
 
	for (; copyHeight > 0; copyHeight--)
	{
		for(int wIndex = 0; wIndex < copyWidth; wIndex++, dst++)
		{
			*dst = col;
		}
	}
}
 
static void drawFrame(Bitmap* bm, const SavedImage* frame, const ColorMapObject* cmap)
{
    int transparent = -1;
 
    for (int i = 0; i < frame->ExtensionBlockCount; ++i) {
        ExtensionBlock* eb = frame->ExtensionBlocks + i;
        if (eb->Function == GRAPHICS_EXT_FUNC_CODE &&
            eb->ByteCount == 4) {
            bool has_transparency = ((eb->Bytes[0] & 1) == 1);
            if (has_transparency) {
                transparent = (unsigned char)eb->Bytes[3];
            }
        }
    }
 
    if (frame->ImageDesc.ColorMap != NULL) {
        // use local color table
        cmap = frame->ImageDesc.ColorMap;
    }
 
    if (cmap == NULL || cmap->ColorCount != (1 << cmap->BitsPerPixel)) {
        
        return;
    }
 
    blitNormal(bm, frame, cmap, transparent);   
}
 
static bool checkIfWillBeCleared(const SavedImage* frame)
{
    for (int i = 0; i < frame->ExtensionBlockCount; ++i) {
        ExtensionBlock* eb = frame->ExtensionBlocks + i;
        if (eb->Function == GRAPHICS_EXT_FUNC_CODE &&
            eb->ByteCount == 4) {
            // check disposal method
            int disposal = ((eb->Bytes[0] >> 2) & 7);
            if (disposal == 2 || disposal == 3) {
                return true;
            }
        }
    }
    return false;
}
 
static void getTransparencyAndDisposalMethod(const SavedImage* frame, bool* trans, int* disposal)
{
    *trans = false;
    *disposal = 0;
    for (int i = 0; i < frame->ExtensionBlockCount; ++i) {
        ExtensionBlock* eb = frame->ExtensionBlocks + i;
        if (eb->Function == GRAPHICS_EXT_FUNC_CODE &&
            eb->ByteCount == 4) {
            *trans = ((eb->Bytes[0] & 1) == 1);
            *disposal = ((eb->Bytes[0] >> 2) & 7);
        }
    }
}
 
// return true if area of 'target' is completely covers area of 'covered'
static bool checkIfCover(const SavedImage* target, const SavedImage* covered)
{
    if (target->ImageDesc.Left <= covered->ImageDesc.Left
        && covered->ImageDesc.Left + covered->ImageDesc.Width <=
            target->ImageDesc.Left + target->ImageDesc.Width
        && target->ImageDesc.Top <= covered->ImageDesc.Top
        && covered->ImageDesc.Top + covered->ImageDesc.Height <=
            target->ImageDesc.Top + target->ImageDesc.Height) {
        return true;
    }
    return false;
}
 
static void disposeFrameIfNeeded(Bitmap* bm, const SavedImage* cur, const SavedImage* next,
                                Bitmap* backup, Rgba color)
{
    // We can skip disposal process if next frame is not transparent
    // and completely covers current area
    bool curTrans;
    int curDisposal;
    getTransparencyAndDisposalMethod(cur, &curTrans, &curDisposal);
    bool nextTrans;
    int nextDisposal;
    getTransparencyAndDisposalMethod(next, &nextTrans, &nextDisposal);


	//#define DISPOSAL_UNSPECIFIED      0       /* No disposal specified. */
	//#define DISPOSE_DO_NOT            1       /* Leave image in place */
	//#define DISPOSE_BACKGROUND        2       /* Set area too background color */
	//#define DISPOSE_PREVIOUS          3       /* Restore to previous content */
	
    if ((curDisposal == 2 || curDisposal == 3)
        && (nextTrans || !checkIfCover(next, cur))) {
        switch (curDisposal) {
        // restore to background color
        // -> 'background' means background under this image.
        case 2:
            fillRect(bm, cur->ImageDesc.Left, cur->ImageDesc.Top,
                    cur->ImageDesc.Width, cur->ImageDesc.Height,
                    color);
            break;
 
        // restore to previous
        case 3:
			{
				bm->swap(backup);
			}
			
            break;
        }
    }
 
    // Save current image if next frame's disposal method == 3
	if (nextDisposal == 3)
		memcpy(backup->getAddr(0,0), bm->getAddr(0,0), bm->getPixelLenth() * sizeof(Rgba));
}

bool GIFMovieData::onGetBitmap(Bitmap* bm)
{
    const GifFileType* gif = fGIF;
    if (NULL == gif)
        return false;
 
    if (gif->ImageCount < 1) {
        return false;
    }
 
	const int width = bm->m_width = gif->SWidth;
	const int height = bm->m_hight = gif->SHeight;
	if (width <= 0 || height <= 0) {
		return false;
	}

	if (fLastDrawIndex >= 0 && fLastDrawIndex == fCurrIndex) {
		return true;
	}

	int startIndex = fLastDrawIndex + 1;
	if (fLastDrawIndex < 0 || !bm->isValid()) {
		// first time
		fBackup.m_width = width;
		fBackup.m_hight = height;
		bm->allocateBitmap();
		fBackup.allocateBitmap();
		startIndex = 0;
	} else if (startIndex > fCurrIndex) {
		// rewind to 1st frame for repeat
		startIndex = 0;
	}
 
    int lastIndex = fCurrIndex;
    if (lastIndex < 0) {
        // first time
        lastIndex = 0;
    } else if (lastIndex > fGIF->ImageCount - 1) {
        // this block must not be reached.
        lastIndex = fGIF->ImageCount - 1;
    }
 
	Rgba bgColor;
    if (gif->SColorMap != NULL) {
        const GifColorType& col = gif->SColorMap->Colors[fGIF->SBackGroundColor];
		bgColor.setColor(0xFF, col.Red, col.Green, col.Blue);
    }
 
	static Rgba paintingColor;
    // draw each frames - not intelligent way
	for (int i = startIndex; i <= lastIndex; i++) {
		const SavedImage* cur = &fGIF->SavedImages[i];
		if (i == 0) {
			bool trans;
			int disposal;
			getTransparencyAndDisposalMethod(cur, &trans, &disposal);
			if (!trans && gif->SColorMap != NULL) {
				paintingColor = bgColor;
			}

			bm->eraseColor(paintingColor);
			fBackup.eraseColor(paintingColor);
		} else {
			// Dispose previous frame before move to next frame.
			const SavedImage* prev = &fGIF->SavedImages[i-1];
			disposeFrameIfNeeded(bm, prev, cur, &fBackup, paintingColor);
		}

		// Draw frame
		// We can skip this process if this index is not last and disposal
		// method == 2 or method == 3
		if (i == lastIndex || !checkIfWillBeCleared(cur)) {
			drawFrame(bm, cur, gif->SColorMap);
		}
	}
 
    // save index
    fLastDrawIndex = lastIndex;

    return true;
}
