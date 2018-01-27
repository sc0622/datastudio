#include "precomp.h"
#include "jrttimer.h"

//
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

namespace Icd {

// JRTTimerData

class JRTTimerData
{
    friend class JRTTimer;
public:
    JRTTimerData()
        : runnable(0)
        , timerId(0)
        , isRunning(false)
        , wAccuracy(1)
        , wInterval(100)
        , timeEvent(JRTTimer::TimeOneShot)
    {

    }

    ~JRTTimerData();

    void init();

    bool start();

    void stop();

    static void WINAPI onTimeCallback(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser,
                                      DWORD_PTR dw1, DWORD_PTR dw2);

private:
    Icd::Runnable *runnable;
    MMRESULT timerId;
    bool isRunning;
    UINT wAccuracy;
    UINT wInterval;
    JRTTimer::TimeEvent timeEvent;
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
    if (isRunning) {
        stop();
    }

    // 利用函数timeGetDevCaps取出系统分辨率的取值范围，如果正确则继续
    TIMECAPS tcaps;
    if (::timeGetDevCaps(&tcaps, sizeof(TIMECAPS)) == TIMERR_NOERROR) {
        // 获取分辨率
        wAccuracy = qMin<UINT>(qMax<UINT>(tcaps.wPeriodMin, wAccuracy), tcaps.wPeriodMax);
        // 调用timeBeginPeriod函数设置定时器的分辨率
        //::timeBeginPeriod(wAccuracy);
        // 设置定时器
        timerId = ::timeSetEvent(wInterval/*1ms和2s时钟间隔，单位为ms*/,
                                 wAccuracy, (LPTIMECALLBACK)onTimeCallback,
                                 (DWORD_PTR)this, TIME_PERIODIC);
        if (timerId == 0) {
            Q_ASSERT(false);
            return false;
        }
    } else {
        Q_ASSERT(false);
        return false;
    }

    //
    isRunning = true;

    return true;
}

void JRTTimerData::stop()
{
    if (isRunning) {
        // 释放定时器
        if (timerId != NULL) {
            ::timeKillEvent(timerId);
        }

        // 删除设置的定时器分辨率
        //::timeEndPeriod(wAccuracy);

        //
        isRunning = false;
    }
}

void WINAPI JRTTimerData::onTimeCallback(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser,
                                         DWORD_PTR dw1, DWORD_PTR dw2)
{
    Q_UNUSED(uTimerID);
    Q_UNUSED(uMsg);
    Q_UNUSED(dw1);
    Q_UNUSED(dw2);

    //
    JRTTimerData *_this = reinterpret_cast<JRTTimerData *>(dwUser);
    if (_this == NULL) {
        Q_ASSERT(false);        //
        return;
    }

    //
    if (_this->runnable) {
        _this->runnable->run();    //
    }
}

// class JRTTimer

JRTTimer::JRTTimer()
    : d(new JRTTimerData())
{
    d->init();
}

JRTTimer::JRTTimer(JRTTimer::TimeEvent timeEvent, unsigned int interval)
    : d(new JRTTimerData())
{
    d->init();
    d->timeEvent = timeEvent;
    d->wInterval = interval;
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
    return (unsigned int)d->wAccuracy;
}

void JRTTimer::setAccuracy(unsigned int value)
{
    //
    d->stop();

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
}

unsigned int JRTTimer::interval() const
{
    return d->wInterval;
}

void JRTTimer::setInterval(unsigned int value)
{
    bool isRunning = d->isRunning;

    //
    if (isRunning) {
        stop();
    }

    //
    d->wInterval = value;

    //
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

    //
    if (isRunning) {
        stop();
    }

    //
    d->timeEvent = value;

    //
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
    //
    d->stop();

    //
    d->runnable = task;
}

} // end of namespace Icd
