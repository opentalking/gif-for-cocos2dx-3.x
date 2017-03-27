#ifndef INSTANTGIFDATA_H
#define INSTANTGIFDATA_H

#include "GIFMovieData.h"
#include "GifBase.h"

/*
 InstantGifData will just decode some raw data when it init.
 The bitmap data will be parsed when need which be used to create CCTexture.
 */
class InstantGifData : public GifBase
{
public:
	static InstantGifData* create( const char* filename);
	virtual bool init(const char*);

	InstantGifData();
	~InstantGifData();
	virtual void updateGif(uint32_t delta);
	virtual std::string getGifFrameName(int index);
private:
	GIFMovieData* m_movie;
	uint32_t m_instantGifId;
};



#endif//INSTANTGIF_H