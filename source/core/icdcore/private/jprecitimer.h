
//! 功能：使用系统的时间计数器得到精确的时间
//! 结构：
//! 说明：

#ifndef JPRECITIMER_H
#define JPRECITIMER_H

#ifdef _MSC_VER
#include <Windows.h>
#include <assert.h>

class JPreciTimer
{
public:
    explicit JPreciTimer(void);
    ~JPreciTimer(void);

    //
    void reset();

    // 返回CPU计数器值(us)
    inline LONGLONG uscount() const
    {
        LARGE_INTEGER lcount;
        QueryPerformanceCounter(&lcount);
        return lcount.QuadPart * 1000000UL / q_freq;
    }

    // 返回CPU计数器值(ms)
    inline LONGLONG mscount() const
    {
        LARGE_INTEGER lcount;
        QueryPerformanceCounter(&lcount);
        return lcount.QuadPart * 1000UL / q_freq;
    }

    // 返回从初始化到现在的时间间隔(us)
    inline LONGLONG ustick() const
    {
        LARGE_INTEGER lcount;
        QueryPerformanceCounter(&lcount);
        return (lcount.QuadPart - q_ocount) * 1000000UL / q_freq;
    }

    // 返回从初始化到现在的时间间隔(ms)
    inline LONGLONG mstick() const
    {
        LARGE_INTEGER lcount;
        QueryPerformanceCounter(&lcount);
        return (lcount.QuadPart - q_ocount) * 1000UL / q_freq;
    }

private:
    LONGLONG	q_freq;		// CPU频率
    LONGLONG	q_ocount;	// 对象初始化时的时间
};
#else
#error("unupported operate system!")
#endif

#endif  // JPRECITIMER_H
