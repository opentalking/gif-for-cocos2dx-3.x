#include "Movie.h"
// We should never see this in normal operation since our time values are
// 0-based. So we use it as a sentinal.
#define UNINITIALIZED_MSEC ((uint32_t)-1)

Movie::Movie()
{
    // uninitialized
    m_Info.duration = UNINITIALIZED_MSEC;

    // uninitialized
    m_CurrTime = UNINITIALIZED_MSEC;
    m_NeedBitmap = true;
}

void Movie::ensureInfo()
{
    if (m_Info.duration == UNINITIALIZED_MSEC && !this->onGetInfo(&m_Info)) {
        // failure
        memset(&m_Info, 0, sizeof(m_Info));
    }
}

uint32_t Movie::duration()
{
    this->ensureInfo();
    return m_Info.duration;
}

int Movie::width()
{
    this->ensureInfo();
    return m_Info.width;
}

int Movie::height()
{
    this->ensureInfo();
    return m_Info.height;
}

int Movie::isOpaque()
{
    this->ensureInfo();
    return m_Info.isOpaque;
}

bool Movie::setTime(uint32_t time)
{
    uint32_t dur = this->duration();
    if (time > dur) {
        time = time % dur;
    }

    bool changed = false;
    if (time != m_CurrTime) {
        m_CurrTime = time;
        changed = this->onSetTime(time);
        m_NeedBitmap |= changed;
    }

    return changed;
}

bool Movie::appendTime(uint32_t time)
{
    uint32_t allTime = m_CurrTime + time;
    return setTime(allTime);
}

Bitmap *Movie::bitmap()
{
    if (m_CurrTime == UNINITIALIZED_MSEC) {
        // uninitialized
        this->setTime(0);
    }

    if (m_NeedBitmap) {
        // failure
        if (!this->onGetBitmap(&m_Bitmap)) {
            return nullptr;
        }

        m_NeedBitmap = false;
    }
    return &m_Bitmap;
}
