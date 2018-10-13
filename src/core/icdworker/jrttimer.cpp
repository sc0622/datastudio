#include "precomp.h"
#include "jrttimer.h"

#ifdef _MSC_VER
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#elif defined(__linux__)
//TODO
#elif defined(__APPLE__)
//TODO
#endif
namespace Icd {

// JRTTimerData

class JRTTimerData
{
    friend class JRTTimer;
public:
    JRTTimerData()
        : runnable(nullptr)
        , timeEvent(JRTTimer::TimeOneShot)
        , timerId(0)
        , wAccuracy(1)
        , wInterval(100)
        , isRunning(false)
    {

    }

    ~JRTTimerData();

    void init();
    bool start();
    void stop();
#ifdef _MSC_VER
    static void WINAPI onTimeCallback(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser,
                                      DWORD_PTR dw1, DWORD_PTR dw2);
#else
#endif

private:
    Icd::Runnable *runnable;
    JRTTimer::TimeEvent timeEvent;
#ifdef _MSC_VER
    MMRESULT timerId;
    UINT wAccuracy;
    UINT wInterval;
#elif defined(__linux__)
    unsigned int timerId;
    unsigned int wAccuracy;
    unsigned int wInterval;
#elif defined(__APPLE__)
    unsigned int timerId;
    unsigned int wAccuracy;
    unsigned int wInterval;
#endif
    bool isRunning;
};

JRTTimerData::~JRTTimerData()
{
    // 关闭定时器
    stop();
}

void JRTTimerData::init()
{
    //
}

bool JRTTimerData::start()
{
    //
    stop();
#ifdef _MSC_VER
    // 利用函数timeGetDevCaps取出系统分辨率的取值范围，如果正确则继续
    TIMECAPS tcaps;
    if (::timeGetDevCaps(&tcaps, sizeof(TIMECAPS)) == TIMERR_NOERROR) {
        // 获取分辨率
        wAccuracy = qMin<UINT>(qMax<UINT>(tcaps.wPeriodMin, wAccuracy), tcaps.wPeriodMax);
        // 调用timeBeginPeriod函数设置定时器的分辨率
        //::timeBeginPeriod(wAccuracy);
        UINT _timeEvent = (timeEvent == JRTTimer::TimePeriodic) ? TIME_PERIODIC : TIME_ONESHOT;
        // 设置定时器
        timerId = ::timeSetEvent(wInterval/*1ms和2s时钟间隔，单位为ms*/,
                                 wAccuracy, LPTIMECALLBACK(onTimeCallback),
                                 DWORD_PTR(this), _timeEvent);
        if (timerId == 0) {
            Q_ASSERT(false);
            return false;
        }
    } else {
        Q_ASSERT(false);
        return false;
    }
#elif defined(__linux__)
    //TODO
#elif defined(__APPLE__)
    //TODO
#endif
    //
    isRunning = true;

    return true;
}

void JRTTimerData::stop()
{
    if (isRunning) {
        // 释放定时器
#ifdef _MSC_VER
        if (timerId != NULL) {
            ::timeKillEvent(timerId);
        }
        // 删除设置的定时器分辨率
        //::timeEndPeriod(wAccuracy);
#elif defined(__linux__)
#elif defined(__APPLE__)
#endif
        //
        isRunning = false;
    }
}
#ifdef _MSC_VER
void WINAPI JRTTimerData::onTimeCallback(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser,
                                         DWORD_PTR dw1, DWORD_PTR dw2)
{
    Q_UNUSED(uTimerID);
    Q_UNUSED(uMsg);
    Q_UNUSED(dw1);
    Q_UNUSED(dw2);

    //
    JRTTimerData *_this = reinterpret_cast<JRTTimerData *>(dwUser);
    if (_this == nullptr) {
        Q_ASSERT(false);        //
        return;
    }

    //
    if (_this->runnable) {
        _this->runnable->run();    //
    }
}
#elif defined(__linux__)
//TODO
#elif defined(__APPLE__)
//TODO
#endif
// class JRTTimer

JRTTimer::JRTTimer()
    : d(new JRTTimerData())
{
    d->init();
}

JRTTimer::JRTTimer(JRTTimer::TimeEvent timeEvent, int interval)
    : d(new JRTTimerData())
{
    d->init();
    d->timeEvent = timeEvent;
    d->wInterval = static_cast<unsigned int>(interval);
}

JRTTimer::~JRTTimer()
{

}

bool JRTTimer::isRunning() const
{
    return d->isRunning;
}

bool JRTTimer::start()
{
    return d->start();
}

void JRTTimer::stop()
{
    d->stop();
}

unsigned int JRTTimer::accuracy() const
{
    return static_cast<unsigned int>(d->wAccuracy);
}

void JRTTimer::setAccuracy(unsigned int value)
{
    //
    d->stop();
#ifdef _MSC_VER
    //
    if (value != d->wAccuracy) {
        //
        TIMECAPS tc;
        if (::timeGetDevCaps(&tc, sizeof(TIMECAPS)) == TIMERR_NOERROR) {
            UINT newValue = qMin<UINT>(qMax<UINT>(tc.wPeriodMin, value), tc.wPeriodMax);
            if (newValue != d->wAccuracy) {
                //
                d->wAccuracy = value;
                //
            }
        }
    }
#elif defined(__linux__)
    (void)value;
    //TODO
#elif defined(__APPLE__)
    (void)value;
    //TODO
#endif
}

unsigned int JRTTimer::interval() const
{
    return d->wInterval;
}

void JRTTimer::setInterval(unsigned int value)
{
    bool isRunning = d->isRunning;

    if (isRunning) {
        stop();
    }

    d->wInterval = value;

    if (isRunning) {
        start();
    }
}

JRTTimer::TimeEvent JRTTimer::timeEvent() const
{
    return d->timeEvent;
}

void JRTTimer::setTimeEvent(JRTTimer::TimeEvent value)
{
    bool isRunning = d->isRunning;

    if (isRunning) {
        stop();
    }

    d->timeEvent = value;

    if (isRunning) {
        start();
    }
}

Runnable *JRTTimer::task() const
{
    return d->runnable;
}

void JRTTimer::setTask(Runnable *task)
{
    d->stop();
    d->runnable = task;
}

} // end of namespace Icd
