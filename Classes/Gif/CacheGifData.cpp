#include "CacheGifData.h"
#include "GIFMovieData.h"

USING_NS_CC;

CacheGifData::CacheGifData()
{
	m_duration = 0;
	m_progress = 0;
	m_currentIndex = UNINITIALIZED_UINT;
}

CacheGifData::~CacheGifData()
{
	if(m_frameData.size() > 0)
	for(std::vector<GifSprieFrame*>::iterator iter = m_frameData.begin(); iter != m_frameData.end(); )
	{
		GifSprieFrame* sprite = *iter;
		++iter;

		uint32_t index = sprite->Index();
		std::string gifFrameName = getGifFrameName(index);

		do 
		{
			SpriteFrame* spriteFrame   = sprite->getSpriteFrame();
			CC_BREAK_IF(spriteFrame == NULL);

			bool spriteFrameInCache = SpriteFrameCache::getInstance()->getSpriteFrameByName(gifFrameName.c_str()) == spriteFrame;
            //1. just GifSprieFrame retain
            //2. CCSpriteFrameCache and GifSprieFrame retain
            //more. other gif CacheGifData retain
			if(spriteFrame->getReferenceCount() == 1 || (spriteFrame->getReferenceCount() ==2 && spriteFrameInCache))
			{
				Texture2D* texture = sprite->getSpriteFrame()->getTexture();
                Director::getInstance()->getTextureCache()->removeTexture(texture);
				SpriteFrameCache::getInstance()->removeSpriteFramesFromTexture(texture);

			}
		} while (0);

		delete sprite;
	}
}

/*
 The FILE* will be closed after the function
 */
bool CacheGifData::init(const char* name)
{
	if (!fileEndWithGif(name))
		return false;
	std::string fileName = cocos2d::FileUtils::getInstance()->fullPathForFilename(name);
	m_gif_fullpath = fileName;
	GIFMovieData* movie = GIFMovieData::create(fileName.c_str());
	bool res = false;
	do
	{
		CC_BREAK_IF( movie == NULL);
		CC_BREAK_IF(initGifData(movie) == false);
		res = this->initWithSpriteFrame(m_frameData[0]->getSpriteFrame());//init CCSprite with the first frame
	} while (0);
    
	CC_SAFE_DELETE(movie);
    
	if(res && m_frameData.size()>1)
		scheduleUpdate();
	return res;
}

std::string CacheGifData::getGifFrameName(int index)
{
	const char* frame = CCString::createWithFormat("%s_%d",m_gif_fullpath.c_str(), index)->getCString();
	return frame;
}

// get CCSpriteFrame from cache or create with Bitmap's data
SpriteFrame* CacheGifData::getGifSpriteFrame(Bitmap* bm, int index)
{
	std::string gifFrameName = getGifFrameName(index);
	SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(gifFrameName.c_str());
	if(spriteFrame != NULL)
	{
		return spriteFrame;
	}
	
	do 
	{	
		Texture2D* texture = createTexture(bm,index,true);
		CC_BREAK_IF(! texture);

		spriteFrame = SpriteFrame::createWithTexture(texture, Rect(0,0,texture->getContentSize().width, texture->getContentSize().height));
		CC_BREAK_IF(! spriteFrame);

		SpriteFrameCache::getInstance()->addSpriteFrame(spriteFrame, gifFrameName.c_str());
	} while (0);


	return spriteFrame;
}

bool CacheGifData::initGifData(GIFMovieData* movie)
{
    if(movie == NULL) return false;

	int gifCount = movie->getGifCount();
	for(int i = 0; i < gifCount; i++)
	{
		GifFrame gifFrame = movie->getGifFrameByIndex(i);
		if(gifFrame.m_bm == NULL || !gifFrame.m_bm->isValid())
		{
			continue;
		}
		addGifSpriteFrame(gifFrame);
	}

	if(m_frameData.size() <= 0)
	{
		return false;
	}

	return true;
}

void CacheGifData::updateGif(uint32_t delta)
{
	m_progress += delta;

	if(m_progress > m_duration)
	{
		m_progress = m_progress - m_duration;
	}

	uint32_t dur = 0;
	for (int i = 0; i < m_frameData.size(); i++)
	{
		dur += m_frameData[i]->Duration();
		if (dur >= m_progress)// If the progress bigger than sum of pre frame duration，change new CCSpriteFrame to display
		{
			if(m_frameData[i]->Index() != m_currentIndex)
			{
				this->setDisplayFrame(m_frameData[i]->getSpriteFrame());
			}
			m_currentIndex = m_frameData[i]->Index();

			return ;
		}
	}
}

void CacheGifData::addGifSpriteFrame(GifFrame& gifFrame)
{
    if(gifFrame.m_frameData.m_index == UNINITIALIZED_UINT)
    {
        return;
    }
    
	if(m_frameData.size() > 0)
	{
		for(std::vector<GifSprieFrame*>::iterator iter = m_frameData.begin(); iter != m_frameData.end(); iter++)
		{
			if((*iter)->Index() == gifFrame.m_frameData.m_index)
			{
				return ;
			}
		}
	}

	cocos2d::SpriteFrame* spriteFrame = getGifSpriteFrame(gifFrame.m_bm, gifFrame.m_frameData.m_index);
    
	GifSprieFrame* frame = new GifSprieFrame;
	frame->setFrameData(gifFrame.m_frameData);
	frame->setSpriteFrame(spriteFrame);
	m_frameData.push_back(frame);
    
	m_duration += frame->Duration();
}