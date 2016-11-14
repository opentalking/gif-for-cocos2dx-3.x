#pragma once
#include "cocos2d.h"
#include "GIFMovie.h"
#include <vector>
#include "GifBase.h"

class GifSprieFrame
{
public:
    GifSprieFrame()
        : m_Data(0, UNINITIALIZED_UINT)
        , m_Frame(NULL)
    {

    }
    virtual ~GifSprieFrame()
    {
        CC_SAFE_RELEASE(m_Frame);
    }

    void setFrameData(FrameData data)
    {
        m_Data = data;
    }

    cocos2d::SpriteFrame *getSpriteFrame()
    {
        return m_Frame;
    }

    uint32_t Index()
    {
        return m_Data.index;
    }

    uint32_t Duration()
    {
        return m_Data.duration;
    }

    void setSpriteFrame(cocos2d::SpriteFrame *);

private:
    FrameData m_Data;
    cocos2d::SpriteFrame *m_Frame;
};

/**
 * @brief The CacheGif will  parse each frame of the bitmap data, and create CCSpriteFrameCache to cache when init it.
 * The advantage of playing fast, the biggest drawback is the cost too long time to create
 */
class CacheGif : public GifBase
{
public:
    CacheGif();
    ~CacheGif();

    CREATE_CCOBJ_WITH_PARAM(CacheGif, const char *)
    CREATE_CCOBJ_WITH_PARAMS(CacheGif, FILE *, const char *)

    virtual bool init(const char *);

    /**
     * @brief The FILE* will be closed after the function
     * @return
     */
    virtual bool init(FILE *, const char *);
    virtual void updateGif(uint32_t delta);

    bool initGifData(GIFMovie *movie);

protected:
    virtual std::string getGifFrameName(int index);

    /**
     * @brief get CCSpriteFrame from cache or create with Bitmap's data
     * @param bm
     * @param index
     * @return
     */
    virtual cocos2d::SpriteFrame *getGifSpriteFrame(Bitmap *bm, int index);
    void addGifSpriteFrame(GifFrame &frame);

private:
    unsigned int m_Duration;
    unsigned int m_Progress;
    unsigned int m_CurrentIndex;
    std::vector<GifSprieFrame *> m_FrameData;
};
