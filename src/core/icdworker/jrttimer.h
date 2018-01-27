#ifndef JRTTIMER_H
#define JRTTIMER_H

#include "icdworker_global.h"

namespace Icd {

class Runnable;
typedef JHandlePtr<Runnable> RunnablePtr;

/**
 * @brief The Runnable class
 */
class Runnable
{
public:
    virtual void run() = 0;
};

class JRTTimer;
class JRTTimerData;
typedef JHandlePtr<JRTTimer> JRTTimerPtr;

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
    explicit JRTTimer(TimeEvent timeEvent, unsigned int interval = -1);

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
     * @brief ��ȡ��ʱ���ֱ���
     * @return ��ʱ���ֱ���
     */
    unsigned int accuracy() const;

    /**
     * @brief ���ö�ʱ���ֱ���
     * @param value : ��ʱ���ֱ���ֵ
     */
    void setAccuracy(unsigned int value);

    /**
     * @brief ��ȡ��ʱ����
     * @return ��ʱ���ڡ���λ��ms
     */
    unsigned int interval() const;

    /**
     * @brief ���ö�ʱ����
     * @param value : ��ʱ���ڡ���λ��ms
     */
    void setInterval(unsigned int value);

    /**
     * @brief ��ȡ��ʱ�������¼�����
     * @return ��ʱ�������¼�����
     */
    TimeEvent timeEvent() const;

    /**
     * @brief ���ö�ʱ�������¼�����
     * @param value : ��ʱ�������¼�����
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
    JHandlePtr<JRTTimerData> d;
};

} // end of namespace Icd

#endif // JRTTIMER_H
