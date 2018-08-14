#include "precomp.h"
#include "jprecitimer.h"

#ifdef _MSC_VER

JPreciTimer::JPreciTimer() : q_freq(0), q_count(0)
{
    LARGE_INTEGER lcount;
    if (QueryPerformanceFrequency(&lcount)) {
        q_freq = lcount.QuadPart;
    }

    reset();
}

JPreciTimer::~JPreciTimer()
{
}

void JPreciTimer::reset()
{
    LARGE_INTEGER lcount;
    if (QueryPerformanceCounter(&lcount)) {
        q_count = lcount.QuadPart;
    }
    else {
        q_count = 0;
    }
}

#else

#endif
