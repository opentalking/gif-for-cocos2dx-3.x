#ifndef GIFMovie_H
#define GIFMovie_H

#include "Movie.h"
#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <assert.h>
#include "GifUtils.h"

class GIFMovie : public Movie {
public:
	virtual ~GIFMovie();
	GIFMovie();
	CREATE_WITH_PARAM(GIFMovie, const char*);
    virtual bool init(const char*);
	CREATE_WITH_PARAM(GIFMovie, FILE*);
	virtual bool init(FILE*);

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
	GifFileType* fGIF;
	int fCurrIndex;
	int fLastDrawIndex;
	Bitmap fBackup;
};

#endif