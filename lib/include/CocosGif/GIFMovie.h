#pragma once
#include "Movie.h"
#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <assert.h>
#include "GifUtils.h"

class GIFMovie : public Movie
{
public:
    GIFMovie();
    virtual ~GIFMovie();

    CREATE_WITH_PARAM(GIFMovie, const char *)
    CREATE_WITH_PARAM(GIFMovie, FILE *)

    virtual bool init(const char *);
    virtual bool init(FILE *);

    int getGifCount()
    {
        return m_GIF == NULL ? 0 : m_GIF->ImageCount;
    }

    int getCurrentIndex()
    {
        return m_CurrentIndex;
    }

    GifFrame getGifFrameByIndex(unsigned int frameIndex);

    static void packARGB32(Rgba *pixel, GifByteType alpha, GifByteType red, GifByteType green, GifByteType blue);
    static int decodeCallBackProc(GifFileType *gif, GifByteType *bytes, int size);
    static void getColorFromTable(int idx, Rgba *dst, const ColorMapObject *cmap);

    static void copyLine(Rgba *dst, const unsigned char *src, const ColorMapObject *cmap, int transparent, int width);
    static void blitNormal(Bitmap *bm, const SavedImage *frame, const ColorMapObject *cmap, int transparent);
    static void fillRect(Bitmap *bm, GifWord left, GifWord top, GifWord width, GifWord height, Rgba col);

    static void drawFrame(Bitmap *bm, const SavedImage *frame, const ColorMapObject *cmap);
    static bool checkIfWillBeCleared(const SavedImage *frame);
    static void getTransparencyAndDisposalMethod(const SavedImage *frame, bool *trans, int *disposal);

    /**
     * @brief Return true if area of 'target' is completely covers area of 'covered'
     * @param target
     * @param covered
     * @return
     */
    static bool checkIfCover(const SavedImage *target, const SavedImage *covered);
    static void disposeFrameIfNeeded(Bitmap *bm, const SavedImage *cur, const SavedImage *next, Bitmap *backup, Rgba color);

protected:
    virtual bool onGetInfo(Info *info);
    virtual bool onSetTime(uint32_t time);
    virtual bool onGetBitmap(Bitmap *);

private:
    GifFileType *m_GIF;
    int m_CurrentIndex;
    int m_LastDrawIndex;
    Bitmap m_BackupBitmap;
};
