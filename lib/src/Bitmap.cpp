#include "Bitmap.h"

Bitmap::Bitmap()
{
    m_Width = 0;
    m_Height = 0;
    m_Data = nullptr;
}

Bitmap::~Bitmap()
{
    resetBitmap();
}

void Bitmap::allocateBitmap()
{
    assert(m_Data == nullptr && m_Width * m_Height > 0);
    m_Data = (Rgba *)malloc(m_Width * m_Height * sizeof(Rgba));
}

void Bitmap::resetBitmap()
{
    if (m_Data) {
        free(m_Data);
        m_Data = nullptr;
    }
}

bool Bitmap::hasData()
{
    return m_Data != nullptr;
}

bool Bitmap::isValid()
{
    return m_Width > 0 && m_Height > 0 && hasData();
}

uint32_t Bitmap::getPixelLenth()
{
    return m_Width * m_Height;
}

const uint32_t *Bitmap::getRGBA()
{
    if (m_Data == nullptr) {
        return nullptr;
    }
    return (uint32_t *) m_Data;
}

void Bitmap::eraseColor(Rgba color)
{
    Rgba paintColor = color;

    // make rgb premultiplied
    if (255 != color.alpha) {
        paintColor.red = AlphaMul(color.red, color.alpha);
        paintColor.green = AlphaMul(color.green, color.alpha);
        paintColor.blue = AlphaMul(color.blue, color.alpha);
    }

    for (uint32_t i = 0; i < m_Width * m_Height; i++)
        *(m_Data + i) = paintColor;
}

Bitmap *Bitmap::getDebugBitmap()
{
    Bitmap *bitmap = new Bitmap;
    bitmap->setWidth(64);
    bitmap->setHeight(64);
    bitmap->allocateBitmap();

    for (uint32_t hight = 0; hight < bitmap->m_Height; hight++) {
        Rgba color;
        color.alpha = 255;

        if (hight < 20) {
            color.red = 255;
            color.green = 0;
            color.blue = 0;
        }
        else if (hight >= 20 && hight < 40) {
            color.red = 0;
            color.green = 255;
            color.blue = 0;
        }
        else {
            color.red = 0;
            color.green = 0;
            color.blue = 255;
        }

        for (uint32_t width = 0; width < bitmap->m_Width; width++) {
            Rgba &colorPixel = bitmap->m_Data[hight * bitmap->m_Width + width];
            colorPixel = color;
        }
    }

    return bitmap;
}

uint32_t Bitmap::getWidth() const
{
    return m_Width;
}

void Bitmap::setWidth(const uint32_t &width)
{
    m_Width = width;
}

uint32_t Bitmap::getHeight() const
{
    return m_Height;
}

void Bitmap::setHeight(const uint32_t &height)
{
    m_Height = height;
}

Rgba *Bitmap::getAddr(int left, int top)
{
    return m_Data + top * m_Width + left;
}

void Bitmap::swap(Bitmap *toSwap)
{
    TSwap(this->m_Data, toSwap->m_Data);
    TSwap(this->m_Width, toSwap->m_Width);
    TSwap(this->m_Height, toSwap->m_Height);
}
