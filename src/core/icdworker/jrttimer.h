#ifndef JRTTIMER_H
#define JRTTIMER_H

#include "icdworker_global.h"

namespace Icd {

class Runnable;
typedef std::shared_ptr<Runnable> RunnablePtr;

/**
 * @brief The Runnable class
 */
class Runnable
{
public:
    virtual ~Runnable() {}
    virtual void run() = 0;
};

class JRTTimer;
class JRTTimerData;
typedef std::shared_ptr<JRTTimer> JRTTimerPtr;

/**
 * @brief The JRTTimer class
 */
class ICDWORKER_EXPORT JRTTimer
{
public:
    /**
     * @brief The TimeEvent enum
     */
    enum TimeEvent {
        TimeOneShot,
        TimePeriodic
    };

    /**
     * @brief JRTTimer
     */
    explicit JRTTimer();

    /**
     * @brief JRTTimer
     * @param timeEvent
     * @param interval
     */
    explicit JRTTimer(TimeEvent timeEvent, int interval = -1);

    ~JRTTimer();

    /**
     * @brief isRunning
     * @return
     */
    bool isRunning() const;

    /**
     * @brief start
     * @return
     */
    bool start();

    /**
     * @brief stop
     */
    void stop();

    /**
     * @brief 获取定时器分辨率
     * @return 定时器分辨率
     */
    unsigned int accuracy() const;

    /**
     * @brief 设置定时器分辨率
     * @param value : 定时器分辨率值
     */
    void setAccuracy(unsigned int value);

    /**
     * @brief 获取定时周期
     * @return 定时周期。单位：ms
     */
    unsigned int interval() const;

    /**
     * @brief 设置定时周期
     * @param value : 定时周期。单位：ms
     */
    void setInterval(unsigned int value);

    /**
     * @brief 获取定时器触发事件类型
     * @return 定时器触发事件类型
     */
    TimeEvent timeEvent() const;

    /**
     * @brief 设置定时器触发事件类型
     * @param value : 定时器触发事件类型
     */
    void setTimeEvent(TimeEvent value);

    /**
     * @brief task
     * @return
     */
    Runnable *task() const;

    /**
     * @brief setTask
     * @param task
     */
    void setTask(Runnable *task);

private:
    JRTTimer(const JRTTimer &);
    JRTTimer &operator=(const JRTTimer &);
    std::shared_ptr<JRTTimerData> d;
};

} // end of namespace Icd

#endif // JRTTIMER_H
