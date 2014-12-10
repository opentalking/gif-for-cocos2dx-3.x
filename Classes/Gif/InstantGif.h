#ifndef INSTANTGIF_H
#define INSTANTGIF_H

#include "GIFMovie.h"
#include "GifBase.h"

/*
 InstantGif will just decode some raw data when it init.
 The bitmap data will be parsed when need which be used to create CCTexture.
 */
class InstantGif : public GifBase
{
public:
	CREATE_CCOBJ_WITH_PARAM(InstantGif,const char*);
	virtual bool init(const char*);

	CREATE_CCOBJ_WITH_PARAMS(InstantGif, FILE*, const char*);
	virtual bool init(FILE*,const char*);
	InstantGif();
	~InstantGif();
	virtual void updateGif(uint32_t delta);
	virtual std::string getGifFrameName(int index);
private:
	GIFMovie* m_movie;
	uint32_t m_instantGifId;
};



#endif//INSTANTGIF_H