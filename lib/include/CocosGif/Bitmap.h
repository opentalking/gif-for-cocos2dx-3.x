#pragma once
// STD
#include <stdint.h>
#include <assert.h>
#include <cstdlib>
// CocosGif
#include "GifMacros.h"
#define UNINITIALIZED_UINT ((uint32_t) - 1)
using ColorType = uint8_t;

class Rgba
{
public:
    Rgba()
    {
        blue = 0;
        green = 0;
        red = 0;
        alpha = 0;
    }

    Rgba(ColorType ialpha, ColorType ired, ColorType igreen, ColorType iblue)
    {
        alpha = ialpha;
        red = ired;
        green = igreen;
        blue = iblue;
    }

    Rgba(Rgba &color)
    {
        alpha = color.alpha;
        blue = color.blue;
        green = color.green;
        red = color.red;
    }

    void setColor(ColorType ialpha, ColorType ired, ColorType igreen, ColorType iblue)
    {
        alpha = ialpha;
        red = ired;
        green = igreen;
        blue = iblue;
    }

    ColorType red;
    ColorType green;
    ColorType blue;
    ColorType alpha;
};

class Bitmap
{
public:
    Bitmap();
    virtual ~Bitmap();

    void allocateBitmap();
    void resetBitmap();
    bool hasData();

    bool isValid();
    uint32_t getPixelLenth();
    const uint32_t *getRGBA();

    Rgba *getAddr(int left, int top);
    void swap(Bitmap *);
    void eraseColor(Rgba color);

    static Bitmap *getDebugBitmap();

    uint32_t getWidth() const;
    void setWidth(const uint32_t &width);
    uint32_t getHeight() const;
    void setHeight(const uint32_t &height);


private:
    Rgba *m_Data;
    uint32_t m_Width;
    uint32_t m_Height;
};

struct FrameData {
    uint32_t duration;
    uint32_t index;

    FrameData(uint32_t duration, uint32_t index)
    {
        duration = duration;
        index = index;
    }
};

struct GifFrame {
public:
    GifFrame()
        : frameData(UNINITIALIZED_UINT, UNINITIALIZED_UINT)
    {
    }

    Bitmap *bitmap;
    FrameData frameData;
};
