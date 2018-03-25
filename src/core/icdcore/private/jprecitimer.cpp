//#include "precomp.h"
#include "jprecitimer.h"

JPreciTimer::JPreciTimer() : q_freq(0), q_ocount(0)
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
        q_ocount = lcount.QuadPart;
    }
    else {
        q_ocount = 0;
    }
}
