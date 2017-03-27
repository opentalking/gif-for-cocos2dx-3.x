#include "InstantGifData.h"
#include "cocos2d.h"

static uint32_t InstantGifDataId = 0;

InstantGifData* InstantGifData::create(const char* filename)
{
	InstantGifData* pRet = new InstantGifData;
	if (pRet && pRet->init(filename))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		return nullptr;
	}
}

bool InstantGifData::init( const char* name)
{
	if (!fileEndWithGif(name))
		return false;

	std::string fileName = cocos2d::FileUtils::getInstance()->fullPathForFilename(name);
	m_gif_fullpath = fileName;
	m_movie = GIFMovieData::create(fileName.c_str());
	if(m_movie == NULL || m_movie->getGifCount() <= 0)
		return false;

	if(m_movie->getGifCount()>1)
		scheduleUpdate();
	m_movie->setTime(0);
	cocos2d::Texture2D* texture = createTexture(m_movie->bitmap(),0,false);
	return initWithTexture(texture);
}

InstantGifData::InstantGifData()
{
	m_movie = NULL;

	InstantGifDataId++;
	if(InstantGifDataId == UNINITIALIZED_UINT)
	{
		InstantGifDataId = 0;
	}
	m_instantGifId = InstantGifDataId;// gif id.
}

std::string InstantGifData::getGifFrameName(int index)
{
    // Use id to create framename or texturename, to ensure multiple gifs created under the same path, not confusion in the texture cache
	return cocos2d::CCString::createWithFormat("%s_instant_%u",m_gif_fullpath.c_str(),m_instantGifId)->getCString();
}

InstantGifData::~InstantGifData()
{
	CC_SAFE_DELETE(m_movie);
	std::string texture = getGifFrameName(0);
    cocos2d::Director::getInstance()->getTextureCache()->removeTextureForKey(texture.c_str());
}

void InstantGifData::updateGif(uint32_t delta)
{
	if(m_movie->appendTime(delta))
	{
		cocos2d::Texture2D* texture = createTexture(m_movie->bitmap(),0,false);
		this->setTexture(texture);
	}
}

