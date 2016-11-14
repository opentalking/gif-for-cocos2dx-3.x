#include "CacheGif.h"
#include "GIFMovie.h"
USING_NS_CC;

CacheGif::CacheGif()
    : m_Duration(0)
    , m_Progress(0)
    , m_CurrentIndex(UNINITIALIZED_UINT)
{

}

CacheGif::~CacheGif()
{
    if (m_FrameData.size() > 0) {
        for (std::vector<GifSprieFrame *>::iterator iter = m_FrameData.begin(); iter != m_FrameData.end();) {
            GifSprieFrame *sprite = *iter;
            ++iter;

            uint32_t index = sprite->Index();
            std::string gifFrameName = getGifFrameName(index);

            do {
                SpriteFrame *spriteFrame   = sprite->getSpriteFrame();
                CC_BREAK_IF(spriteFrame == nullptr);

                bool spriteFrameInCache = SpriteFrameCache::getInstance()->getSpriteFrameByName(gifFrameName.c_str()) == spriteFrame;
                // 1. just GifSprieFrame retain
                // 2. CCSpriteFrameCache and GifSprieFrame retain
                // more. other gif CacheGif retain
                if (spriteFrame->getReferenceCount() == 1 || (spriteFrame->getReferenceCount() == 2 && spriteFrameInCache)) {
                    Texture2D *texture = sprite->getSpriteFrame()->getTexture();
                    Director::getInstance()->getTextureCache()->removeTexture(texture);
                    SpriteFrameCache::getInstance()->removeSpriteFramesFromTexture(texture);

                }
            }
            while (0);

            delete sprite;
        }
    }
}

bool CacheGif::init(const char *fileName)
{
    FILE *f = GifUtils::openFile(fileName);
    return init(f, fileName);
}

bool CacheGif::init(FILE *f, const char *fileName)
{
    m_GifFullpath = fileName;
    if (GifUtils::isGifFile(f) == false) {
        if (f) {
            fclose(f);
        }

        return false;
    }

    GIFMovie *movie = GIFMovie::create(f);
    bool res = false;
    do {
        CC_BREAK_IF(movie == nullptr);
        CC_BREAK_IF(initGifData(movie) == false);
        // init CCSprite with the first frame
        res = this->initWithSpriteFrame(m_FrameData[0]->getSpriteFrame());
    }
    while (0);

    CC_SAFE_DELETE(movie);

    if (res && m_FrameData.size() > 1) {
        scheduleUpdate();
    }

    return res;
}

std::string CacheGif::getGifFrameName(int index)
{
    const char *frame = CCString::createWithFormat("%s_%d", m_GifFullpath.c_str(), index)->getCString();
    return frame;
}

SpriteFrame *CacheGif::getGifSpriteFrame(Bitmap *bm, int index)
{
    std::string gifFrameName = getGifFrameName(index);
    SpriteFrame *spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(gifFrameName.c_str());
    if (spriteFrame != nullptr) {
        return spriteFrame;
    }

    do {
        Texture2D *texture = createTexture(bm, index, true);
        CC_BREAK_IF(!texture);

        spriteFrame = SpriteFrame::createWithTexture(texture, Rect(0, 0, texture->getContentSize().width, texture->getContentSize().height));
        CC_BREAK_IF(!spriteFrame);

        SpriteFrameCache::getInstance()->addSpriteFrame(spriteFrame, gifFrameName.c_str());
    }
    while (0);


    return spriteFrame;
}

bool CacheGif::initGifData(GIFMovie *movie)
{
    if (movie == nullptr) return false;

    int gifCount = movie->getGifCount();
    for (int i = 0; i < gifCount; i++) {
        GifFrame gifFrame = movie->getGifFrameByIndex(i);
        if (gifFrame.bitmap == nullptr || !gifFrame.bitmap->isValid()) {
            continue;
        }
        addGifSpriteFrame(gifFrame);
    }

    if (m_FrameData.size() <= 0) {
        return false;
    }

    return true;
}

void CacheGif::updateGif(uint32_t delta)
{
    m_Progress += delta;

    if (m_Progress > m_Duration) {
        m_Progress = m_Progress - m_Duration;
    }

    uint32_t dur = 0;
    for (unsigned int i = 0; i < m_FrameData.size(); i++) {
        dur += m_FrameData[i]->Duration();

        // If the progress bigger than sum of pre frame duration，change new CCSpriteFrame to display
        if (dur >= m_Progress) {
            if (m_FrameData[i]->Index() != m_CurrentIndex) {
                this->setDisplayFrame(m_FrameData[i]->getSpriteFrame());
            }

            m_CurrentIndex = m_FrameData[i]->Index();

            return ;
        }
    }
}

void CacheGif::addGifSpriteFrame(GifFrame &gifFrame)
{
    if (gifFrame.frameData.index == UNINITIALIZED_UINT) {
        return;
    }

    if (m_FrameData.size() > 0) {
        for (std::vector<GifSprieFrame *>::iterator iter = m_FrameData.begin(); iter != m_FrameData.end(); iter++) {
            if ((*iter)->Index() == gifFrame.frameData.index) {
                return ;
            }
        }
    }

    cocos2d::SpriteFrame *spriteFrame = getGifSpriteFrame(gifFrame.bitmap, gifFrame.frameData.index);

    GifSprieFrame *frame = new GifSprieFrame;
    frame->setFrameData(gifFrame.frameData);
    frame->setSpriteFrame(spriteFrame);
    m_FrameData.push_back(frame);

    m_Duration += frame->Duration();
}

void GifSprieFrame::setSpriteFrame(cocos2d::SpriteFrame *frame)
{
    CC_SAFE_RETAIN(frame);
    CC_SAFE_RELEASE(m_Frame);
    m_Frame = frame;
}
