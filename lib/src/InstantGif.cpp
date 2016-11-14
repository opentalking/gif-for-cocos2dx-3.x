#include "InstantGif.h"
// Cocos2dx
#include "base/CCDirector.h"
#include "renderer/CCTextureCache.h"

uint32_t InstantGif::m_InstantGifId = 0;

InstantGif::InstantGif()
{
    m_Movie = nullptr;

    m_InstantGifId++;
    if (m_InstantGifId == UNINITIALIZED_UINT) {
        m_InstantGifId = 0;
    }

    // gif id.
    m_GifId = m_InstantGifId;
}

InstantGif::~InstantGif()
{
    CC_SAFE_DELETE(m_Movie);
    std::string texture = getGifFrameName(0);
    cocos2d::Director::getInstance()->getTextureCache()->removeTextureForKey(texture.c_str());
}

bool InstantGif::init(const char *fileName)
{
    FILE *f = GifUtils::openFile(fileName);
    return init(f, fileName);
}

bool InstantGif::init(FILE *f, const char *fileName)
{
    m_GifFullpath = fileName;
    if (GifUtils::isGifFile(f) == false) {
        GifUtils::closeFile(f);
        return false;
    }

    m_Movie = GIFMovie::create(f);
    if (m_Movie == NULL || m_Movie->getGifCount() <= 0) {
        return false;
    }

    if (m_Movie->getGifCount() > 1) {
        scheduleUpdate();
    }
    m_Movie->setTime(0);
    cocos2d::Texture2D *texture = createTexture(m_Movie->bitmap(), 0, false);

    return initWithTexture(texture);
}

std::string InstantGif::getGifFrameName(int index)
{
    // Use id to create framename or texturename, to ensure multiple gifs created under the same path, not confusion in the texture cache
    return m_GifFullpath + "_instant_" + std::to_string(m_InstantGifId);
}

void InstantGif::updateGif(uint32_t delta)
{
    if (m_Movie->appendTime(delta)) {
        cocos2d::Texture2D *texture = createTexture(m_Movie->bitmap(), 0, false);
        this->setTexture(texture);
    }
}
