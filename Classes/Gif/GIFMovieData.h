#ifndef GIFMovieData_H
#define GIFMovieData_H

#include "Movie.h"
#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <assert.h>
#include "GifUtils.h"
#include "cocos2d.h"
#include "FileData.h"

class GIFMovieData : public Movie {
public:
	virtual ~GIFMovieData();
	GIFMovieData();
	CREATE_WITH_PARAM(GIFMovieData, const char*);
    virtual bool init(const char*);

	cocos2d::Texture2D* GetTexture(int index = 0);
	static cocos2d::Texture2D* StaticGetTexture( const char* file, int index = 0 );

	int getGifCount()
	{
		return fGIF == NULL ? 0 : fGIF->ImageCount;
	};

	int getCurrentIndex()
	{
		return fCurrIndex;
	};
	GifFrame getGifFrameByIndex(unsigned int frameIndex);
protected:
	virtual bool onGetInfo(Info*);
	virtual bool onSetTime(uint32_t);
	virtual bool onGetBitmap(Bitmap*);

private:
	FileData	m_FileData;
	GifFileType* fGIF;
	int fCurrIndex;
	int fLastDrawIndex;
	Bitmap fBackup;
};

#endif