#pragma once
#include "GIFMovie.h"
#include "GifBase.h"

/**
 * @brief InstantGif will just decode some raw data when it init.
 The bitmap data will be parsed when need which be used to create CCTexture.
 */
class InstantGif : public GifBase
{
public:
    InstantGif();
    ~InstantGif();

    CREATE_CCOBJ_WITH_PARAM(InstantGif, const char *)
    CREATE_CCOBJ_WITH_PARAMS(InstantGif, FILE *, const char *)

    virtual bool init(const char *);
    virtual bool init(FILE *, const char *);
    virtual void updateGif(uint32_t delta);

    virtual std::string getGifFrameName(int index);

private:
    GIFMovie *m_Movie;
    uint32_t m_GifId;
    static uint32_t m_InstantGifId;
};
