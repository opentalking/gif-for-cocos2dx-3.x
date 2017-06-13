#ifndef CACHEGIFDATA_H
#define CACHEGIFDATA_H

#include "CacheGif.h"
#include "GIFMovieData.h"

/*
 The CacheGifData will  parse each frame of the bitmap data, and create CCSpriteFrameCache to cache when init it.
 The advantage of playing fast, the biggest drawback is the cost too long time to create
 */
class CacheGifData : public GifBase
{
public:
	CREATE_CCOBJ_WITH_PARAM(CacheGifData,const char*);
	virtual bool init(const char*);
	CacheGifData();
	~CacheGifData();
	virtual void updateGif(uint32_t delta);
	
	bool initGifData(GIFMovieData* movie);
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