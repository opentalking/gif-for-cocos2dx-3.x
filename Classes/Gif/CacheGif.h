#ifndef CACHEGIF_H
#define CACHEGIF_H

#include "cocos2d.h"
#include "GIFMovie.h"
#include <vector>
#include "GifBase.h"

class GifSprieFrame
{
public:
	GifSprieFrame()
		:m_data(0,UNINITIALIZED_UINT)
		,m_frame(NULL)
	{

	}
	virtual ~GifSprieFrame()
	{
		CC_SAFE_RELEASE(m_frame);
	}

	void setFrameData(FrameData data)
	{
		m_data = data;
	}

	cocos2d::SpriteFrame* getSpriteFrame()
	{
		return m_frame;
	}

	uint32_t Index()
	{
		return m_data.m_index;
	}

	uint32_t Duration()
	{
		return m_data.m_duration;
	}

	void setSpriteFrame(cocos2d::SpriteFrame*);

private:
	FrameData m_data;
	cocos2d::SpriteFrame* m_frame;
};

/*
 The CacheGif will  parse each frame of the bitmap data, and create CCSpriteFrameCache to cache when init it.
 The advantage of playing fast, the biggest drawback is the cost too long time to create
 */
class CacheGif : public GifBase
{
public:
	CREATE_CCOBJ_WITH_PARAM(CacheGif,const char*);
	virtual bool init(const char*);

	CREATE_CCOBJ_WITH_PARAMS(CacheGif, FILE*, const char*);
	virtual bool init(FILE*,const char*);
	CacheGif();
	~CacheGif();
	virtual void updateGif(uint32_t delta);
	
	bool initGifData(GIFMovie* movie);
protected:
	virtual std::string getGifFrameName(int index);
	virtual cocos2d::SpriteFrame* getGifSpriteFrame(Bitmap* bm, int index);
	void addGifSpriteFrame(GifFrame& frame);
private:
	int m_duration;
	int m_progress;
	int m_currentIndex;
	std::vector<GifSprieFrame*> m_frameData;
};



#endif//CACHEGIF_H