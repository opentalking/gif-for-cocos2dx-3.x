#pragma once
#include "2d/CCSprite.h"
#include "Bitmap.h"

class GifBase : public cocos2d::Sprite
{
public:
    virtual void updateGif(uint32_t delta) = 0;
    virtual cocos2d::Texture2D *createTexture(Bitmap *bm, int index, bool getCache);

    virtual void update(float delta)
    {
        // if delta>1, generally speaking  the reason is the device is stuck
        if (delta > 1) {
            return;
        }

        uint32_t ldelta = (uint32_t)(delta * 1000);
        updateGif(ldelta);
    }

    virtual std::string getGifFrameName(int index)
    {
        return m_GifFullpath;
    }

protected:
    std::string m_GifFullpath;
};
