#pragma once
#include <stdint.h>
#include "gif_lib/gif_lib.h"
#include <stdio.h>
#include <string.h>
#include "Bitmap.h"

class Movie
{
public:
    uint32_t duration();
    int width();
    int height();

    int isOpaque();

    /**
     * @brief Specify the time code (between 0...duration) to sample a bitmap from the movie.
     * Returns true if this time code generated a different bitmap/frame from the previous state (i.e. true means you need to redraw).
     * @return
     */
    bool setTime(uint32_t);
    bool appendTime(uint32_t time);

    /**
     * @brief return the right bitmap for the current time code
     * @return
     */
    Bitmap *bitmap();

protected:
    Bitmap m_Bitmap;

protected:
    struct Info {
        uint32_t duration;
        int width;
        int height;
        bool isOpaque;
    };

    Movie();

    virtual bool onGetInfo(Info *) = 0;
    virtual bool onSetTime(uint32_t) = 0;
    virtual bool onGetBitmap(Bitmap *) = 0;

private:
    Info m_Info;
    uint32_t m_CurrTime;
    bool m_NeedBitmap;

    void ensureInfo();
};
